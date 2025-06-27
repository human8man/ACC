#include "StaticMesh.h"
#include "DirectX/DirectX9.h"
#include "DirectX/DirectX11.h"
#include "Camera/Camera.h"

#include <stdlib.h>
#include <locale.h>

// シェーダファイル名(ディレクトリも含む)
const TCHAR SHADER_NAME[] = _T( "Data\\Shader\\StaticMesh.hlsl" );


StaticMesh::StaticMesh()
	: m_pDx9				( nullptr )
	, m_pDevice9			( nullptr )

	, m_pDx11				( nullptr )
	, m_pDevice11			( nullptr )
	, m_pContext11			( nullptr )

	, m_pVertexShader		( nullptr )
	, m_pVertexLayout		( nullptr )
	, m_pPixelShader		( nullptr )

	, m_pCBufferPerMesh		( nullptr )
	, m_pCBufferPerMaterial	( nullptr )
	, m_pCBufferPerFrame	( nullptr )

	, m_pVertexBuffer		( nullptr )
	, m_ppIndexBuffer		( nullptr )
	, m_pSampleLinear		( nullptr )

	, m_Model				()
	, m_ModelForRay			()

	, m_pMaterials			( nullptr )
	, m_NumAttr				()
	, m_AttrID				()
	, m_EnableTexture		( false )
{
}
StaticMesh::~StaticMesh()
{
	// 解放処理
	Release();
}


//=============================================================================
//		初期化関数
//=============================================================================
HRESULT StaticMesh::Init(LPCTSTR lpFileName )
{
	m_pDx9 = DirectX9::GetInstance();
	m_pDevice9 = m_pDx9->GetDevice();

	m_pDx11 = DirectX11::GetInstance();
	m_pDevice11 = m_pDx11->GetDevice();
	m_pContext11 = m_pDx11->GetContext();

	//ファイル読み込み
	if( FAILED( LoadXMesh( lpFileName ) )){ return E_FAIL; }
	//モデル作成
	if( FAILED( CreateModel() ) ){ return E_FAIL; }
	//シェーダ作成
	if( FAILED(CreateShader() )){ return E_FAIL; }
	//コンスタントバッファ作成
	if( FAILED( CreateConstantBuffer() ) ){ return E_FAIL; }
	//サンプラ作成
	if( FAILED( CreateSampler() ) ){ return E_FAIL; }

	return S_OK;
}


//=============================================================================
//		解放関数
//=============================================================================
void StaticMesh::Release()
{
	SAFE_DELETE_ARRAY(m_ppIndexBuffer);
	SAFE_DELETE_ARRAY(m_pMaterials);

	SAFE_RELEASE( m_Model.pD3DXMtrlBuffer );
	SAFE_RELEASE( m_Model.pMesh );
	SAFE_RELEASE( m_ModelForRay.pD3DXMtrlBuffer );
	SAFE_RELEASE( m_ModelForRay.pMesh );

	SAFE_RELEASE( m_pSampleLinear );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pCBufferPerFrame );
	SAFE_RELEASE( m_pCBufferPerMaterial );
	SAFE_RELEASE( m_pCBufferPerMesh );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexShader );

	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
	m_pDx11 = nullptr;

	m_pDevice9 = nullptr;
	m_pDx9 = nullptr;
}

void StaticMesh::Update()
{
}


