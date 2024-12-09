#pragma once

//警告についてのコード分析を無効にする. 4005:再定義.
#pragma warning( disable:4005)

//_declspec()	:DLLから(関数,クラス,クラスのメンバ関数を)エクスポートする.
//align()		:(強制的に)16byte で使用する.
#define ALIGN16	_declspec( align(16) )

//前方宣言.
class CDirectX9;
class CDirectX11;


/**************************************************
*	スタティックメッシュクラス	
*	メッシュデータをファイルから取り出す為だけにDirectX9を使用する.
*	レンダリング(描画)は、DirectX11で行う.
**/
class CStaticMesh
{
public:
	//======================================
	//	構造体.
	//======================================
	//コンスタントバッファのアプリ側の定義.
	//※シェーダ内のコンスタントバッファと一致している必要あり.
	//メッシュ単位で渡す情報.
	struct CBUFFER_PER_MESH
	{
		D3DXMATRIX	mW;			//ワールド行列.
		D3DXMATRIX	mWVP;		//ワールド,ビュー,プロジェクションの合成変換行列.	
	};
	//マテリアル単位で渡す情報.
	struct CBUFFER_PER_MATERIAL
	{
		D3DXVECTOR4	Diffuse;	//ディフューズ色(拡散反射色).
		D3DXVECTOR4 Ambient;	//アンビエント色(環境色).
		D3DXVECTOR4	Specular;	//スペキュラ色(鏡面反射色).
	};
	//フレーム単位で渡す情報.
	// カメラ,ライト情報はここに入れる.
	struct CBUFFER_PER_FRAME
	{
		D3DXVECTOR4	CameraPos;		//カメラ位置(視点位置).
		D3DXVECTOR4	vLightDir;		//ライト方向.
	};

	//頂点の構造体.
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	//頂点座標.
		D3DXVECTOR3	Normal;	//法線(陰影計算に必須).
		D3DXVECTOR2	UV;		//テクスチャ座標.
	};
	//マテリアル構造体.
	struct MY_MATERIAL
	{
		D3DXVECTOR4	Diffuse;	//ディフューズ.
		D3DXVECTOR4	Ambient;	//アンビエント.
		D3DXVECTOR4	Specular;	//スペキュラ.
		D3DXVECTOR4 Emissive;	//エミッシブ.
		float		Power;		//スペキュラパワー.
		TCHAR TextureName[64];	//テクスチャファイル名.
		ID3D11ShaderResourceView* pTexture;//テクスチャ.
		DWORD dwNumFace;		//マテリアルのポリゴン数.

		//コンストラクタ.
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
		//デストラクタ.
		~MY_MATERIAL() {
			SAFE_RELEASE( pTexture );
		}
	};

	//Xファイルのロードで必要。受け取るもの.
	struct MY_MODEL
	{
		TCHAR			FileName[64];		//ファイル名.
		LPD3DXMESH		pMesh;				//メッシュオブジェクト.
		LPD3DXBUFFER	pD3DXMtrlBuffer;	//マテリアルバッファ.
		DWORD			NumMaterials;		//マテリアル数.

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
	CStaticMesh();		//コンストラクタ.
	~CStaticMesh();		//デストラクタ.

	HRESULT Init( CDirectX9& pDx9, CDirectX11& pDx11, LPCTSTR lpFileName );

	//解放関数.
	void Release();

	//レンダリング用.
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj,
		LIGHT& Light, D3DXVECTOR3& CamPos);

	//座標情報を設定.
	void SetPosition( const D3DXVECTOR3& Pos ) { m_Position = Pos; }
	//座標xを設定.
	void SetPositionX( float x ){ m_Position.x = x; }
	//座標yを設定.
	void SetPositionY( float y ){ m_Position.y = y; }
	//座標zを設定.
	void SetPositionZ( float z ){ m_Position.z = z; }

	//回転情報を設定.
	void SetRotation( const D3DXVECTOR3& Rot ) { m_Rotation = Rot;	}
	//回転軸Yを設定(Yaw).
	void SetRotationY( float y ){ m_Rotation.y = y; }
	//回転軸Xを設定(Pitch).
	void SetRotationX( float x ){ m_Rotation.x = x; }
	//回転軸Zを設定(Roll).
	void SetRotationZ( float z ){ m_Rotation.z = z; }

	//拡縮情報を設定.
	void SetScale( const D3DXVECTOR3& Scale ) {	m_Scale = Scale;	}
	void SetScale( float scale ){
		m_Scale.x = scale;
		m_Scale.y = scale;
		m_Scale.z = scale;
	}

	//メッシュを取得.
	LPD3DXMESH GetMesh() const { return m_Model.pMesh; }
	//レイとの当たり判定用のメッシュを取得.
	LPD3DXMESH GetMeshForRay() const { return m_ModelForRay.pMesh; }
	

private:
	//メッシュ読み込み.
	HRESULT LoadXMesh( LPCTSTR lpFileName );
	//シェーダ作成.
	HRESULT CreateShader();
	//モデル作成.
	HRESULT CreateModel();
	//マテリアル作成.
	HRESULT CreateMaterials();
	//インデックスバッファ作成.
	HRESULT CreateIndexBuffer();
	//頂点バッファ作成.
	HRESULT CreateVertexBuffer();
	//コンスタントバッファ作成.
	HRESULT CreateConstantBuffer();
	//サンプラ作成.
	HRESULT CreateSampler();

	//レンダリング関数(クラス内でのみ使用する).
	void RenderMesh( D3DXMATRIX& mWorld, D3DXMATRIX& mView, D3DXMATRIX& mProj );

private:
	//Dx9.
	CDirectX9*				m_pDx9;
	LPDIRECT3DDEVICE9		m_pDevice9;	//Dx9デバイスオブジェクト.

	//Dx11.
	CDirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;	//デバイスｵﾌﾞｼﾞｪｸﾄ.
	ID3D11DeviceContext*	m_pContext11;	//デバイスコンテキスト.

	ID3D11VertexShader*		m_pVertexShader;	//頂点シェーダ.
	ID3D11InputLayout*		m_pVertexLayout;	//頂点レイアウト.
	ID3D11PixelShader*		m_pPixelShader;		//ピクセルシェーダ.

	ID3D11Buffer*			m_pCBufferPerMesh;		//コンスタントバッファ(メッシュ毎).
	ID3D11Buffer*			m_pCBufferPerMaterial;	//コンスタントバッファ(マテリアル毎).
	ID3D11Buffer*			m_pCBufferPerFrame;		//コンスタントバッファ(フレーム毎).

	ID3D11Buffer*			m_pVertexBuffer;	//頂点バッファ.
	ID3D11Buffer**			m_ppIndexBuffer;	//インデックスバッファ.
	ID3D11SamplerState*		m_pSampleLinear;	//サンプラ:テクスチャに各種フィルタをかける.

	MY_MODEL		m_Model;
	MY_MODEL		m_ModelForRay;	//レイとの当たり判定で使用する.

	MY_MATERIAL*	m_pMaterials;	//マテリアル構造体.
	DWORD			m_NumAttr;		//属性数.
	DWORD			m_AttrID[300];	//属性ID ※300属性まで.

	bool			m_EnableTexture;//テクスチャあり.

	D3DXVECTOR3		m_Position;	//位置(x,y,z).
	D3DXVECTOR3		m_Rotation;	//回転値(x,y,z).
								//※x=Pitch, y=Yaw, z=Roll.
	D3DXVECTOR3		m_Scale;	//拡大縮小値(x,y,z等倍).
 };
