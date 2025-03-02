#pragma once

// 警告についてのコード分析を無効にする。4005：再定義.
#pragma warning( disable : 4005 )

#include <d3dx9.h>
#include <d3d11.h>
#include <d3dx10.h>
#include <d3dx11.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")


// オリジナル　マテリアル構造体.
struct MY_SKINMATERIAL
{
	TCHAR Name[110];
	D3DXVECTOR4	Ambient;			// アンビエント.
	D3DXVECTOR4	Diffuse;			// ディフューズ.
	D3DXVECTOR4	Specular;			// スペキュラ.
	D3DXVECTOR4	Emissive;			// エミッシブ.
	float		SpecularPower;		// スペキュラパワー.
	TCHAR		TextureName[512];	// テクスチャーファイル名.
	ID3D11ShaderResourceView* pTexture;
	DWORD		NumFace;	// そのマテリアルであるポリゴン数.

	MY_SKINMATERIAL()
		: Name			()
		, Ambient		()
		, Diffuse		()
		, Specular		()
		, Emissive		()
		, SpecularPower	()
		, TextureName	()
		, pTexture		( nullptr )
		, NumFace		()
	{
	}
	~MY_SKINMATERIAL()
	{
		SAFE_RELEASE( pTexture );
	}
};


// ボーン構造体.
struct BONE
{
	D3DXMATRIX	mBindPose;		// 初期ポーズ（ずっと変わらない）.
	D3DXMATRIX	mNewPose;		// 現在のポーズ（その都度変わる）.
	DWORD		NumChild;		// 子の数.
	int			ChildIndex[50];	// 自分の子の"インデックス"50個まで.
	char		Name[256];

	BONE()
		: mBindPose		()
		, mNewPose		()
		, NumChild		()
		, ChildIndex	()
		, Name			()
	{
		D3DXMatrixIdentity( &mBindPose );
		D3DXMatrixIdentity( &mNewPose );
	}
};


// パーツメッシュ構造体.
struct SKIN_PARTS_MESH
{
	DWORD				NumVert;
	DWORD				NumFace;
	DWORD				NumUV;
	DWORD				NumMaterial;
	MY_SKINMATERIAL*	pMaterial;
	char				TextureFileName[8][256];	// テクスチャーファイル名(8枚まで).
	bool				EnableTexture;

	ID3D11Buffer*	pVertexBuffer;
	ID3D11Buffer**	ppIndexBuffer;

	// ボーン.
	int		NumBone;
	BONE*	pBoneArray;

	bool	EnableBones;	// ボーンの有無フラグ.

	SKIN_PARTS_MESH()
		: NumVert			()
		, NumFace			()
		, NumUV				()
		, NumMaterial		()
		, pMaterial			( nullptr )
		, TextureFileName	()
		, EnableTexture		()
		, pVertexBuffer		( nullptr )
		, ppIndexBuffer		( nullptr )
		, NumBone			()
		, pBoneArray		( nullptr )
		, EnableBones		()
	{}
};


// 派生フレーム構造体.
//	それぞれのメッシュ用の最終ワールド行列を追加する.
struct MYFRAME
	: public D3DXFRAME
{
	D3DXMATRIX CombinedTransformationMatrix;
	SKIN_PARTS_MESH* pPartsMesh;

	MYFRAME()
		: D3DXFRAME						()
		, CombinedTransformationMatrix	()
		, pPartsMesh					( nullptr )
	{
	}
};


// 派生メッシュコンテナー構造体.
//	コンテナーがテクスチャを複数持てるようにポインターのポインターを追加する
struct MYMESHCONTAINER
	: public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9*	ppTextures;
	DWORD				Weight;				// 重みの個数（重みとは頂点への影響。）.
	DWORD				BoneNum;			// ボーンの数.
	LPD3DXBUFFER		pBoneBuffer;		// ボーン・テーブル.
	D3DXMATRIX**		ppBoneMatrix;		// 全てのボーンのワールド行列の先頭ポインタ.
	D3DXMATRIX*			pBoneOffsetMatrices;// フレームとしてのボーンのワールド行列のポインタ.

	MYMESHCONTAINER()
		: D3DXMESHCONTAINER		()
		, ppTextures			( nullptr )
		, Weight				()
		, BoneNum				()
		, pBoneBuffer			( nullptr )
		, ppBoneMatrix			( nullptr )
		, pBoneOffsetMatrices	( nullptr )
	{}
};
// Xファイル内のアニメーション階層を読み下してくれるクラスを派生させる.
//	ID3DXAllocateHierarchyは派生すること想定して設計されている.
class MY_HIERARCHY
	: public ID3DXAllocateHierarchy
{
public:
	MY_HIERARCHY()
		: ID3DXAllocateHierarchy(){}
	STDMETHOD( CreateFrame )( THIS_ LPCSTR, LPD3DXFRAME * );
	STDMETHOD( CreateMeshContainer )( THIS_ LPCSTR, CONST D3DXMESHDATA*, CONST D3DXMATERIAL*,
		CONST D3DXEFFECTINSTANCE*, DWORD, CONST DWORD *, LPD3DXSKININFO, LPD3DXMESHCONTAINER * );
	STDMETHOD( DestroyFrame )( THIS_ LPD3DXFRAME );
	STDMETHOD( DestroyMeshContainer )( THIS_ LPD3DXMESHCONTAINER );
};


