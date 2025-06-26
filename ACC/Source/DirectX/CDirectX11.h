#pragma once

// 警告についてのコード分析を無効にする.4005:再定義
#pragma warning(disable:4005)

#include "Singleton/CSingleton.h"


//==================================================
//		DirectX11 セットアップ
//==================================================
class CDirectX11
	: public CSingleton<CDirectX11>
{
private:
	friend class CSingleton<CDirectX11>;

public:
	CDirectX11();
	~CDirectX11();

	// 作成処理
	HRESULT Create(HWND hWnd);
	// 解放処理
	void Release();

	// 深度（Ｚ）テストON/OFF切り替え
	void SetDepth( bool flag );

	// アルファブレンドON/OFF切り替え
	void SetAlphaBlend( bool flag );

	// バックバッファクリア関数
	void ClearBackBuffer();

	// 表示処理
	void Present();
	// デバイスを取得
	ID3D11Device* GetDevice()			const { return m_pDevice11; }
	// スワップチェーンを取得
	IDXGISwapChain* GetSwapChain()		const { return m_pSwapChain; }
	// デバイスコンテキストを取得
	ID3D11DeviceContext* GetContext()	const { return m_pContext11; }
	// レンダーターゲットビューを取得
	ID3D11RenderTargetView* GetTexRTV()	const { return m_pBackBuffer_TexRTV; }
	// デバッグモード切替
	void SetChangeDebugBuffer(bool flag) { m_IsDebug = flag; }

private:
	// デバイスとスワップチェイン作成
	HRESULT CreateDeviceAndSwapChain( HWND hWnd, UINT uFPS, UINT uWidth, UINT uHeight );

	// バックバッファ作成:カラー用レンダーターゲットビュー作成
	HRESULT CreateColorBackBufferRTV();

	// バックバッファ作成:デプスステンシル用レンダーターゲットビュー作成
	HRESULT CreateDepthStencilBackBufferRTV();

	// ラスタライザ作成
	HRESULT CreateRasterizer();

	// デプスステンシルステート作成
	HRESULT CreateDepthStencilState();

	// アルファブレンドステート作成
	HRESULT CreateAlphaBlendState();

private:
	ID3D11Device*				m_pDevice11;			// デバイスオブジェクト
	ID3D11DeviceContext*		m_pContext11;			// デバイスコンテキスト
	IDXGISwapChain*				m_pSwapChain;			// スワップチェーン
	ID3D11RenderTargetView*		m_pBackBuffer_TexRTV;	// レンダーターゲットビュー
	ID3D11Texture2D*			m_pBackBuffer_DSTex;	// デプスステンシル用テクスチャ
	ID3D11DepthStencilView*		m_pBackBuffer_DSTexDSV;	// デプスステンシルビュー

	// 深度（Ｚ）テスト設定
	ID3D11DepthStencilState*	m_pDepthStencilStateOn;		// 有効設定
	ID3D11DepthStencilState*	m_pDepthStencilStateOff;	// 無効設定

	// アルファブレンド
	ID3D11BlendState*		m_pAlphaBlendOn;	// 有効設定
	ID3D11BlendState*		m_pAlphaBlendOff;	// 無効設定

	// バックバッファのカラー
	D3DXVECTOR4 m_BufferColor;

	bool m_IsDebug;
};