//=============================================================================
//		レンダリング用
//=============================================================================
void StaticMesh::Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light)
{
	// ワールド行列、スケール行列、回転行列、平行移動行列
	D3DXMATRIX mWorld, mScale, mRot, mTran;
	D3DXMATRIX mYaw, mPitch, mRoll;

	//拡大縮小行列作成
	D3DXMatrixScaling(&mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	D3DXMatrixRotationY(&mYaw, m_vRotation.y);	// Y軸回転行列作成
	D3DXMatrixRotationX(&mPitch, m_vRotation.x);// X軸回転行列作成
	D3DXMatrixRotationZ(&mRoll, m_vRotation.z);	// Z軸回転行列作成

	// 平行移動行列作成
	D3DXMatrixTranslation(&mTran, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// 回転行列を作成
	mRot = mYaw * mPitch * mRoll;

	// ワールド行列作成（必ず拡縮×回転×移動の順にする）
	mWorld = mScale * mRot * mTran;

	// 使用するシェーダのセット
	m_pContext11->VSSetShader(m_pVertexShader, nullptr, 0);	// 頂点シェーダ
	m_pContext11->PSSetShader(m_pPixelShader, nullptr, 0);	// ピクセルシェーダ

	// シェーダのコンスタントバッファに各種データを渡す
	D3D11_MAPPED_SUBRESOURCE pData;
	// バッファ内のデータの書き換え開始時にMap
	if (SUCCEEDED(m_pContext11->Map(
		m_pCBufferPerFrame, 0,
		D3D11_MAP_WRITE_DISCARD,
		0, &pData)))
	{
		// コンスタントバッファ(フレーム用)
		CBUFFER_PER_FRAME cb;

		D3DXVECTOR3 CamPos = Camera::GetInstance()->GetPos();

		// カメラ位置
		cb.CameraPos = D3DXVECTOR4(CamPos.x, CamPos.y, CamPos.z, 0.f);

		// ライト方向
		cb.vLightDir = D3DXVECTOR4(Light.vDirection.x, Light.vDirection.y, Light.vDirection.z, 0.f);

		// ライト方向の正規化(ノーマライズ）
		//	※モデルからライトへ向かう方向. ディレクショナルライトで重要な要素
		D3DXVec4Normalize(&cb.vLightDir, &cb.vLightDir);

		memcpy_s(
			pData.pData,	// コピー先のバッファ
			pData.RowPitch,	// コピー先のバッファサイズ
			(void*)(&cb),	// コピー元のバッファ
			sizeof(cb));	// コピー元のバッファサイズ

		// バッファ内のデータの書き換え終了時にUnmap
		m_pContext11->Unmap(m_pCBufferPerFrame, 0);
	}

	// このコンスタントバッファをどのシェーダで使用するか？
	m_pContext11->VSSetConstantBuffers(2, 1, &m_pCBufferPerFrame);	//頂点シェーダ
	m_pContext11->PSSetConstantBuffers(2, 1, &m_pCBufferPerFrame);	//ピクセルシェーダ


	// メッシュのレンダリング
	RenderMesh(mWorld, mView, mProj);
}


//============================================================================
//		インデックスバッファを取得
//============================================================================
std::vector<ID3D11Buffer*> StaticMesh::GetIndexBuffer() const
{
	std::vector<ID3D11Buffer*> indexBuffers;
	for (DWORD No = 0; No < m_NumAttr; No++)
	{
		indexBuffers.emplace_back(m_ppIndexBuffer[No]);
	}
	return indexBuffers;
}


//============================================================================
//		インデックスを取得
//============================================================================
std::vector<DWORD> StaticMesh::GetIndex() const
{
	std::vector<DWORD> index;

	// インデックスをstd::vector<DWORD>に追加
	for (DWORD No = 0; No < m_NumAttr; No++)
	{
		index.emplace_back(m_pMaterials[m_AttrID[No]].dwNumFace);
	}

	return index;
}


//============================================================================
//		レイとメッシュの当たり判定
//============================================================================
RayInfo StaticMesh::IsHitForRay(const RAY& pRay )
{
	FLOAT vDistance;
	D3DXVECTOR3 vAxis, StartPoint, EndPoint, vIntersect;
	D3DXMATRIX mTran, mRot, mScale, mWorld, mInverseWorld, mYaw, mPitch, mRoll;
	RayInfo info;

	// レイの方向ベクトルと位置を設定
	vAxis		= pRay.Axis;	 // レイの方向（軸ベクトル）
	StartPoint	= pRay.Position; // レイの開始位置
	EndPoint = StartPoint + (vAxis * pRay.Length); // レイの終点を計算

	// 移動処理
	D3DXMatrixTranslation( &mTran, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// 回転処理
	D3DXMatrixRotationY(&mYaw,	 m_vRotation.y);	// Y軸回転行列作成
	D3DXMatrixRotationX(&mPitch, m_vRotation.x);	// X軸回転行列作成
	D3DXMatrixRotationZ(&mRoll,	 m_vRotation.z);	// Z軸回転行列作成
	// 回転行列作成
	mRot = mYaw * mPitch * mRoll;

	// 拡縮処理
	D3DXMatrixScaling(&mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	// ワールド行列計算
	mWorld = mScale * mTran;

	// 逆行列を求める
	D3DXMatrixInverse( &mInverseWorld, nullptr, &mWorld );
	// レイの始点、終点に反映
	D3DXVec3TransformCoord( &StartPoint, &StartPoint, &mInverseWorld );
	D3DXVec3TransformCoord( &EndPoint, &EndPoint, &mInverseWorld );

	// 向きと長さ（大きさ）を求める
	D3DXVECTOR3 vDirection = EndPoint - StartPoint;

	BOOL bHit = FALSE;		// 命中フラグ
	DWORD dwIndex = 0;		// インデックス番号
	D3DXVECTOR3 Vertex[3];	// 頂点座標
	FLOAT U = 0, V = 0;		// 重心ヒット座標

	// メッシュとレイの交差を調べる
	D3DXIntersect(
		m_ModelForRay.pMesh,	// 対象メッシュ
		&StartPoint,			// レイの始点
		&vDirection,			// レイの向きと長さ（大きさ）
		&bHit,					// (out)判定結果
		&dwIndex,				// (out)bHitがTRUE時にレイの始点に最も近くの面のインデックス値へのポインタ
		&U, &V,					// (out)重心ヒット座標
		&vDistance,				// (out)メッシュとの距離
		nullptr, nullptr);

	// 無限に伸びるレイのどこかでメッシュが当たっていたら
	if (bHit)
	{
		// 命中した時
		FindVerticesOnPoly( m_ModelForRay.pMesh, dwIndex, Vertex );

		// 重心座標から交点を算出
		//	ローカル交点は v0 + U*(v1-v0) + V*(v2-v0) で求まる
		vIntersect = Vertex[0] + U * (Vertex[1] - Vertex[0]) + V * (Vertex[2] - Vertex[0]);

		// モデルデータが「拡縮」「回転」「移動」していれば行列が必要
		D3DXVec3TransformCoord( &vIntersect, &vIntersect, &mWorld );

		// Hit状態と交差点を返す
		return { true, vIntersect ,vDistance }; 
	}

	return { false, ZEROVEC3, 9999.f };
}


//-----------------------------------------------------------------------------
//		モデル作成
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateModel()
{
	//マテリアル作成
	if( FAILED( CreateMaterials() ) ) { return E_FAIL; }
	//インデックスバッファ作成
	if( FAILED( CreateIndexBuffer() ) ) { return E_FAIL; }
	//頂点バッファ作成
	if( FAILED( CreateVertexBuffer() ) ) { return E_FAIL; }

	return S_OK;
}


//-----------------------------------------------------------------------------
//		HLSLファイルを読み込みシェーダを作成する
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
if(ISDEBUG){
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
}

	// HLSLからバーテックスシェーダのブロブを作成
	if( m_EnableTexture == true ){
		if (FAILED(
			D3DX11CompileFromFile(
				SHADER_NAME, nullptr, nullptr, "VS_Main", "vs_5_0",
				uCompileFlag, 0, nullptr, &pCompiledShader, &pErrors, nullptr )))
		{
			_ASSERT_EXPR( false, _T( "hlsl読み込み失敗" ) );
			return E_FAIL;
		}
	}
	else {
		if( FAILED(
			D3DX11CompileFromFile(
				SHADER_NAME, nullptr, nullptr, "VS_NoTex", "vs_5_0",
				uCompileFlag, 0, nullptr, &pCompiledShader, &pErrors, nullptr )))
		{
			_ASSERT_EXPR( false, _T( "hlsl読み込み失敗" ) );
			return E_FAIL;
		}
	}
	SAFE_RELEASE( pErrors );

	// 上記で作成したブロブから「バーテックスシェーダ」を作成
	if( FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pVertexShader )))	//(out)バーテックスシェーダ
	{
		_ASSERT_EXPR( false, _T( "バーテックスシェーダ作成失敗" ) );
		return E_FAIL;
	}

	// 頂点インプットレイアウトを定義
	D3D11_INPUT_ELEMENT_DESC layout[3];
	// 頂点インプットレイアウトの配列要素数を算出
	UINT numElements = 0;
	if( m_EnableTexture == true )
	{
		D3D11_INPUT_ELEMENT_DESC tmp[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		numElements = sizeof( tmp ) / sizeof( tmp[0] );	// 要素数算出
		memcpy_s( layout, sizeof( layout ),
			tmp, sizeof( D3D11_INPUT_ELEMENT_DESC ) * numElements );
	}
	else
	{
		D3D11_INPUT_ELEMENT_DESC tmp[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		numElements = sizeof( tmp ) / sizeof( tmp[0] );	// 要素数算出
		memcpy_s( layout, sizeof( layout ),
			tmp, sizeof( D3D11_INPUT_ELEMENT_DESC ) * numElements );
	}

	// 頂点インプットレイアウトを作成
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout)))	// (out)頂点インプットレイアウト
	{
		_ASSERT_EXPR(false, _T( "頂点インプットレイアウト作成失敗" ));
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	// HLSLからピクセルシェーダのブロブを作成
	if( m_EnableTexture == true ){
		if( FAILED(
			D3DX11CompileFromFile(
				SHADER_NAME, nullptr, nullptr, "PS_Main", "ps_5_0",
				uCompileFlag, 0, nullptr, &pCompiledShader, &pErrors, nullptr )))
		{
			_ASSERT_EXPR(false, _T( "hlsl読み込み失敗" ));
			return E_FAIL;
		}
	}
	else {
		if( FAILED(
			D3DX11CompileFromFile(
				SHADER_NAME, nullptr, nullptr, "PS_NoTex", "ps_5_0",
				uCompileFlag, 0, nullptr, &pCompiledShader, &pErrors, nullptr )))
		{
			_ASSERT_EXPR(false, _T( "hlsl読み込み失敗" ));
			return E_FAIL;
		}
	}
	SAFE_RELEASE( pErrors );

	// 上記で作成したブロブから「ピクセルシェーダ」を作成
	if( FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pPixelShader )))	//(out)ピクセルシェーダ
	{
		_ASSERT_EXPR( false, _T( "ピクセルシェーダ作成失敗" ) );
		return E_FAIL;
	}
	SAFE_RELEASE( pCompiledShader );

	return S_OK;
}


