#pragma once

//警告についてのコード分析を無効にする.4005:再定義.
#pragma warning(disable:4005)

//_declspec()	:DLLから（関数、クラス、クラスのメンバ関数）エクスポートする.
//align()		:（強制的に）16byteで使用する.
#define ALIGN16	_declspec( align(16) )

//前方宣言.
class CDirectX11;

/**************************************************
*	スプライト2Dクラス.
**/
class CSprite2D
{
public:
	//======================================
	//	構造体.
	//======================================
	//幅高さ構造体.
	struct WHSIZE
	{
		float w;
		float h;
	};
	//スプライト構造体.
	struct SPRITE_STATE
	{
		WHSIZE Disp;	// 表示幅高さ.
		WHSIZE Base;	// 元画像幅高さ.
		WHSIZE Stride;	// 1コマあたりの幅高さ.
		D3DXVECTOR3 Pos;// 画像座標.
	};

	//コンスタントバッファのアプリ側の定義.
	//※シェーダ内のコンスタントバッファと一致している必要あり.
	struct SHADER_CONSTANT_BUFFER
	{
		ALIGN16	D3DXMATRIX	mWorld;		//ワールド行列.	
		ALIGN16	D3DXVECTOR4	vColor;		//カラー（RGBAの型に合わせる）.
		ALIGN16	D3DXVECTOR4	vUV;		//UV座標（x,yのみ使用）.
		ALIGN16 float fViewPortWidth;	//ビューポート幅.
		ALIGN16 float fViewPortHeight;	//ビューポート高さ.
	};
	//頂点の構造体.
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	//頂点座標.
		D3DXVECTOR2	Tex;	//テクスチャ座標.
	};

public:
	CSprite2D();	//コンストラクタ.
	~CSprite2D();	//デストラクタ.

	//初期化.
	HRESULT Init( LPCTSTR lpFileName, SPRITE_STATE& pSs );

	//解放.
	void Release();

	//シェーダ作成.
	HRESULT CreateShader();
	//モデル作成.
	HRESULT CreateModel();
	//テクスチャ作成.
	HRESULT CreateTexture( LPCTSTR lpFileName );
	//サンプラ作成.
	HRESULT CreateSampler();

	//レンダリング用.
	void Render();

	//座標情報を設定.
	void SetPosition(const D3DXVECTOR3& vPos) {	m_vPosition = vPos;	}
	//座標xを設定.
	void SetPositionX( float x ){ m_vPosition.x = x; }
	//座標yを設定.
	void SetPositionY( float y ){ m_vPosition.y = y; }
	//座標zを設定.
	void SetPositionZ( float z ){ m_vPosition.z = z; }

	//回転情報を設定.
	void SetRotation(const D3DXVECTOR3& vRot){	m_vRotation = vRot;	}
	//回転軸Yを設定(Yaw).
	void SetRotationY(float y){	m_vRotation.y = y;	}
	//回転軸Xを設定(Pitch).
	void SetRotationX(float x){	m_vRotation.x = x;	}
	//回転軸Zを設定(Roll).
	void SetRotationZ(float z){	m_vRotation.z = z;	}

	//拡縮情報を設定.
	void SetScale(const D3DXVECTOR3& vScale) { m_vScale = vScale; }

	//α値を設定.
	void SetAlpha(float alpha) { m_Alpha = alpha; }

	//パターン番号(マス目)を設定.
	void SetPatternNo(SHORT x, SHORT y)
	{
		m_PatternNo.x = x;
		m_PatternNo.y = y;
	}
	//最大パターン数(マス目)を取得.
	POINTS GetPatternMax() const { return m_PatternMax; }

private:
	CDirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*		m_pVertexShader;	//頂点シェーダ.
	ID3D11InputLayout*		m_pVertexLayout;	//頂点レイアウト.
	ID3D11PixelShader*		m_pPixelShader;		//ピクセルシェーダ.
	ID3D11Buffer*			m_pConstantBuffer;	//コンスタントバッファ.

	ID3D11Buffer*			m_pVertexBuffer;	//頂点バッファ.

	ID3D11ShaderResourceView*	m_pTexture;			//テクスチャ.
	ID3D11SamplerState*			m_pSampleLinear;	//サンプラ:テクスチャに各種フィルタをかける.

	D3DXVECTOR3		m_vPosition;	//座標.
	D3DXVECTOR3		m_vRotation;	//回転
	D3DXVECTOR3		m_vScale;		//拡縮.

	D3DXVECTOR2		m_UV;		//テクスチャUV座標.

	float			m_Alpha;	//α値(0:透明、1:完全不透明).

	SPRITE_STATE	m_SpriteState;	//スプライト情報.
	POINTS			m_PatternNo;	//パターン番号(マス目).
	POINTS			m_PatternMax;	//最大パターン(マスの最大値).

};