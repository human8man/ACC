#include "SkinMesh.h"
#include "DirectX/DirectX9.h"
#include "DirectX/DirectX11.h"
#include "Camera/Camera.h"

#include <stdlib.h>
#include <locale.h>
#include <crtdbg.h>
#include <memory>
#include <algorithm>


// シェーダ名(ディレクトリも含む)
const TCHAR SHADER_NAME[] = _T( "Data\\Shader\\SkinMesh.hlsl" );

// コンスタントバッファを設定するスロット番号
enum enCBSlot
{
	Mesh,		// メッシュ
	Material,	// マテリアル
	Frame,		// フレーム
	Bones,		// ボーン
};


SkinMesh::SkinMesh()
	: m_pDx9				( nullptr )
	, m_pDevice9			( nullptr )

	, m_pDx11				( nullptr )
	, m_pDevice11			( nullptr )
	, m_pContext11			( nullptr )

	, m_pVertexShader		( nullptr )
	, m_pVertexLayout		( nullptr )
	, m_pPixelShader		( nullptr )
	, m_pSampleLinear		( nullptr )

	, m_pCBufferPerMesh		( nullptr )
	, m_pCBufferPerMaterial	( nullptr )
	, m_pCBufferPerFrame	( nullptr )
	, m_pCBufferPerBone		( nullptr )

	, m_mWorld				()
	, m_mRotation			()

	, m_mView				()
	, m_mProj				()

	, m_Light				()
	, m_CamPos				()

	, m_AnimSpeed			( 0.001 )
	, m_AnimTime			()

	, m_pD3dxMesh			( nullptr )

	, m_FilePath			()
	, m_Frame				()
{
}
SkinMesh::~SkinMesh()
{
	// 解放処理
	Release();

	// シェーダやサンプラ関係
	SAFE_RELEASE( m_pSampleLinear );
	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );

	// コンスタントバッファ関係
	SAFE_RELEASE( m_pCBufferPerBone );
	SAFE_RELEASE( m_pCBufferPerFrame );
	SAFE_RELEASE( m_pCBufferPerMaterial );
	SAFE_RELEASE( m_pCBufferPerMesh );

	SAFE_RELEASE( m_pD3dxMesh );

	// Dx9 デバイス関係
	m_pDevice9	= nullptr;
	m_pDx9		= nullptr;

	// Dx11 デバイス関係
	m_pContext11= nullptr;
	m_pDevice11	= nullptr;
	m_pDx11		= nullptr;
}


//=============================================================================
//		初期化
//=============================================================================
HRESULT SkinMesh::Init( LPCTSTR FileName )
{
	m_pDx9 = DirectX9::GetInstance();
	m_pDevice9 = m_pDx9->GetDevice();

	m_pDx11 = DirectX11::GetInstance();
	m_pDevice11 = m_pDx11->GetDevice();
	m_pContext11 = m_pDx11->GetContext();

	// モデル読み込み
	if( FAILED( LoadXMesh( FileName ) ) ) { return E_FAIL; }
	
	// シェーダの作成
	if( FAILED( CreateShader() ) ) { return E_FAIL; }
	
	// コンスタントバッファ(メッシュごと)
	if( FAILED( CreateCBuffer( &m_pCBufferPerMesh,		sizeof( CBUFFER_PER_MESH ) ) ) )	{ return E_FAIL; }
	
	// コンスタントバッファ(メッシュごと)
	if( FAILED( CreateCBuffer( &m_pCBufferPerMaterial,	sizeof( CBUFFER_PER_MATERIAL ) ) ) ){ return E_FAIL; }
	
	// コンスタントバッファ(メッシュごと)
	if( FAILED( CreateCBuffer( &m_pCBufferPerFrame,		sizeof( CBUFFER_PER_FRAME ) ) ) )	{ return E_FAIL; }
	
	// コンスタントバッファ(メッシュごと)
	if( FAILED( CreateCBuffer( &m_pCBufferPerBone,		sizeof( CBUFFER_PER_BONES ) ) ) )	{ return E_FAIL; }

	// テクスチャー用サンプラー作成
	if( FAILED( CreateSampler( &m_pSampleLinear ) ) ){ return E_FAIL; }

	return S_OK;
}


//-----------------------------------------------------------------------------
//		シェーダ初期化
//-----------------------------------------------------------------------------
HRESULT	SkinMesh::CreateShader()
{
	// D3D11関連の初期化
	ID3DBlob *pCompiledShader = nullptr;
	ID3DBlob *pErrors = nullptr;
	UINT	uCompileFlag = 0;

if(ISDEBUG){
	uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
}

	// HLSLからバーテックスシェーダのブロブを作成
	if( FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME, nullptr, nullptr,
			"VS_Main", "vs_5_0",
			uCompileFlag, 0, nullptr,
			&pCompiledShader, &pErrors, nullptr ) ) )
	{
		_ASSERT_EXPR( false, L"hlsl読み込み失敗" );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );

	
	// 上記で作成したブロブからバーテックスシェーダを作成
	if( FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr, &m_pVertexShader ) ) )
	{
		SAFE_RELEASE( pCompiledShader );
		_ASSERT_EXPR( false, L"バーテックスシェーダ作成失敗" );
		return E_FAIL;
	}


	// 頂点インプットレイアウトを定義	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,							  0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX",	0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT",0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof( layout ) / sizeof( layout[0] );


	// 頂点インプットレイアウトを作成
	if( FAILED(
		m_pDevice11->CreateInputLayout(
			layout, numElements, pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(), &m_pVertexLayout ) ) )
	{
		_ASSERT_EXPR( false, L"頂点インプットレイアウト作成失敗" );
		return E_FAIL;
	}
	// 頂点インプットレイアウトをセット
	m_pContext11->IASetInputLayout( m_pVertexLayout );


	// HLSLからピクセルシェーダのブロブを作成
	if(FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME, nullptr, nullptr,
			"PS_Main", "ps_5_0",
			uCompileFlag, 0, nullptr,
			&pCompiledShader, &pErrors, nullptr ) ) )
	{
		_ASSERT_EXPR( false, L"hlsl読み込み失敗" );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );

	// 上記で作成したブロブからピクセルシェーダを作成
	if( FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr, &m_pPixelShader ) ) )
	{
		SAFE_RELEASE( pCompiledShader );
		_ASSERT_EXPR( false, L"ピクセルシェーダ作成失敗" );
		return E_FAIL;
	}
	SAFE_RELEASE( pCompiledShader );

	return S_OK;
}


