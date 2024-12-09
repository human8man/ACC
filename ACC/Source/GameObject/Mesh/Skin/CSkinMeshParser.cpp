/***************************************************************************************************
*	SkinMeshCode Version 2.40
*	LastUpdate	: 2024/06/14.
**/
#include "CSkinMeshParser.h"
#include <stdlib.h>
#include <string.h>
#include <crtdbg.h>

#include <memory>


//フレームを作成する.
HRESULT MY_HIERARCHY::CreateFrame( LPCSTR Name, LPD3DXFRAME *ppNewFrame )
{
	HRESULT hr = S_OK;
	MYFRAME *pFrame;

	*ppNewFrame = nullptr;

	pFrame = new MYFRAME();
	if ( pFrame == nullptr )
	{
		return E_OUTOFMEMORY;
	}
	int length = static_cast<int>( strlen( Name ) ) + 1;
	pFrame->Name = new CHAR[length]();
	if ( !pFrame->Name )
	{
		return E_FAIL;
	}
	strcpy_s( pFrame->Name, sizeof( CHAR )*length, Name );

	D3DXMatrixIdentity( &pFrame->TransformationMatrix );
	D3DXMatrixIdentity( &pFrame->CombinedTransformationMatrix );

	*ppNewFrame = pFrame;

	return S_OK;
}

//メッシュコンテナーを作成する.
HRESULT MY_HIERARCHY::CreateMeshContainer(
	LPCSTR Name, CONST D3DXMESHDATA* pMeshData,
	CONST D3DXMATERIAL* pMaterials, CONST D3DXEFFECTINSTANCE* pEffectInstances,
	DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo,
	LPD3DXMESHCONTAINER *ppMeshContainer )
{
	*ppMeshContainer = nullptr;
	MYMESHCONTAINER *pMeshContainer = new MYMESHCONTAINER();

	int length = static_cast<int>( strlen( Name ) ) + 1;
	pMeshContainer->Name = new CHAR[length]();
	strcpy_s( pMeshContainer->Name, sizeof( CHAR ) * length, Name );

	memcpy( &pMeshContainer->MeshData, pMeshData, sizeof( pMeshContainer->MeshData ) );
	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	int NumPoly = pMeshContainer->MeshData.pMesh->GetNumFaces();
	//メッシュを複製する.
	//	なぜかスキンメッシュだと、この関数を抜けた直後にD3DX内部でメッシュポインターがおかしくなってしまうので.
	LPDIRECT3DDEVICE9 pDevice = nullptr;
	pMeshContainer->MeshData.pMesh->GetDevice( &pDevice );

	LPD3DXMESH pMesh = nullptr;
	pMeshContainer->MeshData.pMesh->CloneMesh( 0, nullptr, pDevice, &pMesh );
	//SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
	pMeshContainer->MeshData.pMesh = pMesh;

	//メッシュのマテリアル設定.
	pMeshContainer->NumMaterials = max( 1, NumMaterials );
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials]();
	pMeshContainer->pAdjacency = new DWORD[pMeshContainer->MeshData.pMesh->GetNumFaces() * 3]();
	if ( ( pMeshContainer->pAdjacency == nullptr ) || ( pMeshContainer->pMaterials == nullptr ) )
	{
		return E_FAIL;
	}
	memcpy( pMeshContainer->pAdjacency, pAdjacency, sizeof( DWORD ) * NumPoly * 3 );

	if ( NumMaterials > 0 )
	{
		for( DWORD i = 0; i < NumMaterials; i++ )
		{
			memcpy( &pMeshContainer->pMaterials[i], &pMaterials[i], sizeof( D3DXMATERIAL ) );
			if ( pMaterials[i].pTextureFilename != nullptr )
			{
				int length = 512;
				pMeshContainer->pMaterials[i].pTextureFilename = new CHAR[length]();
				strcpy_s( pMeshContainer->pMaterials[i].pTextureFilename,
					sizeof( CHAR ) * length, pMaterials[i].pTextureFilename );
			}
		}
	}
	//マテリアルがなかった場合.
	else
	{
		pMeshContainer->pMaterials[0].pTextureFilename = nullptr;
		memset( &pMeshContainer->pMaterials[0].MatD3D, 0, sizeof( D3DMATERIAL9 ) );
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}
	//当該メッシュがスキン情報を持っている場合(スキンメッシュ固有の処理).
	if ( pSkinInfo != nullptr )
	{
		DWORD BoneNum = 0;

		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();
		BoneNum = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[BoneNum]();

		for( DWORD i = 0; i < BoneNum; i++ )
		{
			memcpy( &pMeshContainer->pBoneOffsetMatrices[i],
				pMeshContainer->pSkinInfo->GetBoneOffsetMatrix( i ), sizeof( D3DMATRIX ) );
		}
	}
	//ローカルに生成したメッシュコンテナーを呼び出し側にコピーする.
	*ppMeshContainer = pMeshContainer;

	return S_OK;
}