//============================================================================
//		パーサークラス.
//============================================================================
class D3DXPARSER
{
public:
	// 最大ボーン数.
	static constexpr int MAX_BONES	= 255;
	// 最大アニメーションセット数.
	static constexpr int MAX_ANIM_SET = 100;

public:
	D3DXPARSER();
	~D3DXPARSER();

	HRESULT LoadMeshFromX( LPDIRECT3DDEVICE9, LPCTSTR fileName );
	HRESULT AllocateBoneMatrix( LPD3DXMESHCONTAINER );
	HRESULT AllocateAllBoneMatrices( LPD3DXFRAME );
	VOID UpdateFrameMatrices( LPD3DXFRAME, LPD3DXMATRIX );


	// メッシュコンテナを取得する.
	LPD3DXMESHCONTAINER GetMeshContainer( LPD3DXFRAME );

	/****************************************************************
	//	アニメーションセットの切り替え.
	//	@param Index			: アニメーション番号
	//	@param pAC				: AnimationController
	****************************************************************/
	void ChangeAnimSet( int Index, LPD3DXANIMATIONCONTROLLER pAC );

	/****************************************************************
	// アニメーションセットの切り替え(開始フレーム指定可能版).
	// @param Index			: アニメーション番号
	// @param StartFramePos	: 開始フレーム
	// @param pAC			: AnimationController
	****************************************************************/
	void ChangeAnimSet_StartPos( int Index, double StartFramePos, LPD3DXANIMATIONCONTROLLER pAC );

	// 一括解放処理.
	HRESULT Release();

	//------------------------------------------------------------------------
	//		取得関数.
	//------------------------------------------------------------------------
	bool GetMatrixFromBone			(LPCSTR BoneName, D3DXMATRIX* pOutMat);
	bool GetPosFromBone				(LPCSTR BoneName, D3DXVECTOR3* pOutPos);
	int GetAnimMax					(LPD3DXANIMATIONCONTROLLER pAC);
	int GetNumUVs					(MYMESHCONTAINER* pContainer);
	int GetNumBones					(MYMESHCONTAINER* pContainer);
	int GetNumFaces					(MYMESHCONTAINER* pContainer);
	int GetNumVertices				(MYMESHCONTAINER* pContainer);
	int GetNumMaterials				(MYMESHCONTAINER* pContainer);
	int GetIndex					(MYMESHCONTAINER* pContainer, DWORD Index);
	int GetNumBoneVertices			(MYMESHCONTAINER* pContainer, int BoneIndex);
	int GeFaceMaterialIndex			(MYMESHCONTAINER* pContainer, int FaceIndex);
	int GetFaceVertexIndex			(MYMESHCONTAINER* pContainer, int FaceIndex, int IndexInFace);
	double GetAnimPeriod			(int Index);
	double GetBoneVerticesWeights	(MYMESHCONTAINER* pContainer, int BoneIndex, int IndexInGroup);
	float GetSpecularPower			(MYMESHCONTAINER* pContainer, int Index);
	DWORD GetBoneVerticesIndices	(MYMESHCONTAINER* pContainer, int BoneIndex, int IndexInGroup);
	LPSTR GetTexturePath			(MYMESHCONTAINER* pContainer, int Index);
	LPCSTR GetBoneName				(MYMESHCONTAINER* pContainer, int BoneIndex);
	D3DXVECTOR2 GetUV				(MYMESHCONTAINER* pContainer, DWORD Index);
	D3DXVECTOR3 GetNormal			(MYMESHCONTAINER* pContainer, DWORD Index);
	D3DXVECTOR3 GetVertexCoord		(MYMESHCONTAINER* pContainer, DWORD Index);
	D3DXVECTOR4 GetAmbient			(MYMESHCONTAINER* pContainer, int Index);
	D3DXVECTOR4 GetDiffuse			(MYMESHCONTAINER* pContainer, int Index);
	D3DXVECTOR4 GetSpecular			(MYMESHCONTAINER* pContainer, int Index);
	D3DXVECTOR4 GetEmissive			(MYMESHCONTAINER* pContainer, int Index);
	D3DXMATRIX GetNewPose			(MYMESHCONTAINER* pContainer, int BoneIndex);
	D3DXMATRIX GetBindPose			(MYMESHCONTAINER* pContainer, int BoneIndex);

public:
	MY_HIERARCHY	cHierarchy;
	MY_HIERARCHY*	m_pHierarchy;
	LPD3DXFRAME		m_pFrameRoot;

	LPD3DXANIMATIONCONTROLLER	m_pAnimController;			// デフォルトで一つ.
	LPD3DXANIMATIONSET			m_pAnimSet[MAX_ANIM_SET];
};

