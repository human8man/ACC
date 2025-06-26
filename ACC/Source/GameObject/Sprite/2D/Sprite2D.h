#pragma once

// 警告についてのコード分析を無効にする.4005:再定義
#pragma warning(disable:4005)

// _declspec()	: DLLから（関数、クラス、クラスのメンバ関数）エクスポートする
// align()		:（強制的に）16byteで使用する
#define ALIGN16	_declspec( align(16) )

// 前方宣言
class CDirectX11;


class Sprite2D
{
public:
	//------------------------------------------
	//		構造体
	//------------------------------------------
	//幅高さ構造体
	struct WHSIZE
	{
		float w;
		float h;
	};
	// スプライト構造体
	struct SPRITE_STATE
	{
		WHSIZE Disp;		// 表示幅高さ
		WHSIZE Base;		// 元画像幅高さ
		WHSIZE Stride;		// 1コマあたりの幅高さ
		D3DXVECTOR3 Pos;	// 画像座標
		D3DXVECTOR3 Scale;	// スケール
		std::string Path;	// パス
		std::string Name;	// 名前
		bool IsDisp;		// 表示するか
		bool IsGrab;		// つかまれているか(UIEditorで使用)
		bool IsLock;		// 固定するか(UIEditorで使用)

		SPRITE_STATE()
			: Disp		( 0, 0 )
			, Base		( 0, 0 )
			, Stride	( 0, 0 )
			, Pos		( 0.f,0.f,0.f )
			, Scale		( 1.f,1.f,1.f )
			, Path		( "" )
			, Name		( "" )
			, IsDisp	( true )
			, IsGrab	( false )
			, IsLock	( false )
		{}
	};


	// コンスタントバッファのアプリ側の定義
	//	※シェーダ内のコンスタントバッファと一致している必要あり
	struct SHADER_CONSTANT_BUFFER
	{
		ALIGN16	D3DXMATRIX	mWorld;		// ワールド行列
		ALIGN16	D3DXVECTOR4	vColor;		// カラー（RGBAの型に合わせる）
		ALIGN16	D3DXVECTOR4	vUV;		// UV座標（x,yのみ使用）
		ALIGN16 float fViewPortWidth;	// ビューポート幅
		ALIGN16 float fViewPortHeight;	// ビューポート高さ
	};
	// 頂点の構造体
	struct VERTEX
	{
		D3DXVECTOR3 Pos; // 頂点座標
		D3DXVECTOR2	Tex; // テクスチャ座標
	};

public:
	Sprite2D();
	~Sprite2D();

	// 初期化処理
	HRESULT Init( const std::string& lpFileName );

	// 解放処理
	void Release();

	// シェーダ作成
	HRESULT CreateShader();
	// モデル作成
	HRESULT CreateModel();
	// テクスチャ作成
	HRESULT CreateTexture( LPCTSTR lpFileName );
	// サンプラ作成
	HRESULT CreateSampler();

	// レンダリング用
	void Render();

	// 座標情報を設定
	void SetPos(const D3DXVECTOR3& pos) {	m_vPosition = pos;	}
	// 回転情報を設定
	void SetPivot(const D3DXVECTOR3& pivot){	m_vPivot = pivot;}
	// 回転情報を設定
	void SetRot(const D3DXVECTOR3& rot){	m_vRotation = rot;	}
	// 拡縮情報を設定
	void SetScale(const D3DXVECTOR3& scale) { m_vScale = scale; }
	// カラーを設定
	void SetColor(const D3DXVECTOR3& color) { m_vColor = color; }
	// α値を設定
	void SetAlpha(float alpha) { m_Alpha = alpha; }

	// パターン番号(マス目)を設定
	void SetPatternNo(SHORT x, SHORT y) {
		m_PatternNo.x = x;
		m_PatternNo.y = y;
	}

	// 画像の元サイズを設定
	void SetBase(D3DXVECTOR2 base) { 
		m_SpriteState.Base = WHSIZE(base.x, base.y); 
		CreateModel(); 
	}
	// 画像の表示範囲を設定
	void SetDisp(D3DXVECTOR2 disp) { 
		m_SpriteState.Disp = WHSIZE(disp.x, disp.y);
		CreateModel(); 
	}
	// パターン番号を設定した際に乗算される幅、高さの設定
	void SetStride(D3DXVECTOR2 stride) {
		m_SpriteState.Stride = WHSIZE(stride.x, stride.y); 
		CreateModel(); 
	}
	// スプライトのファイルパスを設定
	void SetFilePath(const std::string& FilePath) { 
		m_SpriteState.Path = FilePath; 
	}

	// スプライトのファイルパスを設定
	void SetSpriteName(const std::string& name) {
		m_SpriteState.Name = name;
	}

	// jsonファイルのスプライト情報を取得
	HRESULT SpriteStateDataLoad	(const std::string& FilePath);
	// スプライト情報をまとめたjsonファイルの作成
	HRESULT CreateSpriteState	(const std::string& FilePath);

	// カラーを取得
	D3DXVECTOR3 GetColor()	const { return m_vColor; }	
	// 透過度を取得
	float		GetAlpha()	const { return m_Alpha; }
	// スケール値を取得
	D3DXVECTOR3 GetScale()	const{ return m_vScale;}
	// 座標情報を取得
	D3DXVECTOR3 GetPos()	const{ return m_vPosition;}
	// 基準座標情報を取得
	D3DXVECTOR3 GetPivot()	const{ return m_vPivot;}
	// 回転情報を取得
	D3DXVECTOR3 GetRot()	const{ return m_vRotation;}
	// 最大パターン数(マス目)を取得
	POINTS GetPatternMax()	const { return m_PatternMax; }
	// 画像情報を取得
	SPRITE_STATE GetSpriteData()const { return m_SpriteState; }
private:
	CDirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*		m_pVertexShader;	// 頂点シェーダ
	ID3D11InputLayout*		m_pVertexLayout;	// 頂点レイアウト
	ID3D11PixelShader*		m_pPixelShader;		// ピクセルシェーダ
	ID3D11Buffer*			m_pConstantBuffer;	// コンスタントバッファ

	ID3D11Buffer*			m_pVertexBuffer;	// 頂点バッファ

	ID3D11ShaderResourceView*	m_pTexture;		// テクスチャ
	ID3D11SamplerState*			m_pSampleLinear;// サンプラ:テクスチャに各種フィルタをかける

	D3DXVECTOR3		m_vPosition;	// 座標
	D3DXVECTOR3		m_vPivot;	// 回転軸座標
	D3DXVECTOR3		m_vRotation;	// 回転
	D3DXVECTOR3		m_vScale;		// 拡縮

	D3DXVECTOR2		m_UV;		// テクスチャUV座標

	D3DXVECTOR3		m_vColor;	// カラー
	float			m_Alpha;	// α値(0:透明、1:完全不透明)

	SPRITE_STATE	m_SpriteState;		// スプライト情報
	POINTS			m_PatternNo;		// パターン番号(マス目)
	POINTS			m_PatternMax;		// 最大パターン(マスの最大値)

};