//フレームを破棄する.
HRESULT MY_HIERARCHY::DestroyFrame( LPD3DXFRAME pFrameToFree )
{
	if ( pFrameToFree->Name ) {
		delete[] pFrameToFree->Name;
		pFrameToFree->Name = nullptr;
	}
	if ( pFrameToFree->pMeshContainer ) {
		DestroyMeshContainer( pFrameToFree->pMeshContainer );
	}
	if ( pFrameToFree->pFrameFirstChild ) {
		DestroyFrame( pFrameToFree->pFrameFirstChild );
	}
	if ( pFrameToFree->pFrameSibling ) {
		DestroyFrame( pFrameToFree->pFrameSibling );
	}
	delete pFrameToFree;

	return S_OK;
}

//メッシュコンテナーを破棄する.
HRESULT MY_HIERARCHY::DestroyMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase )
{
	DWORD dwMaterial;
	MYMESHCONTAINER *pMeshContainer = reinterpret_cast<MYMESHCONTAINER*>( pMeshContainerBase );

	SAFE_DELETE_ARRAY( pMeshContainer->Name );
	SAFE_RELEASE( pMeshContainer->pSkinInfo );
	SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
	SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );

	if( pMeshContainer->ppBoneMatrix != nullptr ) 
	{
		for( DWORD i = 0; i < pMeshContainer->BoneNum; i++ ) {
			pMeshContainer->ppBoneMatrix[i] = nullptr;
		}
		SAFE_DELETE_ARRAY( pMeshContainer->ppBoneMatrix );
	}

	if( pMeshContainer->ppTextures != nullptr )
	{
		for( dwMaterial = 0; dwMaterial < pMeshContainer->NumMaterials; dwMaterial++ )
		{
			SAFE_RELEASE( pMeshContainer->ppTextures[dwMaterial] );
		}
	}
	SAFE_DELETE_ARRAY( pMeshContainer->ppTextures );

	SAFE_RELEASE( pMeshContainer->MeshData.pMesh );

	if ( pMeshContainer->pBoneBuffer != nullptr )
	{
		SAFE_RELEASE( pMeshContainer->pBoneBuffer );
		SAFE_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );
	}

	SAFE_DELETE( pMeshContainer );

	return S_OK;
}


/*******************************************************************************************************************************************
*
*	以降、パーサークラス.
*
**/
//コンストラクタ.
D3DXPARSER::D3DXPARSER()
	: cHierarchy		()
	, m_pHierarchy		( nullptr )
	, m_pFrameRoot		( nullptr )
	, m_pAnimController	( nullptr )
	, m_pAnimSet		()
{
#if _DEBUG
	_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );
#endif
}

//デストラクタ.
D3DXPARSER::~D3DXPARSER()
{
}

//ボーン行列の領域確保.
HRESULT D3DXPARSER::AllocateBoneMatrix( LPD3DXMESHCONTAINER pMeshContainerBase )
{
	MYFRAME *pFrame = nullptr;
	DWORD dwBoneNum = 0;

	MYMESHCONTAINER *pMeshContainer = reinterpret_cast<MYMESHCONTAINER*>( pMeshContainerBase );
	if ( pMeshContainer->pSkinInfo == nullptr )
	{
		return S_OK;
	}
	dwBoneNum = pMeshContainer->pSkinInfo->GetNumBones();
	pMeshContainer->ppBoneMatrix = new D3DXMATRIX*[dwBoneNum]();

	for( DWORD i = 0; i < dwBoneNum; i++ )
	{
		//まずはnullptrで初期化.
		pMeshContainer->ppBoneMatrix[i] = nullptr;

		pFrame = reinterpret_cast<MYFRAME*>(
			D3DXFrameFind( m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName( i ) ) );
		if ( pFrame == nullptr )
		{
			return E_FAIL;
		}
		pMeshContainer->ppBoneMatrix[i] = &pFrame->CombinedTransformationMatrix;

	}
	return S_OK;
}