//-----------------------------------------------------------------------------
//		Xファイルからスキン関連の情報を読み出す関数
//-----------------------------------------------------------------------------
HRESULT SkinMesh::ReadSkinInfo( MYMESHCONTAINER* pContainer, VERTEX* pVB, SKIN_PARTS_MESH* pParts )
{
	// Xファイルから抽出すべき情報は、
	//	「頂点ごとのボーンインデックス」「頂点ごとのボーンウェイト」
	//	「バインド行列」「ポーズ行列」の4項目

	int i, k, m, n;			// 各種ループ変数
	int NumVertex	= 0;	// 頂点数
	int NumBone		= 0;	// ボーン数

	// 頂点数
	NumVertex	= m_pD3dxMesh->GetNumVertices( pContainer );
	// ボーン数
	NumBone		= m_pD3dxMesh->GetNumBones( pContainer );

	// それぞれのボーンに影響を受ける頂点を調べる
	//	そこから逆に、頂点ベースでボーンインデックス・重みを整頓する
	for( i = 0; i < NumBone; i++ )
	{
		// このボーンに影響を受ける頂点数
		auto NumIndex = m_pD3dxMesh->GetNumBoneVertices( pContainer, i );

		auto pIndex	 = std::make_unique<int[]>( NumIndex );
		auto pWeight = std::make_unique<double[]>( NumIndex );

		for( k = 0; k < NumIndex; k++ )
		{
			pIndex[k]	= m_pD3dxMesh->GetBoneVerticesIndices( pContainer, i, k );
			pWeight[k]	= m_pD3dxMesh->GetBoneVerticesWeights( pContainer, i, k );
		}

		// 頂点側からインデックスをたどって、頂点サイドで整理する
		for( k = 0; k < NumIndex; k++ )
		{
			// XファイルやCGソフトがボーン4本以内とは限らない
			//	5本以上の場合は、重みの大きい順に4本に絞る
			VERTEX* pV = &pVB[pIndex[k]];

			// ウェイトの大きさ順にソート(バブルソート)
			for( m = 4; m > 1; m-- )
			{
				for( n = 1; n < m; n++ )
				{
					if( pV->BoneWeight[n - 1] < pV->BoneWeight[n] )
					{
						std::swap( pV->BoneWeight[n - 1], pV->BoneWeight[n] );
						std::swap( pV->BoneIndex[ n - 1], pV->BoneIndex[ n] );
					}
				}
			}
			// ソート後は、最後の要素に一番小さいウェイトが入ってるはず
			bool flag = false;
			for( m = 0; m < 4; m++ )
			{
				if( pV->BoneWeight[m] == 0 )
				{
					flag = true;
					pV->BoneIndex[m] = i;
					pV->BoneWeight[m] = static_cast<float>( pWeight[k] );
					break;
				}
			}
			if( flag == false )
			{
				pV->BoneIndex[3] = i;
				pV->BoneWeight[3] = static_cast<float>( pWeight[k] );
				break;
			}
		}
	}


	// ボーン生成
	pParts->NumBone	= NumBone;
	pParts->pBoneArray	= new BONE[NumBone]();

	// ポーズ行列を得る(初期ポーズ)
	for( i = 0; i < pParts->NumBone; i++ )
	{
		pParts->pBoneArray[i].mBindPose = m_pD3dxMesh->GetBindPose( pContainer, i );
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//		Xからスキンメッシュを作成する(素材（X)のほうは、三角ポリゴンにすること)
//-----------------------------------------------------------------------------
HRESULT SkinMesh::LoadXMesh( LPCTSTR FileName )
{
	//ファイル名をパスごと取得
	lstrcpy( m_FilePath, FileName );

	//Xファイル読み込み
	m_pD3dxMesh = new D3DXPARSER();
	m_pD3dxMesh->LoadMeshFromX( m_pDevice9, FileName );


	//全てのメッシュを作成する
	BuildAllMesh( m_pD3dxMesh->m_pFrameRoot );

	return S_OK;
}


//-----------------------------------------------------------------------------
//		Direct3Dのインデックスバッファー作成
//-----------------------------------------------------------------------------
HRESULT SkinMesh::CreateIndexBuffer(
	DWORD Size, int* pIndex, ID3D11Buffer** ppIndexBuffer )
{
	D3D11_BUFFER_DESC bd;
	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.ByteWidth		= Size;
	bd.BindFlags		= D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags	= 0;
	bd.MiscFlags		= 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem			= pIndex;
	InitData.SysMemPitch		= 0;
	InitData.SysMemSlicePitch	= 0;

	if( FAILED(
		m_pDevice11->CreateBuffer( &bd, &InitData, ppIndexBuffer ) ) )
	{
		return E_FAIL;
	}
	
	return S_OK;
}


//=============================================================================
//		レンダリング
//=============================================================================
void SkinMesh::Render(
	const D3DXMATRIX& mView, 
	const D3DXMATRIX& mProj, 
	const LIGHT& Light, 
	LPD3DXANIMATIONCONTROLLER pAC )
{
	m_mView		= mView;
	m_mProj		= mProj;
	m_Light		= Light;
	m_CamPos	= Camera::GetInstance()->GetPos();

	if ( pAC == nullptr )
	{
		if ( m_pD3dxMesh->m_pAnimController )
		{
			m_pD3dxMesh->m_pAnimController->AdvanceTime( m_AnimSpeed, nullptr );
		}
	}
	else
	{
		pAC->AdvanceTime( m_AnimSpeed, nullptr );
	}

	D3DXMATRIX m;
	D3DXMatrixIdentity( &m );
	m_pD3dxMesh->UpdateFrameMatrices( m_pD3dxMesh->m_pFrameRoot, &m );
	DrawFrame( m_pD3dxMesh->m_pFrameRoot );
}


//-----------------------------------------------------------------------------
//		全てのメッシュを作成する
//-----------------------------------------------------------------------------
void SkinMesh::BuildAllMesh( D3DXFRAME* pFrame )
{
	if( pFrame && pFrame->pMeshContainer )
	{
		CreateAppMeshFromD3DXMesh( pFrame );
	}

	// 再帰関数
	if( pFrame->pFrameSibling != nullptr )
	{
		BuildAllMesh( pFrame->pFrameSibling );
	}
	if( pFrame->pFrameFirstChild != nullptr )
	{
		BuildAllMesh( pFrame->pFrameFirstChild );
	}
}


//-----------------------------------------------------------------------------
//		メッシュ作成
//-----------------------------------------------------------------------------
HRESULT SkinMesh::CreateAppMeshFromD3DXMesh( LPD3DXFRAME p )
{
	MYFRAME* pFrame = reinterpret_cast<MYFRAME*>( p );

	//	LPD3DXMESH pD3DXMesh = pFrame->pMeshContainer->MeshData.pMesh;// D3DXメッシュ(ここから・・・)
	MYMESHCONTAINER* pContainer = reinterpret_cast<MYMESHCONTAINER*>( pFrame->pMeshContainer );

	// アプリメッシュ(・・・ここにメッシュデータをコピーする)
	SKIN_PARTS_MESH* pAppMesh = new SKIN_PARTS_MESH();
	pAppMesh->EnableTexture = false;

	// 事前に頂点数、ポリゴン数等を調べる
	pAppMesh->NumVert	= m_pD3dxMesh->GetNumVertices( pContainer );
	pAppMesh->NumFace	= m_pD3dxMesh->GetNumFaces( pContainer );
	pAppMesh->NumUV		= m_pD3dxMesh->GetNumUVs( pContainer );
	// Direct3DではUVの数だけ頂点が必要
	if( pAppMesh->NumVert < pAppMesh->NumUV ) {
		//共有頂点等で、頂点が足りないとき
		_ASSERT_EXPR( false, L"Direct3Dは、UVの数だけ頂点が必要です(UVを置く場所が必要です)テクスチャは正しく貼られないと思われます" );
		return E_FAIL;
	}
	// 一時的なメモリ確保(頂点バッファとインデックスバッファ)
	auto pVB = std::make_unique<VERTEX[]>( pAppMesh->NumVert );
	auto pFaceBuffer = std::make_unique<int[]>( pAppMesh->NumFace * 3 );
	// 3頂点ポリゴンなので、1フェイス=3頂点(3インデックス)

	// 頂点読み込み
	for( DWORD i = 0; i < pAppMesh->NumVert; i++ ){
		pVB[i].Position = m_pD3dxMesh->GetVertexCoord( pContainer, i );
	}
	// ポリゴン情報(頂点インデックス)読み込み
	for( DWORD i = 0; i < pAppMesh->NumFace*3; i++ ){
		pFaceBuffer[i] = m_pD3dxMesh->GetIndex( pContainer, i );
	}
	// 法線読み込み
	for( DWORD i = 0; i < pAppMesh->NumVert; i++ ){
		pVB[i].vNormal	= m_pD3dxMesh->GetNormal( pContainer, i);
	}
	// テクスチャ座標読み込み
	for( DWORD i = 0; i < pAppMesh->NumVert; i++ ){
		pVB[i].Texture = m_pD3dxMesh->GetUV( pContainer, i );
	}

	// マテリアル読み込み
	pAppMesh->NumMaterial	= m_pD3dxMesh->GetNumMaterials( pContainer );
	pAppMesh->pMaterial		= new MY_SKINMATERIAL[pAppMesh->NumMaterial]();

	// マテリアルの数だけインデックスバッファを作成
	pAppMesh->ppIndexBuffer = new ID3D11Buffer*[pAppMesh->NumMaterial]();
	// 掛け算ではなく「ID3D11Buffer*」の配列という意味
	for( DWORD i = 0; i < pAppMesh->NumMaterial; i++ )
	{
		// 環境光(アンビエント)
		pAppMesh->pMaterial[i].Ambient	= m_pD3dxMesh->GetAmbient( pContainer, i );
		// 拡散反射光(ディフューズ)
		pAppMesh->pMaterial[i].Diffuse	= m_pD3dxMesh->GetDiffuse( pContainer, i );
		// 鏡面反射光(スペキュラ)
		pAppMesh->pMaterial[i].Specular	= m_pD3dxMesh->GetSpecular( pContainer, i );
		// 自己発光(エミッシブ)
		pAppMesh->pMaterial[i].Emissive	= m_pD3dxMesh->GetEmissive( pContainer, i );
		// スペキュラパワー
		pAppMesh->pMaterial[i].SpecularPower = m_pD3dxMesh->GetSpecularPower( pContainer, i );

		// アンビエントが0だと光源からの直射光が全く当たらない陰の部分の明るさが、
		//	真っ暗になるので、最低値と最高値を設定する
		float low	= 0.3f;	// 適当に0.3くらいにしておく
		float high	= 1.f;
		pAppMesh->pMaterial[i].Ambient.x = std::clamp( pAppMesh->pMaterial[i].Ambient.x, low, high );
		pAppMesh->pMaterial[i].Ambient.y = std::clamp( pAppMesh->pMaterial[i].Ambient.y, low, high );
		pAppMesh->pMaterial[i].Ambient.z = std::clamp( pAppMesh->pMaterial[i].Ambient.z, low, high );
		// なお、x,y,z(r,g,b)のみ対処して、w(a)はそのまま使用する

#if 0
		// テクスチャ(ディフューズテクスチャのみ)
#ifdef UNICODE
		WCHAR TexFilename_w[32] = L"";
		// テクスチャ名のサイズを取得
		size_t charSize = strlen( m_pD3dxMesh->GetTexturePath( pContainer, i ) ) + 1;
		size_t ret;	// 変換された文字数

		// マルチバイト文字のシーケンスを対応するワイド文字のシーケンスに変換します
		errno_t err = mbstowcs_s(
			&ret,
			TexFilename_w,
			charSize,
			m_pD3dxMesh->GetTexturePath( pContainer, i ),
			_TRUNCATE );

		LPTSTR name = TexFilename_w;
#else// #ifdef UNICODE
		LPTSTR name = d3dxMaterials[No].pTextureFilename;
#endif// #ifdef UNICODE
#endif

		// テクスチャ(ディフューズテクスチャのみ)
#ifdef UNICODE
		WCHAR TextureName_w[32] = L"";
		//文字変換
		ConvertCharaMultiByteToUnicode( TextureName_w, 32, m_pD3dxMesh->GetTexturePath( pContainer, i ));
		LPTSTR name = TextureName_w;
#else// #ifdef UNICODE
		LPTSTR name = m_pD3dxMesh->GetTexturePath( pContainer, i );
#endif// #ifdef UNICODE

		if( name != nullptr ){
			LPTSTR ret = _tcsrchr( m_FilePath, _T( '\\' ) );
			if( ret != nullptr ){
				int check = static_cast<int>( ret - m_FilePath );
				TCHAR path[512];
				lstrcpy( path, m_FilePath );
				path[check + 1] = '\0';

				// パスのコピー
				lstrcpy( pAppMesh->pMaterial[i].TextureName, path );
				// テクスチャファイル名を連結
				lstrcat( pAppMesh->pMaterial[i].TextureName, name );
			}
		}
		// テクスチャを作成
		if( pAppMesh->pMaterial[i].TextureName[0] != 0
			&& FAILED(
				D3DX11CreateShaderResourceViewFromFile(
				m_pDevice11, pAppMesh->pMaterial[i].TextureName,
				nullptr, nullptr, &pAppMesh->pMaterial[i].pTexture, nullptr ) ))
		{
			_ASSERT_EXPR( false, L"テクスチャ作成失敗" );
			return E_FAIL;
		}
		// そのマテリアルであるインデックス配列内の開始インデックスを調べる
		//	さらにインデックスの個数を調べる
		int count = 0;
		auto pIndex = std::make_unique<int[]>( pAppMesh->NumFace * 3 );
			// とりあえず、メッシュ内のポリゴン数でメモリ確保
			//	(ここのポリゴングループは必ずこれ以下になる)

		for( DWORD k = 0; k < pAppMesh->NumFace; k++ )
		{
			// もし i==k 番目のポリゴンのマテリアル番号なら
			if( i == m_pD3dxMesh->GeFaceMaterialIndex( pContainer, k ))
			{
				// k番目のポリゴンを作る頂点のインデックス
				pIndex[count]	  = m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 0 ); // 1個目
				pIndex[count + 1] = m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 1 ); // 2個目
				pIndex[count + 2] = m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 2 ); // 3個目
				count += 3;
			}
		}
		if( count > 0 ){
			// インデックスバッファ作成
			CreateIndexBuffer( count * sizeof( int ), pIndex.get(), &pAppMesh->ppIndexBuffer[i]);
		}
		else{
			// 解放時の処理に不具合が出たため
			//	カウント数が0以下の場合は、インデックスバッファを nullptr にしておく
			pAppMesh->ppIndexBuffer[i] = nullptr;
		}

		// そのマテリアル内のポリゴン数
		pAppMesh->pMaterial[i].NumFace = count / 3;

	}

	// スキン情報ある？
	if( pContainer->pSkinInfo == nullptr ){
		if (ISDEBUG) {
			// 不明なスキンあればここで教える.不要ならコメントアウトしてください
			TCHAR strDbg[128];
			WCHAR str[64] = L"";
			ConvertCharaMultiByteToUnicode(str, 64, pContainer->Name);
			_stprintf_s(strDbg, _T("ContainerName:[%s]"), str);
			MessageBox(nullptr, strDbg, _T("Not SkinInfo"), MB_OK);
		}
		pAppMesh->EnableBones = false;
	}
	else{
		// スキン情報(ジョイント、ウェイト)読み込み
		ReadSkinInfo( pContainer, pVB.get(), pAppMesh );
	}

	// バーテックスバッファを作成
	D3D11_BUFFER_DESC bd;
	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.ByteWidth		= sizeof( VERTEX ) * ( pAppMesh->NumVert );
	bd.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags	= 0;
	bd.MiscFlags		= 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pVB.get();

	HRESULT hRslt = S_OK;
	if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, &pAppMesh->pVertexBuffer ) ) )
	{
		hRslt = E_FAIL;
	}

	// パーツメッシュに設定
	pFrame->pPartsMesh = pAppMesh;

	return hRslt;
}