//-----------------------------------------------------------------------------
//		サンプラ作成
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateSampler()
{
	// テクスチャ用のサンプラ構造体
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof( samDesc ) );
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;// リニアフィルタ(線形補間)
						// POINT:高速だが粗い
	samDesc.AddressU
		= D3D11_TEXTURE_ADDRESS_WRAP;// ラッピングモード(WRAP:繰り返し)
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	// MIRROR: 反転繰り返し
	// CLAMP : 端の模様を引き伸ばす
	// BORDER: 別途境界色を決める

	// サンプラ作成
	if( FAILED( m_pDevice11->CreateSamplerState( &samDesc, &m_pSampleLinear ) ) )
	{
		_ASSERT_EXPR( false, _T( "サンプラ作成失敗" ) );
		return E_FAIL;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//		マテリアル作成
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateMaterials()
{
	// ファイルのパス(ディレクトリ)を確保
	const int PATH_MAX = 64;
	TCHAR path[PATH_MAX] = _T( "" );
	int path_count = lstrlen( m_Model.FileName );
	for( int k = path_count; k >= 0; k-- ) {
		if( m_Model.FileName[k] == '\\' ) {
			for( int j = 0; j <= k; j++ ) {
				path[j] = m_Model.FileName[j];
			}
			path[k + 1] = '\0';
			break;
		}
	}

	// 読み込んだ情報から必要な情報を抜き出す
	D3DXMATERIAL* d3dxMaterials = static_cast<D3DXMATERIAL*>( m_Model.pD3DXMtrlBuffer->GetBufferPointer() );
	// マテリアル数分の領域を確保
	m_pMaterials = new MY_MATERIAL[m_Model.NumMaterials]();
	// マテリアル数分繰り返し
	for( DWORD No = 0; No < m_Model.NumMaterials; No++ )
	{
		// マテリアル情報のコピー
		// ディフューズ
		m_pMaterials[No].Diffuse.x = d3dxMaterials[No].MatD3D.Diffuse.r;
		m_pMaterials[No].Diffuse.y = d3dxMaterials[No].MatD3D.Diffuse.g;
		m_pMaterials[No].Diffuse.z = d3dxMaterials[No].MatD3D.Diffuse.b;
		m_pMaterials[No].Diffuse.w = d3dxMaterials[No].MatD3D.Diffuse.a;
		// アンビエント
		m_pMaterials[No].Ambient.x = d3dxMaterials[No].MatD3D.Ambient.r;
		m_pMaterials[No].Ambient.y = d3dxMaterials[No].MatD3D.Ambient.g;
		m_pMaterials[No].Ambient.z = d3dxMaterials[No].MatD3D.Ambient.b;
		m_pMaterials[No].Ambient.w = d3dxMaterials[No].MatD3D.Ambient.a;		
		// スペキュラ
		m_pMaterials[No].Specular.x = d3dxMaterials[No].MatD3D.Specular.r;
		m_pMaterials[No].Specular.y = d3dxMaterials[No].MatD3D.Specular.g;
		m_pMaterials[No].Specular.z = d3dxMaterials[No].MatD3D.Specular.b;
		m_pMaterials[No].Specular.w = d3dxMaterials[No].MatD3D.Specular.a;
		// スペキュラパワー
		m_pMaterials[No].Power = d3dxMaterials[No].MatD3D.Power;
		// エミッシブ
		m_pMaterials[No].Emissive.x = d3dxMaterials[No].MatD3D.Emissive.r;
		m_pMaterials[No].Emissive.y = d3dxMaterials[No].MatD3D.Emissive.g;
		m_pMaterials[No].Emissive.z = d3dxMaterials[No].MatD3D.Emissive.b;
		m_pMaterials[No].Emissive.w = d3dxMaterials[No].MatD3D.Emissive.a;

		// (その面に)テクスチャが貼られているか？
		if( d3dxMaterials[No].pTextureFilename != nullptr
			&& strlen( d3dxMaterials[No].pTextureFilename ) > 0 )
		{
#ifdef UNICODE
			WCHAR TexFilename_w[32] = L"";
			// テクスチャ名のサイズを取得
			size_t charSize = strlen( d3dxMaterials[No].pTextureFilename ) + 1;
			size_t ret;	// 変換された文字数

			// マルチバイト文字のシーケンスを対応するワイド文字のシーケンスに変換します
			errno_t err = mbstowcs_s(
				&ret,
				TexFilename_w,
				charSize,
				d3dxMaterials[No].pTextureFilename,
				_TRUNCATE );

			LPTSTR filename = TexFilename_w;
#else// #ifdef UNICODE
			LPTSTR filename = d3dxMaterials[No].pTextureFilename;
#endif// #ifdef UNICODE

			// テクスチャありのフラグを立てる
			m_EnableTexture = true;

			// パスをコピー
			lstrcpy( m_pMaterials[No].TextureName, path );
			// テクスチャファイル名を連結
			lstrcat( m_pMaterials[No].TextureName, filename );

			// テクスチャ作成
			if( FAILED( D3DX11CreateShaderResourceViewFromFile(
				m_pDevice11, m_pMaterials[No].TextureName,// テクスチャファイル名
				nullptr, nullptr,
				&m_pMaterials[No].pTexture, // (out)テクスチャオブジェクト
				nullptr ) ) )
			{
				_ASSERT_EXPR( false, _T( "テクスチャ作成失敗" ) );
				return E_FAIL;
			}
		}
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//		インデックスバッファ作成
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateIndexBuffer()
{
	D3D11_BUFFER_DESC	bd = {}; // Dx11バッファ構造体
	D3D11_SUBRESOURCE_DATA	InitData = {}; // 初期化データ

	// マテリアル数分の領域を確保
	m_ppIndexBuffer = new ID3D11Buffer*[m_Model.NumMaterials]();
	for( DWORD No = 0; No < m_Model.NumMaterials; No++ ){
		m_ppIndexBuffer[No] = nullptr;
	}

	// メッシュの属性情報を得る
	// 属性情報でインデックスバッファから細かいマテリアルごとのインデックスバッファを分離できる
	D3DXATTRIBUTERANGE* pAttrTable = nullptr;

	// メッシュの面および頂点の順番変更を制御し、パフォーマンスを最適化する
	// D3DXMESHOPT_COMPACT : 面の順番を変更し、使用されていない頂点と面を削除する
	// D3DXMESHOPT_ATTRSORT : パフォーマンスを上げる為、面の順番を変更して最適化を行う
	m_Model.pMesh->OptimizeInplace( D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT,
		nullptr, nullptr, nullptr, nullptr );
	// 属性テーブルの取得
	m_Model.pMesh->GetAttributeTable( nullptr, &m_NumAttr );
	pAttrTable = new D3DXATTRIBUTERANGE[m_NumAttr];
	if( FAILED( m_Model.pMesh->GetAttributeTable( pAttrTable, &m_NumAttr ) ) )
	{
		_ASSERT_EXPR( false, _T( "属性テーブル取得失敗" ) );
		return E_FAIL;
	}

	// 同じくLockしないと取り出せない
	int* pIndex = nullptr;
	m_Model.pMesh->LockIndexBuffer( D3DLOCK_READONLY, (void**)&pIndex );
	// 属性ごとのインデックスバッファを作成
	for( DWORD No = 0; No < m_NumAttr; No++ )
	{
		m_AttrID[No] = pAttrTable[No].AttribId;
		// Dx9のインデックスバッファからの情報で、Dx11のインデックスバッファを作成
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( int )*pAttrTable[No].FaceCount * 3;// 面数×3で頂点数
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		// 大きいインデックスバッファ内のオフセット(×3する)
		InitData.pSysMem = &pIndex[pAttrTable[No].FaceStart * 3];

		if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, &m_ppIndexBuffer[No] ) ) )
		{
			_ASSERT_EXPR( false, _T( "インデックスバッファ作成失敗" ) );
			return E_FAIL;
		}
		// 面の数をコピー
		m_pMaterials[m_AttrID[No]].dwNumFace = pAttrTable[No].FaceCount;
	}
	// 属性テーブルの削除
	delete[] pAttrTable;
	// 使用済みのインデックスバッファの解放
	m_Model.pMesh->UnlockIndexBuffer();

	return S_OK;
}


