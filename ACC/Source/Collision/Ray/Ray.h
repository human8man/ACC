#pragma once

// 警告についてのコード分析を無効にする.4005:再定義
#pragma warning(disable:4005)

// 前方宣言
class DirectX11;


//------------------------------------------------------
//	レイ構造体
//------------------------------------------------------
struct RAY
{
	D3DXVECTOR3	Axis;		// 軸
	D3DXVECTOR3	Position;	// 位置
	float		Length;		// 長さ
	float		RotationY;	// Y軸回転値

	RAY() : Axis(), Position(), Length(), RotationY() {}
};


//------------------------------------------------------
//		レイ当たり判定情報構造体
//------------------------------------------------------
struct RayInfo{
	bool		Hit;	// Hit判定
	D3DXVECTOR3	HitPos;	// Hit座標
	FLOAT		Length;	// レイ開始地点から終点までの長さ
};


//------------------------------------------------------
//	十字レイ構造体
//------------------------------------------------------
struct CROSSRAY
{
	enum enDir { ZF, ZB, XL, XR, max };
	RAY Ray[enDir::max];

	CROSSRAY() : Ray()
	{
		Ray[enDir::ZF].Axis = D3DXVECTOR3( 0.f, 0.f, 1.f );
		Ray[enDir::ZB].Axis = D3DXVECTOR3( 0.f, 0.f,-1.f );
		Ray[enDir::XL].Axis = D3DXVECTOR3(-1.f, 0.f, 0.f );
		Ray[enDir::XR].Axis = D3DXVECTOR3( 1.f, 0.f, 0.f );

		for( int i = 0; i < enDir::max; i++ ){
			Ray[i].Length = 1.f;
		}
	}
};


//============================================================================
//		レイ（線分）の表示クラス
//============================================================================
class Ray
{
public:
	//------------------------------------------------------
	//	構造体
	//------------------------------------------------------
	// コンスタントバッファのアプリ側の定義
	//	※シェーダ内のコンスタントバッファと一致している必要あり
	struct SHADER_CONSTANT_BUFFER
	{
		D3DXMATRIX	mWVP;
		D3DXVECTOR4	vColor;
	};
	// 頂点の構造体
	struct VERTEX
	{
		D3DXVECTOR3 Pos;
	};

public:
	Ray();
	~Ray();

	// 初期化
	HRESULT Init( RAY& pRay);

	// 解放
	void Release();

	// シェーダ作成
	HRESULT CreateShader();
	// モデル作成
	HRESULT CreateModel();

	// レンダリング用
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj, RAY Ray);

private:
	DirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*		m_pVertexShader;	// 頂点シェーダ
	ID3D11InputLayout*		m_pVertexLayout;	// 頂点レイアウト
	ID3D11PixelShader*		m_pPixelShader;		// ピクセルシェーダ
	ID3D11Buffer*			m_pConstantBuffer;	// コンスタントバッファ

	ID3D11Buffer*			m_pVertexBuffer;	// 頂点バッファ

	D3DXVECTOR3		m_vPosition;	// 座標
	D3DXVECTOR3		m_vRotation;	// 回転

	RAY	m_Ray;	// レイ構造体
};