//-----------------------------------------------------------------------------
//		ボーンを次のポーズ位置にセットする関数
//-----------------------------------------------------------------------------
void SkinMesh::SetNewPoseMatrices( SKIN_PARTS_MESH* pParts, int Frame, MYMESHCONTAINER* pContainer )
{
	// 望むフレームでUpdateすること、しないと行列が更新されない
	// m_pD3dxMesh->UpdateFrameMatrices(m_pD3dxMesh->m_pFrameRoot)
	// をレンダリング時に実行すること

	// また、アニメーション時間に見合った行列を更新するのはD3DXMESHでは
	// アニメーションコントローラが(裏で)やってくれるものなので、
	// アニメーションコントローラを使ってアニメを進行させることも必要
	// m_pD3dxMesh->m_pAnimController->AdvanceTime(...)を
	// レンダリング時に実行すること

	if( pParts->NumBone <= 0 ){
		//ボーンが 0　以下の場合
	}

	for( int i = 0; i < pParts->NumBone; i++ )
	{
		pParts->pBoneArray[i].mNewPose = m_pD3dxMesh->GetNewPose( pContainer, i );
	}
}


//-----------------------------------------------------------------------------
//		次の(現在の)ポーズ行列を返す関数
//-----------------------------------------------------------------------------
D3DXMATRIX SkinMesh::GetCurrentPoseMatrix( SKIN_PARTS_MESH* pParts, int Index )
{
	D3DXMATRIX ret = pParts->pBoneArray[Index].mBindPose * pParts->pBoneArray[Index].mNewPose;
	return ret;
}


