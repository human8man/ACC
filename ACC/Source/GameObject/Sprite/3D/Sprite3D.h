#pragma once
#include "Sprite/SpriteObject.h"

// 警告についてのコード分析を無効にする.4005:再定義
#pragma warning(disable:4005)

// 前方宣言
class DirectX11;


class Sprite3D
	:public SpriteObject
{
public:
	// コンスタントバッファのアプリ側の定義
	//	※シェーダ内のコンスタントバッファと一致している必要あり
	struct SHADER_CONSTANT_BUFFER
	{
		// ワールド,ビュー,プロジェクションの合成変換行列.	
		D3DXMATRIX	mWVP = D3DXMATRIX(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		// カラー（RGBAの型に合わせる）
		D3DXVECTOR4	vColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		// UV座標（x,yのみ使用）
		D3DXVECTOR4	vUV = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	};

public:
	Sprite3D();
	~Sprite3D();

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
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj);

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

	// ビルボードのON/OFF切り替え
	void SetBillboard( bool flag ) { m_Billboard = flag; }

private:
	DirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*		m_pVertexShader;	// 頂点シェーダ
	ID3D11InputLayout*		m_pVertexLayout;	// 頂点レイアウト
	ID3D11PixelShader*		m_pPixelShader;		// ピクセルシェーダ
	ID3D11Buffer*			m_pConstantBuffer;	// コンスタントバッファ

	ID3D11Buffer*			m_pVertexBuffer;	// 頂点バッファ

	ID3D11ShaderResourceView*	m_pTexture;		// テクスチャ
	ID3D11SamplerState*			m_pSampleLinear;// サンプラ:テクスチャに各種フィルタをかける

	D3DXVECTOR2		m_UV;		// テクスチャUV座標

	bool			m_Billboard;	// ビルボードON/OFF
};