//全てのボーン行列の領域を確保(再帰関数).
HRESULT D3DXPARSER::AllocateAllBoneMatrices( LPD3DXFRAME pFrame )
{
	if ( pFrame->pMeshContainer != nullptr )
	{
		if ( FAILED( AllocateBoneMatrix( pFrame->pMeshContainer )))
		{
			return E_FAIL;
		}
	}
	if ( pFrame->pFrameSibling != nullptr )
	{
		if ( FAILED( AllocateAllBoneMatrices( pFrame->pFrameSibling )))
		{
			return E_FAIL;
		}
	}
	if ( pFrame->pFrameFirstChild != nullptr )
	{
		if ( FAILED( AllocateAllBoneMatrices( pFrame->pFrameFirstChild )))
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

//Xファイルからメッシュを読み込む.
HRESULT D3DXPARSER::LoadMeshFromX( LPDIRECT3DDEVICE9 pDevice9, LPCTSTR fileName )
{
	//Xファイルからアニメーションメッシュを読み込み作成する
	m_pHierarchy = new MY_HIERARCHY();
	if ( FAILED(
		D3DXLoadMeshHierarchyFromX(
			fileName, D3DXMESH_MANAGED, pDevice9, m_pHierarchy,
			nullptr, &m_pFrameRoot, &m_pAnimController )))
	{
		_ASSERT_EXPR( false, L"ファイルの読み込みに失敗しました" );
		return E_FAIL;
	}
	//ボーンメモリ割りあて.
	AllocateAllBoneMatrices( m_pFrameRoot );

	/*
		m_pContainer = (MYMESHCONTAINER*)GetMeshContainer( m_pFrameRoot );

		if( m_pContainer == nullptr ){
			_ASSERT_EXPR( false, L"メッシュコンテナが見つからなかった" );
			return E_FAIL;
		}
	*/

	UINT AnimMax = m_pAnimController->GetNumAnimationSets();
	//アニメーションセットを得る.
	for( UINT i = 0; i < AnimMax; i++ )
	{
		m_pAnimController->GetAnimationSet( i, &m_pAnimSet[i] );
	}

	return S_OK;
}

//フレーム内のメッシュ毎にワールド変換行列を更新する.
VOID D3DXPARSER::UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix )
{
	MYFRAME *pFrame = reinterpret_cast<MYFRAME*>( pFrameBase );

	if ( pParentMatrix != nullptr )
	{
		D3DXMatrixMultiply( &pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix );
	}
	else
	{
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;
	}
	if ( pFrame->pFrameSibling != nullptr )
	{
		UpdateFrameMatrices( pFrame->pFrameSibling, pParentMatrix );
	}
	if ( pFrame->pFrameFirstChild != nullptr )
	{
		UpdateFrameMatrices( pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix );
	}
}

//頂点数の取得.
int D3DXPARSER::GetNumVertices( MYMESHCONTAINER* pContainer )
{
	return pContainer->MeshData.pMesh->GetNumVertices();
}

//面数の取得.
int D3DXPARSER::GetNumFaces( MYMESHCONTAINER* pContainer )
{
	return pContainer->MeshData.pMesh->GetNumFaces();
}

//マテリアル数の取得.
int D3DXPARSER::GetNumMaterials( MYMESHCONTAINER* pContainer )
{
	return pContainer->NumMaterials;
}

//UVの取得.
int D3DXPARSER::GetNumUVs( MYMESHCONTAINER* pContainer )
{
	//頂点数と同じ数だけ設定されてるはず.
	return pContainer->MeshData.pMesh->GetNumVertices();
}

//指定されたボーンが影響を及ぼす頂点数を返す.
int D3DXPARSER::GetNumBoneVertices( MYMESHCONTAINER* pContainer, int BoneIndex )
{
	return pContainer->pSkinInfo->GetNumBoneInfluences( BoneIndex );
}

//指定されたボーンが影響を及ぼす頂点のインデックスを返す 第2引数は、影響を受ける頂点のインデックスグループ内のインデックス（インデックスが若い順）
//例えばボーンに影響をうける頂点が４つだとして、そのボーンに影響をうける４つの頂点のうち2番目のインデックスを知りたい場合は、IndexInGroupに1を指定する（0スタートなので）
DWORD D3DXPARSER::GetBoneVerticesIndices(MYMESHCONTAINER* pContainer, int BoneIndex, int IndexInGroup)
{
	auto Num = pContainer->pSkinInfo->GetNumBoneInfluences( BoneIndex );
	auto pVerts = std::make_unique<DWORD[]>(Num);
	auto pWights = std::make_unique<float[]>(Num);

	DWORD dwRslt = 0;

	if ( FAILED(
		pContainer->pSkinInfo->GetBoneInfluence( BoneIndex, pVerts.get(), pWights.get() )))
	{
		MessageBox( nullptr, _T( "ボーン影響を受ける頂点見つからない" ), _T( "error" ), MB_OK );
	}
	else {
		dwRslt = pVerts[IndexInGroup];
	}

	return dwRslt;
}

//指定されたボーンが影響を及ぼす頂点のボーンウェイトを返す 第2引数は、影響を受ける頂点のインデックスグループ内のインデックス（インデックスが若い順）
//例えばボーンに影響をうける頂点が４つだとして、そのボーンに影響をうける４つの頂点のうち2番目の頂点のボーンウェイトを知りたい場合は、IndexInGroupに1を指定する（0スタートなので）
double D3DXPARSER::GetBoneVerticesWeights( MYMESHCONTAINER* pContainer, int BoneIndex, int IndexInGroup )
{
	auto Num = pContainer->pSkinInfo->GetNumBoneInfluences( BoneIndex );
	auto pVerts = std::make_unique<DWORD[]>(Num);
	auto pWights = std::make_unique<float[]>(Num);
	double dRslt = 0.0f;

	if (FAILED(
		pContainer->pSkinInfo->GetBoneInfluence( BoneIndex, pVerts.get(), pWights.get() )))
	{
		MessageBox( nullptr, _T( "ボーン影響を受ける頂点見つからない" ), _T( "error" ), MB_OK );
	}
	else
	{
		dRslt = static_cast<double>( pWights[IndexInGroup] );
	}

	return dRslt;
}

//頂点座標の取得.
D3DXVECTOR3 D3DXPARSER::GetVertexCoord( MYMESHCONTAINER* pContainer, DWORD Index )
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	pContainer->MeshData.pMesh->GetVertexBuffer( &pVB );
	DWORD Stride = pContainer->MeshData.pMesh->GetNumBytesPerVertex();
	BYTE *pVertices = nullptr;
	D3DXVECTOR3 Coord;

	if ( SUCCEEDED(
		pVB->Lock( 0, 0, reinterpret_cast<VOID**>( &pVertices ), 0 )))
	{
		pVertices += Index * Stride;
		Coord = *( reinterpret_cast<D3DXVECTOR3*>( pVertices ) );
		pVB->Unlock();
	}
	SAFE_RELEASE( pVB );

	return Coord;
}