//-----------------------------------------------------------------------------
//		頂点バッファ作成
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateVertexBuffer()
{
	D3D11_BUFFER_DESC	bd = {}; // Dx11バッファ構造体
	D3D11_SUBRESOURCE_DATA	InitData = {}; // 初期化データ

	// Dx9の場合、mapではなくLockで頂点バッファからデータを取り出す
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	m_Model.pMesh->GetVertexBuffer( &pVB );
	DWORD dwStride = m_Model.pMesh->GetNumBytesPerVertex();
	BYTE* pVertices = nullptr;
	VERTEX* pVertex = nullptr;
	if( SUCCEEDED( pVB->Lock( 0, 0, (VOID**)&pVertices, 0 ) ) )
	{
		pVertex = (VERTEX*)pVertices;
		// Dx9の頂点バッファからの情報で、Dx11頂点バッファを作成
		bd.Usage = D3D11_USAGE_DEFAULT;
		// 頂点を格納するのに必要なバイト数
		bd.ByteWidth = m_Model.pMesh->GetNumBytesPerVertex()*m_Model.pMesh->GetNumVertices();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		InitData.pSysMem = pVertex;
		if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, &m_pVertexBuffer ) ) )
		{
			_ASSERT_EXPR( false, _T( "頂点バッファ作成失敗" ) );
			return E_FAIL;
		}
		pVB->Unlock();
	}
	SAFE_RELEASE( pVB );	// 頂点バッファ解放
	return S_OK;
}