//-----------------------------------------------------------------------------
//		フレームの描画
//-----------------------------------------------------------------------------
VOID SkinMesh::DrawFrame( LPD3DXFRAME p )
{
	MYFRAME*			pFrame	= reinterpret_cast<MYFRAME*>( p );
	SKIN_PARTS_MESH*	pPartsMesh	= pFrame->pPartsMesh;
	MYMESHCONTAINER*	pContainer	= reinterpret_cast<MYMESHCONTAINER*>( pFrame->pMeshContainer );

	if( pPartsMesh != nullptr )
	{
		DrawPartsMesh( pPartsMesh,  pFrame->CombinedTransformationMatrix, pContainer );
	}

	// 再帰関数
	// (兄弟)
	if( pFrame->pFrameSibling != nullptr )
	{
		DrawFrame( pFrame->pFrameSibling );
	}
	// (親子)
	if( pFrame->pFrameFirstChild != nullptr )
	{
		DrawFrame( pFrame->pFrameFirstChild );
	}
}
#define SEND_CB_TO_FUNC


//-----------------------------------------------------------------------------
//		パーツメッシュを描画
//-----------------------------------------------------------------------------
void SkinMesh::DrawPartsMesh(
	SKIN_PARTS_MESH* pMesh, D3DXMATRIX World, MYMESHCONTAINER* pContainer )
{
	// ワールド行列算出
	CalcWorldMatrix();

	// アニメーションフレームを進める スキンを更新
	m_Frame++;
	if( m_Frame >= 3600 ){ m_Frame = 0; }
	SetNewPoseMatrices( pMesh, m_Frame, pContainer );

	// コンスタントバッファに情報を送る(ボーン)
	SendCBufferPerBone( pMesh );

	// コンスタントバッファに情報を設定(フレームごと)
	SendCBufferPerFrame();

	// コンスタントバッファに情報を設定(メッシュごと)
	SendCBufferPerMesh();

	// バーテックスバッファをセット
	UINT stride = sizeof( VERTEX );
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &pMesh->pVertexBuffer, &stride, &offset );

	// 使用するシェーダのセット
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );

	// 頂点インプットレイアウトをセット
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// プリミティブ・トポロジーをセット
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// マテリアルの数だけ、
	//	それぞれのマテリアルのインデックスバッファを描画
	for( DWORD i = 0; i < pMesh->NumMaterial; i++ )
	{
		// 使用されていないマテリアル対策
		if( pMesh->pMaterial[i].NumFace == 0 ) { continue; }

		// インデックスバッファをセット
		stride	= sizeof( int );
		offset	= 0;
		m_pContext11->IASetIndexBuffer( pMesh->ppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0 );

		// コンスタントバッファに情報を設定（マテリアルごと）
		SendCBufferPerMaterial( &pMesh->pMaterial[i] );

		// テクスチャをシェーダに渡す
		SendTexture( &pMesh->pMaterial[i] );

		// 描画
		m_pContext11->DrawIndexed( pMesh->pMaterial[i].NumFace * 3, 0, 0 );
	}
}