//法線の取得.
D3DXVECTOR3 D3DXPARSER::GetNormal( MYMESHCONTAINER* pContainer, DWORD Index )
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	pContainer->MeshData.pMesh->GetVertexBuffer( &pVB );
	DWORD Stride = pContainer->MeshData.pMesh->GetNumBytesPerVertex();
	BYTE *pVertices = nullptr;
	D3DXVECTOR3 Normal;

	if ( SUCCEEDED(
		pVB->Lock( 0, 0, reinterpret_cast<VOID**>( &pVertices ), 0)))
	{
		pVertices += Index * Stride;
		pVertices += sizeof( D3DXVECTOR3 );	//座標分進める.
		Normal = *( reinterpret_cast<D3DXVECTOR3*>( pVertices ) );
		pVB->Unlock();
	}
	SAFE_RELEASE( pVB );

	return Normal;
}

//テクスチャー座標.
D3DXVECTOR2 D3DXPARSER::GetUV( MYMESHCONTAINER* pContainer, DWORD Index )
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	pContainer->MeshData.pMesh->GetVertexBuffer( &pVB );
	DWORD Stride = pContainer->MeshData.pMesh->GetNumBytesPerVertex();
	BYTE *pVertices = nullptr;
	D3DXVECTOR2 UV;

	if ( SUCCEEDED(
		pVB->Lock( 0, 0, reinterpret_cast<VOID**>( &pVertices ), 0 )))
	{
		pVertices += Index * Stride;
		pVertices += sizeof( D3DXVECTOR3 );	//座標分進める.
		pVertices += sizeof( D3DXVECTOR3 );	//法線分進める.
		UV = *( reinterpret_cast<D3DXVECTOR2*>( pVertices ) );
		pVB->Unlock();
	}
	SAFE_RELEASE( pVB );

	return UV;
}