//-----------------------------------------------------------------------------
//		コンスタントバッファ作成
//-----------------------------------------------------------------------------
HRESULT StaticMesh::CreateConstantBuffer()
{
	// コンスタントバッファ(メッシュ用)
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// コンスタントバッファを指定
	cb.ByteWidth = sizeof( CBUFFER_PER_MESH );	// コンスタントバッファのサイズ
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// 書き込みでアクセス
	cb.MiscFlags = 0;							// その他のフラグ(未使用)
	cb.StructureByteStride = 0;					// 構造体のサイズ(未使用)
	cb.Usage = D3D11_USAGE_DYNAMIC;				// 使用方法:直接書き込み

	// コンスタントバッファの作成
	if( FAILED( m_pDevice11->CreateBuffer( &cb, nullptr, &m_pCBufferPerMesh ) ) )
	{
		_ASSERT_EXPR( false, _T( "コンスタントバッファ(メッシュ)作成失敗" ) );
		return E_FAIL;
	}

	// コンスタントバッファ(マテリアル用)
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		// コンスタントバッファを指定
	cb.ByteWidth = sizeof( CBUFFER_PER_MATERIAL );	// コンスタントバッファのサイズ
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// 書き込みでアクセス
	cb.MiscFlags = 0;								// その他のフラグ(未使用)
	cb.StructureByteStride = 0;						// 構造体のサイズ(未使用)
	cb.Usage = D3D11_USAGE_DYNAMIC;					// 使用方法:直接書き込み

	// コンスタントバッファの作成
	if( FAILED( m_pDevice11->CreateBuffer( &cb, nullptr, &m_pCBufferPerMaterial ) ) )
	{
		_ASSERT_EXPR( false, _T( "コンスタントバッファ(マテリアル用)作成失敗" ) );
		return E_FAIL;
	}

	// コンスタントバッファ(フレーム用)
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// コンスタントバッファを指定
	cb.ByteWidth = sizeof( CBUFFER_PER_FRAME );	// コンスタントバッファのサイズ
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// 書き込みでアクセス
	cb.MiscFlags = 0;							// その他のフラグ(未使用)
	cb.StructureByteStride = 0;					// 構造体のサイズ(未使用)
	cb.Usage = D3D11_USAGE_DYNAMIC;				// 使用方法:直接書き込み

	// コンスタントバッファの作成
	if( FAILED( m_pDevice11->CreateBuffer( &cb, nullptr, &m_pCBufferPerFrame ) ) )
	{
		_ASSERT_EXPR( false, _T( "コンスタントバッファ(フレーム用)作成失敗" ) );
		return E_FAIL;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//		メッシュ読み込み
//-----------------------------------------------------------------------------
HRESULT StaticMesh::LoadXMesh( LPCTSTR lpFileName )
{
	//ファイル名をコピー
	lstrcpy( m_Model.FileName, lpFileName );
	lstrcpy( m_ModelForRay.FileName, lpFileName );

	// マテリアルバッファ
	LPD3DXBUFFER pD3DXMtrlBuffer = nullptr;

	//Xファイルのロード
	if( FAILED( D3DXLoadMeshFromX(
		lpFileName,					// ファイル名
		D3DXMESH_SYSTEMMEM			// システムメモリに読み込み
		| D3DXMESH_32BIT,			// 32bit
		m_pDevice9, nullptr,
		&m_Model.pD3DXMtrlBuffer,	// (out)マテリアル情報
		nullptr,
		&m_Model.NumMaterials,		// (out)マテリアル数
		&m_Model.pMesh ) ) )		// (out)メッシュオブジェクト
	{
		_ASSERT_EXPR( false, _T( "Xファイル読込失敗" ) );
		return E_FAIL;
	}

	// 頂点情報の保存
	SaveVertices(m_Model.pMesh);

	// Xファイルのロード(レイとの判定用に別設定で読み込む)
	if( FAILED( D3DXLoadMeshFromX(
		lpFileName,						// ファイル名
		D3DXMESH_SYSTEMMEM,				// システムメモリに読み込み
		m_pDevice9, nullptr,
		&m_ModelForRay.pD3DXMtrlBuffer,	// (out)マテリアル情報
		nullptr,
		&m_ModelForRay.NumMaterials,	// (out)マテリアル数
		&m_ModelForRay.pMesh ) ) )		// (out)メッシュオブジェクト
	{
		_ASSERT_EXPR( false, _T( "Xファイル読込失敗" ) );
		return E_FAIL;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//		レンダリング関数(クラス内でのみ使用する)
//-----------------------------------------------------------------------------
void StaticMesh::RenderMesh( D3DXMATRIX& mWorld, D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	// シェーダのコンスタントバッファに各種データを渡す
	D3D11_MAPPED_SUBRESOURCE pData;
	// バッファ内のデータの書き換え開始時にMap
	if( SUCCEEDED( m_pContext11->Map(
		m_pCBufferPerMesh, 0,
		D3D11_MAP_WRITE_DISCARD,
		0, &pData )))
	{
		// コンスタントバッファ(メッシュ用)
		CBUFFER_PER_MESH cb;

		// ワールド行列を渡す
		cb.mW = mWorld;
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// ワールド,ビュー,プロジェクション行列を渡す
		D3DXMATRIX mWVP = mWorld * mView * mProj;
		D3DXMatrixTranspose( &mWVP, &mWVP );	// 行列を転置する
		// ※行列の計算方法がDirectXとGPUで異なるため転置が必要
		cb.mWVP = mWVP;

		memcpy_s(
			pData.pData,	// コピー先のバッファ
			pData.RowPitch,	// コピー先のバッファサイズ
			(void*)(&cb),	// コピー元のバッファ
			sizeof(cb));	// コピー元のバッファサイズ

		// バッファ内のデータの書き換え終了時にUnmap
		m_pContext11->Unmap( m_pCBufferPerMesh, 0 );
	}

	// このコンスタントバッファをどのシェーダで使用するか？
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pCBufferPerMesh ); // 頂点シェーダ
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pCBufferPerMesh ); // ピクセルシェーダ

	// 頂点インプットレイアウトをセット
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// プリミティブ・トポロジーをセット
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// 頂点バッファをセット
	UINT stride = m_Model.pMesh->GetNumBytesPerVertex();
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

	// 属性の数だけ、それぞれの属性のインデックスバッファを描画
	for( DWORD No = 0; No < m_NumAttr; No++ )
	{
		// 使用されていないマテリアル対策
		if( m_pMaterials[m_AttrID[No]].dwNumFace == 0 ) { continue; }
		
		// インデックスバッファをセット
		m_pContext11->IASetIndexBuffer( m_ppIndexBuffer[No], DXGI_FORMAT_R32_UINT, 0 );

		// マテリアルの各要素をシェーダに渡す
		D3D11_MAPPED_SUBRESOURCE pDataMat;
		if( SUCCEEDED(
			m_pContext11->Map(m_pCBufferPerMaterial,
				0, D3D11_MAP_WRITE_DISCARD, 0, &pDataMat )))
		{
			// コンスタントバッファ(マテリアル用)
			CBUFFER_PER_MATERIAL cb;
			// ディフューズ,アンビエント,スペキュラをシェーダに渡す
			cb.Diffuse	= m_pMaterials[m_AttrID[No]].Diffuse;
			cb.Ambient	= m_pMaterials[m_AttrID[No]].Ambient;
			cb.Specular = m_pMaterials[m_AttrID[No]].Specular;

			memcpy_s(pDataMat.pData, pDataMat.RowPitch,
				(void*)&cb, sizeof(cb));

			m_pContext11->Unmap( m_pCBufferPerMaterial, 0 );
		}

		// このコンスタントバッファをどのシェーダで使うか？
		m_pContext11->VSSetConstantBuffers( 1, 1, &m_pCBufferPerMaterial );
		m_pContext11->PSSetConstantBuffers( 1, 1, &m_pCBufferPerMaterial );

		// テクスチャをシェーダに渡す
		if (m_pMaterials[m_AttrID[No]].pTexture != nullptr) {
			// テクスチャがあるとき
			m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinear );
			m_pContext11->PSSetShaderResources(
				0, 1, &m_pMaterials[m_AttrID[No]].pTexture );
		}
		else {
			// テクスチャがないとき
			ID3D11ShaderResourceView* pNothing[1] = { 0 };
			m_pContext11->PSSetShaderResources( 0, 1, pNothing );
		}

		// プリミティブ(ポリゴン)をレンダリング
		m_pContext11->DrawIndexed( m_pMaterials[m_AttrID[No]].dwNumFace * 3, 0, 0 );
	}
}


//-----------------------------------------------------------------------------
//		各頂点情報の保存
//-----------------------------------------------------------------------------
void StaticMesh::SaveVertices(LPD3DXMESH pMesh)
{
	if (!pMesh) return;

	// 頂点バッファをロックしてデータへのアクセスを確保
	void* pVertices = nullptr;
	if (FAILED(pMesh->LockVertexBuffer(D3DLOCK_READONLY, &pVertices))) { return; }

	// 頂点数と頂点1つあたりのバイトサイズを取得
	DWORD numVertices = pMesh->GetNumVertices();		// 総頂点数
	DWORD vertexSize = pMesh->GetNumBytesPerVertex();	// 各頂点のバイト数

	// 保存先の頂点リストをクリア（再利用時の安全策）
	m_Vertices.clear();

	// 頂点バッファから全頂点を取得
	for (DWORD i = 0; i < numVertices; ++i)
	{
		// 現在の頂点の位置を計算
		BYTE* pVertex = static_cast<BYTE*>(pVertices) + i * vertexSize;

		// 頂点の位置を取得
		D3DXVECTOR3 position = *(D3DXVECTOR3*)pVertex;
		m_Vertices.push_back(position);
	}

	// 頂点バッファのロックを解除
	pMesh->UnlockVertexBuffer();
}


//----------------------------------------------------------------------------
//		交差位置のポリゴンの頂点を見つける
//			※頂点座標はローカル座標
//----------------------------------------------------------------------------
HRESULT StaticMesh::FindVerticesOnPoly( LPD3DXMESH pMesh, DWORD dwPolyIndex, D3DXVECTOR3* pVertices)
{
	DWORD dwStride	  = pMesh->GetNumBytesPerVertex();// 頂点ごとのバイト数を取得
	DWORD dwVertexAmt = pMesh->GetNumVertices();	  // 頂点数を取得
	DWORD dwPoly	  = pMesh->GetNumFaces();		  // 面数を取得
	WORD* pwPoly	  = nullptr;

	// インデックスバッファをロック（読み込みモード）
	pMesh->LockIndexBuffer( D3DLOCK_READONLY, reinterpret_cast<VOID**>(&pwPoly));

	BYTE* pbVertices = nullptr;				// 頂点(バイト型)
	FLOAT* pfVertices = nullptr;			// 頂点(float型)
	LPDIRECT3DVERTEXBUFFER9 VB = nullptr;	// 頂点バッファ

	// 頂点情報の取得
	pMesh->GetVertexBuffer(&VB);

	// 頂点バッファのロック
	if (SUCCEEDED( VB->Lock(0, 0, reinterpret_cast<VOID**>(&pbVertices), 0 ) ))
	{
		// ポリゴンの頂点１つ目を取得
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3]]);
		pVertices[0].x = pfVertices[0];
		pVertices[0].y = pfVertices[1];
		pVertices[0].z = pfVertices[2];

		// ポリゴンの頂点２つ目を取得
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 1]]);
		pVertices[1].x = pfVertices[0];
		pVertices[1].y = pfVertices[1];
		pVertices[1].z = pfVertices[2];

		// ポリゴンの頂点３つ目を取得
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 2]]);
		pVertices[2].x = pfVertices[0];
		pVertices[2].y = pfVertices[1];
		pVertices[2].z = pfVertices[2];

		pMesh->UnlockIndexBuffer();	// ロック解除
		VB->Unlock();	// ロック解除
	}
	VB->Release();	// 不要になったので解放

	return S_OK;
}