//-----------------------------------------------------------------------------
//		ワールド行列算出
//-----------------------------------------------------------------------------
void SkinMesh::CalcWorldMatrix()
{
	D3DXMATRIX	mScale, mYaw, mPitch, mRoll, mTran;
	// 拡縮
	D3DXMatrixScaling( &mScale, m_vScale.x, m_vScale.y, m_vScale.z );

	// 回転
	D3DXMatrixRotationY( &mYaw, m_vRotation.y );		// Y軸回転
	D3DXMatrixRotationX( &mPitch, m_vRotation.x );	// X軸回転
	D3DXMatrixRotationZ( &mRoll, m_vRotation.z );	// Z軸回転
	m_mRotation = mYaw * mPitch * mRoll;

	// 平行移動
	D3DXMatrixTranslation( &mTran, m_vPosition.x, m_vPosition.y, m_vPosition.z );

	// ワールド行列
	m_mWorld = mScale * m_mRotation * mTran;
}


//-----------------------------------------------------------------------------
//		コンスタントバッファ（ボーン）をシェーダに渡す
//-----------------------------------------------------------------------------
void SkinMesh::SendCBufferPerBone(	SKIN_PARTS_MESH* pMesh )
{
	D3D11_MAPPED_SUBRESOURCE pData;

	if( SUCCEEDED( m_pContext11->Map( m_pCBufferPerBone, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ) ) ) 
	{
		CBUFFER_PER_BONES cb;

		for( int i = 0; i < pMesh->NumBone; i++ )
		{
			D3DXMATRIX mat = GetCurrentPoseMatrix( pMesh, i );
			D3DXMatrixTranspose( &mat, &mat );
			cb.mBone[i] = mat;
		}
		memcpy_s( pData.pData, pData.RowPitch, reinterpret_cast<void*>( &cb ), sizeof( cb ) );

		m_pContext11->Unmap( m_pCBufferPerBone, 0 );
	}
	m_pContext11->VSSetConstantBuffers( enCBSlot::Bones, 1, &m_pCBufferPerBone );
	m_pContext11->PSSetConstantBuffers( enCBSlot::Bones, 1, &m_pCBufferPerBone );
}