//インデックスバッファ内の指定した位置（インデックスインデックス）にある頂点インデックスを返す.
int D3DXPARSER::GetIndex( MYMESHCONTAINER* pContainer, DWORD Index )
{
	WORD VertIndex = 0;
	WORD* pIndex = nullptr;
	pContainer->MeshData.pMesh->LockIndexBuffer( D3DLOCK_READONLY, reinterpret_cast<VOID**>( &pIndex ) );

	VertIndex = pIndex[Index];

	pContainer->MeshData.pMesh->UnlockIndexBuffer();

	return VertIndex;
}

//アンビエント（環境光）の取得.
D3DXVECTOR4 D3DXPARSER::GetAmbient( MYMESHCONTAINER* pContainer, int Index )
{
	D3DXCOLOR color = pContainer->pMaterials[Index].MatD3D.Ambient;
	return D3DXVECTOR4( color.r, color.g, color.b, color.a );
}

//ディフューズ（拡散反射光）の取得.
D3DXVECTOR4 D3DXPARSER::GetDiffuse( MYMESHCONTAINER* pContainer, int Index )
{
	D3DXCOLOR color = pContainer->pMaterials[Index].MatD3D.Diffuse;
	return D3DXVECTOR4( color.r, color.g, color.b, color.a );
}

//スペキュラ（鏡面反射光）の取得.
D3DXVECTOR4 D3DXPARSER::GetSpecular( MYMESHCONTAINER* pContainer, int Index )
{
	D3DXCOLOR color = pContainer->pMaterials[Index].MatD3D.Specular;
	return D3DXVECTOR4( color.r, color.g, color.b, color.a );
}

//エミッシブ（自己発光）の取得.
D3DXVECTOR4 D3DXPARSER::GetEmissive( MYMESHCONTAINER* pContainer, int Index )
{
	D3DXCOLOR color = pContainer->pMaterials[Index].MatD3D.Emissive;
	return D3DXVECTOR4( color.r, color.g, color.b, color.a );
}

//テクスチャのパスの取得.
LPSTR D3DXPARSER::GetTexturePath( MYMESHCONTAINER* pContainer, int Index )
{
	return pContainer->pMaterials[Index].pTextureFilename;
}

//スペキュラパワーの取得.
float D3DXPARSER::GetSpecularPower( MYMESHCONTAINER* pContainer, int Index )
{
	return pContainer->pMaterials[Index].MatD3D.Power;
}

//そのポリゴンが、どのマテリアルであるかを返す.
int D3DXPARSER::GeFaceMaterialIndex( MYMESHCONTAINER* pContainer, int FaceIndex )
{
	int MaterialIndex = 0;
	DWORD* pBuf = nullptr;
	if ( SUCCEEDED(
		pContainer->MeshData.pMesh->LockAttributeBuffer( D3DLOCK_READONLY, &pBuf )))
	{
		MaterialIndex = pBuf[FaceIndex];
	}
	else
	{
		MessageBox( nullptr, _T( "属性バッファのロック失敗" ), _T( "error" ), MB_OK );
	}
	return MaterialIndex;
}


