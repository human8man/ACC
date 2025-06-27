#pragma once

// 警告についてのコード分析を無効にする. 4005:再定義
#pragma warning( disable:4005)

// _declspec()	: DLLから(関数,クラス,クラスのメンバ関数を)エクスポートする
// align()		: (強制的に)16byte で使用する
#define ALIGN16	_declspec( align(16) )

#include "Collision/Ray/Ray.h"

// 前方宣言
class DirectX9;
class DirectX11;


class StaticMesh
{
public:
	//---------------------------------------------
	//	構造体
	//---------------------------------------------
	struct CBUFFER_PER_MESH
	{
		D3DXMATRIX	mW;		// ワールド行列
		D3DXMATRIX	mWVP;	// ワールド,ビュー,プロジェクションの合成変換行列.	
	};
	// マテリアル単位で渡す情報
	struct CBUFFER_PER_MATERIAL
	{
		D3DXVECTOR4	Diffuse;	// ディフューズ色(拡散反射色)
		D3DXVECTOR4 Ambient;	// アンビエント色(環境色)
		D3DXVECTOR4	Specular;	// スペキュラ色(鏡面反射色)
	};
	// フレーム単位で渡す情報
	struct CBUFFER_PER_FRAME
	{
		D3DXVECTOR4	CameraPos;	// カメラ位置(視点位置)
		D3DXVECTOR4	vLightDir;	// ライト方向
	};

	// 頂点の構造体
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	// 頂点座標
		D3DXVECTOR3	Normal;	// 法線(陰影計算に必須)
		D3DXVECTOR2	UV;		// テクスチャ座標
	};
	// マテリアル構造体
	struct MY_MATERIAL
	{
		D3DXVECTOR4	Diffuse;	// ディフューズ
		D3DXVECTOR4	Ambient;	// アンビエント
		D3DXVECTOR4	Specular;	// スペキュラ
		D3DXVECTOR4 Emissive;	// エミッシブ
		float		Power;		// スペキュラパワー
		TCHAR TextureName[64];	// テクスチャファイル名
		ID3D11ShaderResourceView* pTexture;// テクスチャ
		DWORD dwNumFace;		// マテリアルのポリゴン数

		// コンストラクタ
		MY_MATERIAL()
			: Diffuse		()
			, Ambient		()
			, Specular		()
			, Emissive		()
			, Power			()
			, TextureName	()
			, pTexture		( nullptr )
			, dwNumFace		( 0 )
		{
		}
		// デストラクタ
		~MY_MATERIAL() {
			SAFE_RELEASE( pTexture );
		}
	};

	// Xファイルのロードで必要。受け取るもの
	struct MY_MODEL
	{
		TCHAR			FileName[64];	 // ファイル名
		LPD3DXMESH		pMesh;			 // メッシュオブジェクト
		LPD3DXBUFFER	pD3DXMtrlBuffer; // マテリアルバッファ
		DWORD			NumMaterials;	 // マテリアル数

		MY_MODEL()
			: FileName			()
			, pMesh				( nullptr )
			, pD3DXMtrlBuffer	( nullptr )
			, NumMaterials		()
		{}
		~MY_MODEL(){
			SAFE_RELEASE( pMesh );
			SAFE_RELEASE( pD3DXMtrlBuffer );
		}
	};