//-----------------------------------------------------------------------------
//		コンスタントバッファ（フレーム）をシェーダに渡す
//-----------------------------------------------------------------------------
void SkinMesh::SendCBufferPerFrame()
{
	D3D11_MAPPED_SUBRESOURCE pData;

	if( SUCCEEDED( m_pContext11->Map( m_pCBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ) ) )
	{
		CBUFFER_PER_FRAME cb;

		// カメラ位置
		cb.CameraPos = D3DXVECTOR4( m_CamPos.x, m_CamPos.y, m_CamPos.z, 0.f );
		// ライト方向
		cb.vLightDir = D3DXVECTOR4( m_Light.vDirection.x, m_Light.vDirection.y, m_Light.vDirection.z, 0.f );
		// ライト方向の正規化(ノーマライズ)
		//	※モデルからライトへ向かう方向. ディレクショナルライトで重要な要素
		D3DXVec4Normalize( &cb.vLightDir, &cb.vLightDir );

		memcpy_s( pData.pData, pData.RowPitch, reinterpret_cast<void*>( &cb ), sizeof( cb ) );

		m_pContext11->Unmap( m_pCBufferPerFrame, 0 );
	}
	m_pContext11->VSSetConstantBuffers( enCBSlot::Frame, 1, &m_pCBufferPerFrame );
	m_pContext11->PSSetConstantBuffers( enCBSlot::Frame, 1, &m_pCBufferPerFrame );
}


//-----------------------------------------------------------------------------
//		コンスタントバッファ（メッシュ）をシェーダに渡す
//-----------------------------------------------------------------------------
void SkinMesh::SendCBufferPerMesh()
{
	D3D11_MAPPED_SUBRESOURCE pDat;

	if( SUCCEEDED( m_pContext11->Map( m_pCBufferPerMesh, 0, D3D11_MAP_WRITE_DISCARD, 0, &pDat )))
	{
		CBUFFER_PER_MESH cb;
		cb.mW = m_mWorld;
		D3DXMatrixTranspose( &cb.mW, &cb.mW );		// 行列を転置する
		cb.mWVP = m_mWorld * m_mView * m_mProj;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );	// 行列を転置する
		// ※行列の計算方法がDirectXとGPUで異なるため転置が必要

		memcpy_s( pDat.pData, pDat.RowPitch, reinterpret_cast<void*>( &cb ), sizeof( cb ));

		m_pContext11->Unmap( m_pCBufferPerMesh, 0 );
	}

	m_pContext11->VSSetConstantBuffers( enCBSlot::Mesh, 1, &m_pCBufferPerMesh );
	m_pContext11->PSSetConstantBuffers( enCBSlot::Mesh, 1, &m_pCBufferPerMesh );
}


//-----------------------------------------------------------------------------
//		コンスタントバッファ（マテリアル）をシェーダに渡す
//-----------------------------------------------------------------------------
void SkinMesh::SendCBufferPerMaterial( MY_SKINMATERIAL* pMaterial )
{
	D3D11_MAPPED_SUBRESOURCE pDat;

	if( SUCCEEDED( m_pContext11->Map( m_pCBufferPerMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &pDat )))
	{
		CBUFFER_PER_MATERIAL cb;
		cb.Ambient	= pMaterial->Ambient;
		cb.Diffuse	= pMaterial->Diffuse;
		cb.Specular	= pMaterial->Specular;

		memcpy_s( pDat.pData, pDat.RowPitch, reinterpret_cast<void*>( & cb ), sizeof( cb ));

		m_pContext11->Unmap( m_pCBufferPerMaterial, 0 );
	}

	m_pContext11->VSSetConstantBuffers( enCBSlot::Material, 1, &m_pCBufferPerMaterial );
	m_pContext11->PSSetConstantBuffers( enCBSlot::Material, 1, &m_pCBufferPerMaterial );
}


//-----------------------------------------------------------------------------
//		テクスチャをシェーダに渡す
//-----------------------------------------------------------------------------
void SkinMesh::SendTexture( MY_SKINMATERIAL* pMaterial )
{
	if( pMaterial->TextureName != nullptr )
	{
		m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinear );
		m_pContext11->PSSetShaderResources( 0, 1, &pMaterial->pTexture );
	}
	else
	{
		ID3D11ShaderResourceView* Nothing[1] = { 0 };
		m_pContext11->PSSetShaderResources( 0, 1, Nothing );
	}
}