//FaceIndex番目のポリゴンを形成する3頂点の中で、IndexInFace番目[0,2]の頂点のインデックスを返す.
int D3DXPARSER::GetFaceVertexIndex( MYMESHCONTAINER* pContainer, int FaceIndex, int IndexInFace )
{
	//インデックスバッファーを調べれば分かる.
	WORD VertIndex = 0;
	WORD* pIndex = nullptr;
	pContainer->MeshData.pMesh->LockIndexBuffer( D3DLOCK_READONLY, reinterpret_cast<VOID**>( &pIndex ) );

	VertIndex = pIndex[FaceIndex * 3 + IndexInFace];

	pContainer->MeshData.pMesh->UnlockIndexBuffer();

	return VertIndex;
}


D3DXMATRIX D3DXPARSER::GetBindPose( MYMESHCONTAINER* pContainer, int BoneIndex )
{
	return *pContainer->pSkinInfo->GetBoneOffsetMatrix( BoneIndex );
}

//この関数をよぶごとにUpdateすると無駄なので、Render関数実行時にUpdateするものとしてそのまま渡す.
//親クラスがこのクラスのUpdateMatricesを実行しない場合は、UpdateMatricesしないと行列が最新にならないので留意.
D3DXMATRIX D3DXPARSER::GetNewPose( MYMESHCONTAINER* pContainer, int BoneIndex )
{
	MYMESHCONTAINER* pMyContaiber = reinterpret_cast<MYMESHCONTAINER*>( pContainer );
	return *pMyContaiber->ppBoneMatrix[BoneIndex];
}


LPCSTR D3DXPARSER::GetBoneName( MYMESHCONTAINER* pContainer, int BoneIndex )
{
	return pContainer->pSkinInfo->GetBoneName( BoneIndex );
}


int D3DXPARSER::GetNumBones( MYMESHCONTAINER* pContainer )
{
	return pContainer->pSkinInfo->GetNumBones();
}

//メッシュコンテナを呼び出す(再帰処理)
LPD3DXMESHCONTAINER D3DXPARSER::GetMeshContainer( LPD3DXFRAME pFrame )
{
	LPD3DXMESHCONTAINER pCon = nullptr;

	//メッシュコンテナあれば返す.
	if ( pFrame->pMeshContainer ) {
		return pFrame->pMeshContainer;
	}

	//無かったら、子のフレームをチェック.
	//そもそも子フレームある？
	if ( pFrame->pFrameFirstChild ) {
		//あればチェックする.
		pCon = GetMeshContainer( pFrame->pFrameFirstChild );
	}

	//子のフレーム最下層までチェックしたが見つからなかった.
	if ( pCon == nullptr ) {
		//兄弟のフレームも探す.
		//そもそも兄弟フレームある？
		if ( pFrame->pFrameSibling ) {
			//あるればチェックする.
			pCon = GetMeshContainer( pFrame->pFrameSibling );
		}
	}

	//見つからない場合はnullptrが入る.
	return pCon;
}

//アニメーションセットの切り替え.
//	第２引数は、個別に制御する用のAnimationControllerをセットする.
//	　不要な場合は、nullptrを設定すること.
void D3DXPARSER::ChangeAnimSet( int Index, LPD3DXANIMATIONCONTROLLER pAC )
{
	D3DXTRACK_DESC TrackDesc;		//アニメーショントラック構造体.

	//※以下3つは、ほぼ固定でOK.
	TrackDesc.Weight	= 1.0f;	//重み.
	TrackDesc.Speed		= 1.0f;	//速さ.
	TrackDesc.Enable	= TRUE;	//有効.

	TrackDesc.Priority = D3DXPRIORITY_LOW;

	TrackDesc.Position = 0.0;		//フレーム位置(開始位置を指定できる)

	LPD3DXANIMATIONCONTROLLER pTmpAC;
	pTmpAC = pAC ? pAC : m_pAnimController;

	//指定(Index）のアニメーショントラックに変更.
	pTmpAC->SetTrackDesc( 0, &TrackDesc );
	pTmpAC->SetTrackAnimationSet( 0, m_pAnimSet[Index] );
	pTmpAC->SetTrackEnable( Index, TRUE );
}