public:
	StaticMesh();
	~StaticMesh();

	// 初期化
	HRESULT Init( LPCTSTR lpFileName );

	// 解放関数
	void Release();

	// レンダリング用
	void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light );

	// 座標情報の設定
	void SetPos( const D3DXVECTOR3& pos )	{ m_Position = pos; }
	void SetPos(float x, float y, float z)	{ m_Position = D3DXVECTOR3(x, y, z); }
	void SetPosX( float x ){ m_Position.x = x; }
	void SetPosY( float y ){ m_Position.y = y; }
	void SetPosZ( float z ){ m_Position.z = z; }

	// 回転情報を設定
	void SetRot(const D3DXVECTOR3& rot) { m_Rotation = rot; }
	void SetRot(float rot) { m_Rotation = D3DXVECTOR3(rot, rot, rot); }
	void SetRotY( float y ){ m_Rotation.y = y; }
	void SetRotX( float x ){ m_Rotation.x = x; }
	void SetRotZ( float z ){ m_Rotation.z = z; }

	// 拡縮情報を設定
	void SetScale( const D3DXVECTOR3& scale ) {	m_Scale = scale; }
	void SetScale(float x, float y, float z) { m_Scale = D3DXVECTOR3(x, y, z); }

	D3DXVECTOR3 GetScale()	const { return m_Scale; }		// サイズを取得
	D3DXVECTOR3 GetPos()	const { return m_Position; }	// メッシュ位置を取得
	D3DXVECTOR3 GetRot()	const { return m_Rotation; }	// メッシュ角度を取得
	// オブジェクト情報(座標情報,回転情報,サイズ)を取得
	ObjectInfo GetObjeInfo() const { return ObjectInfo{ m_Position, m_Rotation, m_Scale }; }

	LPD3DXMESH GetMesh()	const { return m_Model.pMesh; }	// メッシュを取得
	LPD3DXMESH GetMeshForRay()	const { return m_ModelForRay.pMesh; }		// レイとの当たり判定用のメッシュを取得
	std::vector<D3DXVECTOR3>GetVertices()	const { return m_Vertices; }	// メッシュの頂点情報を取得
	ID3D11Buffer*		GetVertexBuffer() const { return m_pVertexBuffer; }
	ID3D11InputLayout*	GetVertexLayout() const { return m_pVertexLayout; }

	// インデックスバッファを取得
	std::vector<ID3D11Buffer*> GetIndexBuffer() const;
	// インデックスを取得
	std::vector<DWORD> GetIndex() const;

	// レイとメッシュの当たり判定( Hit判定、交差点、長さを返す)
	RayInfo IsHitForRay( const RAY& pRay  );
private:
	HRESULT CreateModel();					// モデル作成
	HRESULT CreateShader();					// シェーダ作成
	HRESULT CreateSampler();				// サンプラ作成
	HRESULT CreateMaterials();				// マテリアル作成
	HRESULT CreateIndexBuffer();			// インデックスバッファ作成
	HRESULT CreateVertexBuffer();			// 頂点バッファ作成
	HRESULT CreateConstantBuffer();			// コンスタントバッファ作成
	HRESULT LoadXMesh( LPCTSTR lpFileName );// メッシュ読み込み

	// レンダリング関数(クラス内でのみ使用する)
	void RenderMesh( D3DXMATRIX& mWorld, D3DXMATRIX& mView, D3DXMATRIX& mProj );
	// 各頂点情報の保存
	void SaveVertices(LPD3DXMESH pMesh);

	// 交差位置のポリゴンの頂点を見つける
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		D3DXVECTOR3* pVertices);
private:
	DirectX9*				m_pDx9;
	LPDIRECT3DDEVICE9		m_pDevice9;	// Dx9デバイスオブジェクト

	DirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;	// デバイスオブジェクト
	ID3D11DeviceContext*	m_pContext11;	// デバイスコンテキスト

	ID3D11VertexShader*		m_pVertexShader;	// 頂点シェーダ
	ID3D11InputLayout*		m_pVertexLayout;	// 頂点レイアウト
	ID3D11PixelShader*		m_pPixelShader;		// ピクセルシェーダ

	ID3D11Buffer*			m_pCBufferPerMesh;		// コンスタントバッファ(メッシュ毎)
	ID3D11Buffer*			m_pCBufferPerMaterial;	// コンスタントバッファ(マテリアル毎)
	ID3D11Buffer*			m_pCBufferPerFrame;		// コンスタントバッファ(フレーム毎)

	ID3D11Buffer*			m_pVertexBuffer;	// 頂点バッファ
	ID3D11Buffer**			m_ppIndexBuffer;	// インデックスバッファ
	ID3D11SamplerState*		m_pSampleLinear;	// サンプラ:テクスチャに各種フィルタをかける

	MY_MODEL		m_Model;
	MY_MODEL		m_ModelForRay;			// レイとの当たり判定で使用する
	std::vector<D3DXVECTOR3> m_Vertices;	// メッシュコライダーで使用

	MY_MATERIAL*	m_pMaterials;	// マテリアル構造体
	DWORD			m_NumAttr;		// 属性数
	DWORD			m_AttrID[300];	// 属性ID ※300属性まで

	bool			m_EnableTexture;// テクスチャあり

	D3DXVECTOR3		m_Position;	// 位置(x,y,z)
	D3DXVECTOR3		m_Rotation;	// 回転値(x,y,z)x=Pitch, y=Yaw, z=Roll
	D3DXVECTOR3		m_Scale;	// 拡大縮小値(x,y,z等倍)
 };