//=============================================================================
//		解放関数
//=============================================================================
HRESULT SkinMesh::Release()
{
	if( m_pD3dxMesh != nullptr ){
		// 全てのメッシュ削除
		DestroyAllMesh( m_pD3dxMesh->m_pFrameRoot );

		// パーサークラス解放処理
		m_pD3dxMesh->Release();
		SAFE_DELETE( m_pD3dxMesh );
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
//		全てのメッシュを削除
//-----------------------------------------------------------------------------
void SkinMesh::DestroyAllMesh( D3DXFRAME* pFrame )
{
	if ((pFrame != nullptr) && (pFrame->pMeshContainer != nullptr))
	{
		DestroyAppMeshFromD3DXMesh( pFrame );
	}

	// 再帰関数
	if( pFrame->pFrameSibling != nullptr )
	{
		DestroyAllMesh( pFrame->pFrameSibling );
	}
	if( pFrame->pFrameFirstChild != nullptr )
	{
		DestroyAllMesh( pFrame->pFrameFirstChild );
	}
}


//-----------------------------------------------------------------------------
//		メッシュの削除
//-----------------------------------------------------------------------------
HRESULT SkinMesh::DestroyAppMeshFromD3DXMesh( LPD3DXFRAME p )
{
	MYFRAME* pFrame = reinterpret_cast<MYFRAME*>(p);
	MYMESHCONTAINER* pMeshContainerTmp = reinterpret_cast<MYMESHCONTAINER*>(pFrame->pMeshContainer);

	// MYMESHCONTAINERのリソース解放
	if (pMeshContainerTmp != nullptr)
	{
		// ボーンバッファ解放
		if (pMeshContainerTmp->pBoneBuffer != nullptr)
		{
			pMeshContainerTmp->pBoneBuffer->Release();
			pMeshContainerTmp->pBoneBuffer = nullptr;
		}

		// ボーンオフセット行列解放
		if (pMeshContainerTmp->pBoneOffsetMatrices != nullptr)
		{
			delete pMeshContainerTmp->pBoneOffsetMatrices;
			pMeshContainerTmp->pBoneOffsetMatrices = nullptr;
		}

		// ボーンマトリックス配列解放
		if (pMeshContainerTmp->ppBoneMatrix != nullptr)
		{
			int iMax = static_cast<int>(pMeshContainerTmp->pSkinInfo->GetNumBones());
			for (int i = 0; i < iMax; i++)
			{
				pMeshContainerTmp->ppBoneMatrix[i] = nullptr;
			}
			delete[] pMeshContainerTmp->ppBoneMatrix;
			pMeshContainerTmp->ppBoneMatrix = nullptr;
		}

		// テクスチャ解放
		if (pMeshContainerTmp->ppTextures != nullptr)
		{
			int iMax = static_cast<int>(pMeshContainerTmp->NumMaterials);
			for (int i = 0; i < iMax; i++)
			{
				if (pMeshContainerTmp->ppTextures[i] != nullptr)
				{
					pMeshContainerTmp->ppTextures[i]->Release();
					pMeshContainerTmp->ppTextures[i] = nullptr;
				}
			}
			delete[] pMeshContainerTmp->ppTextures;
			pMeshContainerTmp->ppTextures = nullptr;
		}
	}

	pMeshContainerTmp = nullptr; // メッシュコンテナ解放完了

	// LPD3DXMESHCONTAINER のリソース解放
	if (pFrame->pMeshContainer != nullptr)
	{
		delete[] pFrame->pMeshContainer->pAdjacency;
		pFrame->pMeshContainer->pAdjacency = nullptr;

		// エフェクトデータ解放
		if (pFrame->pMeshContainer->pEffects != nullptr)
		{
			if (pFrame->pMeshContainer->pEffects->pDefaults != nullptr)
			{
				delete pFrame->pMeshContainer->pEffects->pDefaults->pParamName;
				delete pFrame->pMeshContainer->pEffects->pDefaults->pValue;
				delete pFrame->pMeshContainer->pEffects->pDefaults;
				pFrame->pMeshContainer->pEffects->pDefaults = nullptr;
			}
			delete pFrame->pMeshContainer->pEffects->pEffectFilename;
			delete pFrame->pMeshContainer->pEffects;
			pFrame->pMeshContainer->pEffects = nullptr;
		}

		// マテリアル解放
		if (pFrame->pMeshContainer->pMaterials != nullptr)
		{
			int iMax = static_cast<int>(pFrame->pMeshContainer->NumMaterials);
			for (int i = 0; i < iMax; i++)
			{
				delete[] pFrame->pMeshContainer->pMaterials[i].pTextureFilename;
			}
			delete[] pFrame->pMeshContainer->pMaterials;
			pFrame->pMeshContainer->pMaterials = nullptr;
		}
	}

	// スキン情報の解放
	if (pFrame->pMeshContainer->pSkinInfo != nullptr)
	{
		pFrame->pMeshContainer->pSkinInfo->Release();
		pFrame->pMeshContainer->pSkinInfo = nullptr;
	}

	// MYFRAME のリソース解放
	if (pFrame->pPartsMesh != nullptr)
	{
		// ボーン情報の解放
		delete[] pFrame->pPartsMesh->pBoneArray;
		pFrame->pPartsMesh->pBoneArray = nullptr;

		// マテリアルの解放
		if (pFrame->pPartsMesh->pMaterial != nullptr)
		{
			int iMax = static_cast<int>(pFrame->pPartsMesh->NumMaterial);
			for (int i = 0; i < iMax; i++)
			{
				if (pFrame->pPartsMesh->pMaterial[i].pTexture != nullptr)
				{
					pFrame->pPartsMesh->pMaterial[i].pTexture->Release();
					pFrame->pPartsMesh->pMaterial[i].pTexture = nullptr;
				}
			}
			delete[] pFrame->pPartsMesh->pMaterial;
			pFrame->pPartsMesh->pMaterial = nullptr;
		}
		
		// インデックスバッファの解放
		if (pFrame->pPartsMesh->ppIndexBuffer != nullptr)
		{
			for (DWORD i = 0; i < pFrame->pPartsMesh->NumMaterial; i++)
			{
				if (pFrame->pPartsMesh->ppIndexBuffer[i] != nullptr)
				{
					pFrame->pPartsMesh->ppIndexBuffer[i]->Release();
					pFrame->pPartsMesh->ppIndexBuffer[i] = nullptr;
				}
			}
			delete[] pFrame->pPartsMesh->ppIndexBuffer;
			pFrame->pPartsMesh->ppIndexBuffer = nullptr;
		}

		// 頂点バッファ解放
		if (pFrame->pPartsMesh->pVertexBuffer != nullptr)
		{
			pFrame->pPartsMesh->pVertexBuffer->Release();
			pFrame->pPartsMesh->pVertexBuffer = nullptr;
		}
	}

	// 最後にパーツメッシュを解放
	delete[] pFrame->pPartsMesh;
	pFrame->pPartsMesh = nullptr;

	return S_OK;
}


//=============================================================================
//		アニメーションセットの切り替え
//=============================================================================
void SkinMesh::ChangeAnimSet( int index, LPD3DXANIMATIONCONTROLLER pAC )
{
	if( m_pD3dxMesh == nullptr ){ return; }
	m_pD3dxMesh->ChangeAnimSet( index, pAC );
}


//=============================================================================
//		アニメーションセットの切り替え(開始フレーム指定可能版)
//=============================================================================
void SkinMesh::ChangeAnimSet_StartPos( int index, double dStartFramePos, LPD3DXANIMATIONCONTROLLER pAC )
{
	if( m_pD3dxMesh == nullptr ){ return; }
	m_pD3dxMesh->ChangeAnimSet_StartPos( index, dStartFramePos, pAC );
}


//=============================================================================
//		アニメーション停止時間を取得
//=============================================================================
double SkinMesh::GetAnimPeriod( int index )
{
	if( m_pD3dxMesh == nullptr ){ return 0.f; }
	return m_pD3dxMesh->GetAnimPeriod( index );
}


//=============================================================================
//		アニメーション数を取得
//=============================================================================
int SkinMesh::GetAnimMax( LPD3DXANIMATIONCONTROLLER pAC )
{
	if( m_pD3dxMesh != nullptr ){ return m_pD3dxMesh->GetAnimMax( pAC ); }
	return -1;
}


//=============================================================================
//		指定したボーン情報(行列)を取得する関数
//=============================================================================
bool SkinMesh::GetMatrixFromBone(
	LPCSTR sBoneName, D3DXMATRIX* pOutMat )
{
	if( m_pD3dxMesh != nullptr ){
		if( m_pD3dxMesh->GetMatrixFromBone( sBoneName, pOutMat ) ){ return true; }
	}
	return false;
}


//=============================================================================
//		指定したボーン情報(座標)を取得する関数
//=============================================================================
bool SkinMesh::GetPosFromBone( LPCSTR sBoneName, D3DXVECTOR3* pOutPos )
{
	if( m_pD3dxMesh != nullptr )
	{
		D3DXVECTOR3 tmpPos;
		if( m_pD3dxMesh->GetPosFromBone( sBoneName, &tmpPos ) ){
			D3DXMATRIX mWorld, mScale, mTran;
			D3DXMATRIX mRot, mYaw, mPitch, mRoll;
			D3DXMatrixScaling( &mScale, m_vScale.x, m_vScale.y, m_vScale.z );
			D3DXMatrixRotationY( &mYaw,		m_vRotation.y );
			D3DXMatrixRotationX( &mPitch,	m_vRotation.x );
			D3DXMatrixRotationZ( &mRoll,	m_vRotation.z );
			D3DXMatrixTranslation( &mTran, tmpPos.x, tmpPos.y, tmpPos.z );

			mRot = mYaw * mPitch * mRoll;
			mWorld = mTran * mScale * mRot;

			pOutPos->x = mWorld._41 + m_vPosition.x;
			pOutPos->y = mWorld._42 + m_vPosition.y;
			pOutPos->z = mWorld._43 + m_vPosition.z;

			return true;
		}
	}
	return false;
}


//=============================================================================
//		指定したボーン座標を取得する関数
//=============================================================================
bool SkinMesh::GetDeviaPosFromBone( LPCSTR sBoneName, D3DXVECTOR3* pOutPos, D3DXVECTOR3 SpecifiedPos )
{
	if ( m_pD3dxMesh != nullptr )
	{
		D3DXMATRIX mtmp;
		if ( m_pD3dxMesh->GetMatrixFromBone( sBoneName, &mtmp )){
			D3DXMATRIX mWorld, mScale, mTran, mDevia;
			D3DXMATRIX mRot, mYaw, mPitch, mRoll;
			// おそらくボーンの初期の向きが正位置になっているはず
			// ずらしたい方向の行列を作成
			D3DXMatrixTranslation( &mDevia, SpecifiedPos.x, SpecifiedPos.y, SpecifiedPos.z );
			// ボーン位置行列とずらしたい方向行列を掛け合わせる
			D3DXMatrixMultiply( &mtmp, &mDevia, &mtmp );
			// 位置のみ取得
			D3DXVECTOR3 tmpPos = D3DXVECTOR3( mtmp._41, mtmp._42, mtmp._43 );

			D3DXMatrixScaling( &mScale, m_vScale.x, m_vScale.y, m_vScale.z );
			D3DXMatrixRotationY( &mYaw,		m_vRotation.y );
			D3DXMatrixRotationX( &mPitch,	m_vRotation.x );
			D3DXMatrixRotationZ( &mRoll,	m_vRotation.z );
			D3DXMatrixTranslation( &mTran, tmpPos.x, tmpPos.y, tmpPos.z );

			mRot = mYaw * mPitch * mRoll;
			mWorld = mTran * mScale * mRot;

			pOutPos->x = mWorld._41 + m_vPosition.x;
			pOutPos->y = mWorld._42 + m_vPosition.y;
			pOutPos->z = mWorld._43 + m_vPosition.z;

			return true;
		}
	}
	return false;
}


//-----------------------------------------------------------------------------
//		コンスタントバッファ作成関数
//-----------------------------------------------------------------------------
HRESULT SkinMesh::CreateCBuffer( ID3D11Buffer** pConstantBuffer, UINT Size)
{
	D3D11_BUFFER_DESC cb;
	cb.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth			= Size;
	cb.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags			= 0;
	cb.StructureByteStride	= 0;
	cb.Usage				= D3D11_USAGE_DYNAMIC;

	if (FAILED( m_pDevice11->CreateBuffer( &cb, nullptr, pConstantBuffer ))) { return E_FAIL; }
	return S_OK;
}


//-----------------------------------------------------------------------------
//		サンプラー作成関数
//-----------------------------------------------------------------------------
HRESULT SkinMesh::CreateSampler( ID3D11SamplerState** pSampler )
{
	// テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory( &SamDesc, sizeof( D3D11_SAMPLER_DESC ) );

	SamDesc.Filter	= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	if (FAILED( m_pDevice11->CreateSamplerState( &SamDesc, &m_pSampleLinear )))	{ return E_FAIL; }
	return S_OK;
}


//-----------------------------------------------------------------------------
//		マルチバイト文字をUnicode文字に変換する
//-----------------------------------------------------------------------------
void SkinMesh::ConvertCharaMultiByteToUnicode(
	WCHAR* Dest,			// (out)変換後の文字列(Unicode文字列)
	size_t DestArraySize,	// 変換後の文字列の配列の要素最大数
	const CHAR* str )		// (in)変換前の文字列(マルチバイト文字列)
{
	// テクスチャ名のサイズを取得
	size_t charSize = strlen( str ) + 1;
	size_t ret;	//変換された文字数

	// マルチバイト文字のシーケンスを対応するワイド文字のシーケンスに変換します
	errno_t err = mbstowcs_s( &ret, Dest, charSize, str, _TRUNCATE );
}