//アニメーションセットの切り替え(開始フレーム指定可能版)
//　第２引数に開始したいフレームを指定する.
//	　完全停止したい場合は、前後でアニメーション速度も0.0fに指定してやる必要がある.
//	第３引数は、個別に制御する用のAnimationControllerをセットする.
//	　不要な場合は、nullptrを設定すること.
void D3DXPARSER::ChangeAnimSet_StartPos(
	int Index, double StartFramePos, LPD3DXANIMATIONCONTROLLER pAC )
{
	D3DXTRACK_DESC TrackDesc;		//アニメーショントラック構造体.

	//※以下3つは、ほぼ固定でOK.
	TrackDesc.Weight = 1.0f;	//重み.
	TrackDesc.Speed	 = 1.0f;	//速さ.
	TrackDesc.Enable = TRUE;	//有効.

	TrackDesc.Priority = D3DXPRIORITY_LOW;
	TrackDesc.Position = StartFramePos;

	LPD3DXANIMATIONCONTROLLER pTmpAC;
	pTmpAC = ( pAC != nullptr ) ? pAC : m_pAnimController;

	//指定(Index）のアニメーショントラックに変更.
	pTmpAC->SetTrackDesc( 0, &TrackDesc );
	pTmpAC->SetTrackAnimationSet( 0, m_pAnimSet[Index] );
	pTmpAC->SetTrackEnable( Index, TRUE );
}

//アニメーション停止時間を取得.
double D3DXPARSER::GetAnimPeriod( int Index)
{
	if( Index < 0 || MAX_ANIM_SET <= Index ) {
		return 0.0;
	}
	return m_pAnimSet[Index]->GetPeriod();
}

//最大アニメーション数を取得.
int D3DXPARSER::GetAnimMax( LPD3DXANIMATIONCONTROLLER pAC )
{
	if( pAC != nullptr ) {
		return pAC->GetNumAnimationSets();
	}
	return m_pAnimController->GetNumAnimationSets();
}

//指定したボーン情報(行列)を取得する関数.
bool D3DXPARSER::GetMatrixFromBone( LPCSTR BoneName, D3DXMATRIX* pOutMat )
{
	LPD3DXFRAME pFrame;
	pFrame = reinterpret_cast<MYFRAME*>( D3DXFrameFind( m_pFrameRoot, BoneName ) );

	if ( pFrame == nullptr ) {
		return false;
	}

	MYFRAME* pMyFrame = reinterpret_cast<MYFRAME*>( pFrame );
	//位置情報を取得.
	*pOutMat = pMyFrame->CombinedTransformationMatrix;

	return true;
}

//指定したボーン情報(座標)を取得する関数.
bool D3DXPARSER::GetPosFromBone( LPCSTR BoneName, D3DXVECTOR3* pOutPos )
{
	D3DXMATRIX mBone;
	if ( GetMatrixFromBone( BoneName, &mBone ) == false ) {
		return false;
	}
	pOutPos->x = mBone._41;
	pOutPos->y = mBone._42;
	pOutPos->z = mBone._43;

	return true;
}

//一括解放処理.
HRESULT D3DXPARSER::Release()
{
	//作成したものを最後に作ったものから順解放していく.
	//基本的には new したものを delete していく.

	//アニメーションセットの解放.
	DWORD AnimMax = m_pAnimController->GetNumAnimationSets();
	for( DWORD i = 0; i < AnimMax; i++ )
	{
		SAFE_RELEASE( m_pAnimSet[i] );
	}

	//アニメーションコントローラ削除.
	SAFE_RELEASE( m_pAnimController );

	//フレームとメッシュコンテナの削除.
	m_pHierarchy->DestroyFrame( m_pFrameRoot );

	//解放処理いる？
	if ( m_pFrameRoot != nullptr ) {
		m_pFrameRoot = nullptr;
	}

	//Hierarchy削除.
	if ( m_pHierarchy != nullptr ) {
		delete m_pHierarchy;
		m_pHierarchy = nullptr;
	}

	return S_OK;
}
