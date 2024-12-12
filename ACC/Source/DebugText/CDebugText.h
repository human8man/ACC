#pragma once

//警告についてのコード分析を無効にする.4005:再定義.
#pragma warning(disable:4005)

//_declspec()	:DLLから（関数、クラス、クラスのメンバ関数）エクスポートする.
//align()		:（強制的に）16byteで使用する.
#define ALIGN16	_declspec( align(16) )

//前方宣言.
class CDirectX11;

/**************************************************
*	デバッグテキストクラス.
**/
class CDebugText
{
public:
	//定数宣言.
	static constexpr int SPRITE_MAX_W = 10;
	static constexpr int SPRITE_MAX_H = 10;
	static constexpr int SPRITE_MAX = SPRITE_MAX_W * SPRITE_MAX_H;

	//======================================
	//	構造体.
	//======================================
	//コンスタントバッファのアプリ側の定義.
	//※シェーダ内のコンスタントバッファと一致している必要あり.
	struct SHADER_CONSTANT_BUFFER
	{
		ALIGN16	D3DXMATRIX	mWorld;		//ワールド行列.	
		ALIGN16	D3DXVECTOR4	vColor;		//カラー（RGBAの型に合わせる）.
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
	CDebugText();	//コンストラクタ.
	~CDebugText();	//デストラクタ.

	//初期化.
	HRESULT Init();

	//解放.
	void Release();

	//レンダリング用.
	void Render( LPCTSTR text, int x, int y );

	//α値を設定.
	void SetAlpha( float alpha ) { m_Alpha = alpha; }

	//色を設定.
	void SetColor( float r, float g, float b ){
		m_Color.x = r;
		m_Color.y = g;
		m_Color.z = b;
	}

private:
	//シェーダ作成.
	HRESULT CreateShader();
	//モデル作成.
	HRESULT CreateModel();
	//テクスチャ作成.
	HRESULT CreateTexture( LPCTSTR lpFileName );
	//サンプラ作成.
	HRESULT CreateSampler();
	
	//フォントレンダリング.
	void RenderFont( int FontIndex, float x, float y );

private:
	CDirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*		m_pVertexShader;	//頂点シェーダ.
	ID3D11InputLayout*		m_pVertexLayout;	//頂点レイアウト.
	ID3D11PixelShader*		m_pPixelShader;		//ピクセルシェーダ.
	ID3D11Buffer*			m_pConstantBuffer;	//コンスタントバッファ.

	ID3D11Buffer*			m_pVertexBuffer[SPRITE_MAX];	//頂点バッファ.

	ID3D11ShaderResourceView*	m_pTexture;			//テクスチャ.
	ID3D11SamplerState*			m_pSampleLinear;	//サンプラ:テクスチャに各種フィルタをかける.

	float			m_Alpha;	//α値(0:透明、1:完全不透明).

	D3DXVECTOR3		m_Color;				//色(RGB).
	float			m_Kerning[SPRITE_MAX];	//カーニング.
};