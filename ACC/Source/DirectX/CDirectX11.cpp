#include "../Common/MyMacro.h"
#include "DirectX/CDirectX11.h"


//============================================================================
//		CDirectX11クラス
//============================================================================
CDirectX11::CDirectX11()
	: m_pDevice11				( nullptr )
	, m_pContext11				( nullptr )
	, m_pSwapChain				( nullptr )
	, m_pBackBuffer_TexRTV		( nullptr )
	, m_pBackBuffer_DSTex		( nullptr )
	, m_pBackBuffer_DSTexDSV	( nullptr )

	, m_pDepthStencilStateOn	( nullptr )
	, m_pDepthStencilStateOff	( nullptr )

	, m_pAlphaBlendOn			( nullptr )
	, m_pAlphaBlendOff			( nullptr )
{
}

CDirectX11::~CDirectX11()
{
	Release();
}


//============================================================================
//		作成処理.
//============================================================================
HRESULT CDirectX11::Create(HWND hWnd)
{
	// デバイスとスワップチェインを作成.
	if( FAILED( CreateDeviceAndSwapChain(
			hWnd,
			static_cast<UINT>(FPS),
			static_cast<UINT>(WND_W),
			static_cast<UINT>(WND_H) )))
	{
		return E_FAIL;
	}

	// 各種テクスチャとそれに付帯する各種ビュー（画面）を作成.
	//	バックバッファ準備：カラーバッファレンダーターゲットビュー.
	if( FAILED(CreateColorBackBufferRTV() ) ) { return E_FAIL; }
	
	// バックバッファ準備：デプス（深度）ステンシルレンダーターゲットビュー.
	if( FAILED( CreateDepthStencilBackBufferRTV() ) ) { return E_FAIL; }

	// レンダーターゲットビューとデプスステンシルビューをパイプラインにセット.
	m_pContext11->OMSetRenderTargets( 1, &m_pBackBuffer_TexRTV, m_pBackBuffer_DSTexDSV );
	
	// デプスステンシル設定の作成.
	if( FAILED( CreateDepthStencilState() ) ){ return E_FAIL; }

	// アルファブレンドステートの作成.
	if( FAILED( CreateAlphaBlendState() ) ){ return E_FAIL; }

	//------------------------------------------------
	//	ビューポート設定.
	//------------------------------------------------
	D3D11_VIEWPORT vp;
	vp.Width	= WND_W;	// 幅.
	vp.Height	= WND_H;	// 高さ.
	vp.MinDepth = 0.f;		// 最小深度（手前）.
	vp.MaxDepth = 1.f;		// 最大深度（奥）.
	vp.TopLeftX = 0.f;		// 左上位置x.
	vp.TopLeftY = 0.f;		// 左上位置y.

	// ラスタライザーステージにビューポートを設定する.
	m_pContext11->RSSetViewports( 1, &vp );

	// ラスタライズ（面の塗りつぶし方）の設定.
	if( FAILED( CreateRasterizer() ) ){ return E_FAIL; }

	return S_OK;
}


//============================================================================
//		解放処理.
//============================================================================
void CDirectX11::Release()
{
	SAFE_RELEASE( m_pAlphaBlendOff );
	SAFE_RELEASE( m_pAlphaBlendOn );

	SAFE_RELEASE( m_pDepthStencilStateOff );
	SAFE_RELEASE( m_pDepthStencilStateOn );

	SAFE_RELEASE( m_pBackBuffer_DSTexDSV );
	SAFE_RELEASE( m_pBackBuffer_DSTex );
	SAFE_RELEASE( m_pBackBuffer_TexRTV );

	SAFE_RELEASE( m_pSwapChain );
	SAFE_RELEASE( m_pContext11 );
	SAFE_RELEASE( m_pDevice11 );
}


//----------------------------------------------------------------------------
//		デバイスとスワップチェーンの作成.
//----------------------------------------------------------------------------
HRESULT CDirectX11::CreateDeviceAndSwapChain( HWND hWnd, UINT uFPS, UINT uWidth, UINT uHeight )
{
	// スワップチェーン構造体.
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );							// 0で初期化.
	sd.BufferCount			= 1;								// バックバッファの数.
	sd.BufferDesc.Width		= uWidth;							// バックバッファの幅.
	sd.BufferDesc.Height	= uHeight;							// バックバッファの高さ.
	sd.BufferDesc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;		// フォーマット（32ビットカラー）.
	sd.BufferDesc.RefreshRate.Numerator		= uFPS;				// リフレッシュレート（分母） ※FPS:60.
	sd.BufferDesc.RefreshRate.Denominator	= 1;				// リフレッシュレート（分子）.
	sd.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 使い方（表示先）.
	sd.OutputWindow			= hWnd;								// ウィンドウハンドル.
	sd.SampleDesc.Count		= 1;								// マルチサンプルの数.
	sd.SampleDesc.Quality	= 0;								// マルチサンプルのクオリティ.
	sd.Windowed				= TRUE;								// ウィンドウモード（フルスクリーン時はFALSE）.


	// 作成を試みる機能レベルの優先を指定.
	//	(GPUがサポートする機能セットの定義).
	//	D3D_FEATURE_LEVEL列挙型の配列.
	//	D3D_FEATURE_LEVEL_11_0:Direct3D 11.0 の GPUレベル.
	D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL* pFeatureLevel = nullptr;// 配列の要素数.

	// デバイスとスワップチェーンの作成.
	// デバイス作成処理を関数化.
	auto TryCreateDevice = [&](D3D_DRIVER_TYPE driverType) -> bool {
		return SUCCEEDED(D3D11CreateDeviceAndSwapChain(
			nullptr, driverType, nullptr, 0,
			&pFeatureLevels, 1, D3D11_SDK_VERSION,
			&sd, &m_pSwapChain, &m_pDevice11, nullptr, &m_pContext11));
	};

	// ハードウェア → WARP（ソフトウェア）→ リファレンスの順で作成を試みる.
	if (!(TryCreateDevice(D3D_DRIVER_TYPE_HARDWARE)	||
		  TryCreateDevice(D3D_DRIVER_TYPE_WARP)		||
		  TryCreateDevice(D3D_DRIVER_TYPE_REFERENCE)))
	{
		MessageBox(nullptr, _T("デバイスとスワップチェーン作成失敗"), _T("Error"), MB_OK);
		return E_FAIL;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//		ラスタライザ作成.
//-----------------------------------------------------------------------------
HRESULT CDirectX11::CreateRasterizer()
{
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory( &rdc, sizeof( rdc ) );
	rdc.FillMode = D3D11_FILL_SOLID;// 塗りつぶし（ソリッド）.

	// カリングの設定.
	//	D3D11_CULL_BACK		:背面を描画しない.
	//	D3D11_CULL_FRONT	:正面を描画しない.
	//	D3D11_CULL_NONE		:カリングを切る（正背面を描画する）.
	rdc.CullMode = D3D11_CULL_NONE;

	// ポリゴンの表裏を決定するフラグ.
	// TRUE		:左回りなら前向き。右回りなら後ろ向き。
	// FALSE	:逆になる.
	rdc.FrontCounterClockwise = FALSE;

	// 距離についてのクリッピング有効.
	rdc.DepthClipEnable = FALSE;

	ID3D11RasterizerState* pRs = nullptr;
	m_pDevice11->CreateRasterizerState( &rdc, &pRs );
	m_pContext11->RSSetState( pRs );
	SAFE_RELEASE( pRs );

	return S_OK;
}


//-----------------------------------------------------------------------------
//		デプスステンシル設定.
//-----------------------------------------------------------------------------
HRESULT CDirectX11::CreateDepthStencilState()
{
	// 深度テスト（Ｚテスト）の設定.
	//  ※ON/OFFの共通部分のみ設定.
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc		= D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable	= FALSE;
	dsDesc.StencilReadMask	= D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask	= D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// 深度テスト（Ｚテスト）を有効にする.
	dsDesc.DepthEnable = TRUE; // 有効.
	// 深度設定作成.
	if( FAILED( m_pDevice11->CreateDepthStencilState( &dsDesc, &m_pDepthStencilStateOn ) ) )
	{
		_ASSERT_EXPR( false, _T("深度ON設定作成失敗") );
		return E_FAIL;
	}

	// 深度テスト（Ｚ）テストを無効にする.
	dsDesc.DepthEnable = FALSE;// 無効.
	// 深度設定作成.
	if( FAILED( m_pDevice11->CreateDepthStencilState( &dsDesc, &m_pDepthStencilStateOff ) ) )
	{
		_ASSERT_EXPR( false, _T( "深度OFF設定作成失敗" ) );
		return E_FAIL;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//		ブレンドステート作成.
//-----------------------------------------------------------------------------
HRESULT CDirectX11::CreateAlphaBlendState()
{
	// アルファブレンド用ブレンドステート構造体.
	// pngファイル内にアルファ情報があるので、透過するようにブレンドステートで設定する.
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory( &BlendDesc, sizeof( BlendDesc ) );	// 初期化.

	BlendDesc.IndependentBlendEnable = false;	// false	:RenderTarget[0]のメンバのみ使用する.
												// true	:RenderTarget[0～7]が使用できる.
												// （レンダーターゲット毎に独立したブレンド処理）.
	
	BlendDesc.AlphaToCoverageEnable					= false;						// アルファトゥカバレージを使用する.
	BlendDesc.RenderTarget[0].SrcBlend				= D3D11_BLEND_SRC_ALPHA;		// 元素材に対する設定(アルファブレンドを指定).
	BlendDesc.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;	// 重ねる素材に対する設定(アルファブレンドの反転を指定).
	BlendDesc.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;			// ブレンドオプション(ADD:加算合成).
	BlendDesc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;				// 元素材のアルファに対する指定(そのまま使用).
	BlendDesc.RenderTarget[0].DestBlendAlpha		= D3D11_BLEND_ZERO;				// 重ねる素材のアルファに対する設定.
	BlendDesc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;			// アルファのブレンドオプション(ADD:加算合成).
	BlendDesc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;	// ピクセル毎の書き込みマスク(全ての成分（RGBA）へのデータの格納を許可する).
	BlendDesc.RenderTarget[0].BlendEnable			= true;							// アルファブレンドを使用する.
	
	// ブレンドステート作成.
	if( FAILED( m_pDevice11->CreateBlendState( &BlendDesc, &m_pAlphaBlendOn ) ) )
	{
		_ASSERT_EXPR( false, _T( "アルファブレンドステート(ON)作成失敗" ) );
		return E_FAIL;
	}

	// アルファブレンドを使用する.
	BlendDesc.RenderTarget[0].BlendEnable = false;	// 無効.
	// ブレンドステート作成.
	if( FAILED(m_pDevice11->CreateBlendState( &BlendDesc, &m_pAlphaBlendOff ) ) )
	{
		_ASSERT_EXPR( false, _T( "アルファブレンドステート(OFF)作成失敗" ) );
		return E_FAIL;
	}

	return S_OK;
}


//============================================================================
//		透過設定の切り替え.
//============================================================================
void CDirectX11::SetAlphaBlend( bool flag )
{
	UINT mask = 0xffffffff;	// マスク値.
	ID3D11BlendState* pTmp = ( flag == true ) ? m_pAlphaBlendOn : m_pAlphaBlendOff;

	// アルファブレンド設定をセット.
	m_pContext11->OMSetBlendState( pTmp, nullptr, mask );
}


//============================================================================
//		深度（Ｚ）テストON/OFF切り替え.
//============================================================================
void CDirectX11::SetDepth(bool flag)
{
	ID3D11DepthStencilState* pTmp
		= ( flag == true ) ? m_pDepthStencilStateOn : m_pDepthStencilStateOff;

	// 深度設定をセット.
	m_pContext11->OMSetDepthStencilState( pTmp, 1 );
}


//============================================================================
//		バックバッファクリア関数.
//============================================================================
void CDirectX11::ClearBackBuffer()
{
	// 画面のクリア.
	float ClearColor[4] = { 0.f, 0.f, 0.f, 1.f };	//クリア色（RGBAの順）.
	// カラーバックバッファ.
	m_pContext11->ClearRenderTargetView(
		m_pBackBuffer_TexRTV, ClearColor );
	// デプスステンシルバックバッファ.
	m_pContext11->ClearDepthStencilView(
		m_pBackBuffer_DSTexDSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0 );
	// レンダーターゲットと深度/ステンシルビューを設定
	m_pContext11->OMSetRenderTargets(
		1,
		&m_pBackBuffer_TexRTV,
		m_pBackBuffer_DSTexDSV);
}


//============================================================================
//		表示処理.
//============================================================================
void CDirectX11::Present()
{
	m_pSwapChain->Present( 0, 0 );
}


//----------------------------------------------------------------------------
//		バックバッファ作成:カラー用レンダーターゲットビュー作成.
//----------------------------------------------------------------------------
HRESULT CDirectX11::CreateColorBackBufferRTV()
{
	// バックバッファテクスチャを取得（既にあるので作成ではない）.
	ID3D11Texture2D* pBackBuffer_Tex = nullptr;
	if( FAILED(
		m_pSwapChain->GetBuffer(
			0,
			__uuidof( ID3D11Texture2D ),	// __uuidof:式に関連付けされたGUIDを取得
											//         Texture2Dの唯一の物として扱う
			(LPVOID*)&pBackBuffer_Tex ) ) )	// (out)バックバッファテクスチャ.
	{
		_ASSERT_EXPR( false, _T( "スワップチェインからバックバッファ取得失敗" ) );
		return E_FAIL;
	}

	// そのテクスチャに対してレンダーターゲットビュー（RTV）を作成.
	if( FAILED(
		m_pDevice11->CreateRenderTargetView(
			pBackBuffer_Tex,
			nullptr,
			&m_pBackBuffer_TexRTV ) ) )	//(out)RTV.
	{
		_ASSERT_EXPR( false, _T( "レンダータイゲットビュー作成失敗" ) );
		return E_FAIL;
	}
	// バックバッファテクスチャを解放.
	SAFE_RELEASE( pBackBuffer_Tex );

	return S_OK;
}


//-----------------------------------------------------------------------------
//		バックバッファ作成:デプスステンシル用レンダーターゲットビュー作成.
//-----------------------------------------------------------------------------
HRESULT CDirectX11::CreateDepthStencilBackBufferRTV()
{
	// デプス（深さor深度）ステンシルビュー用のテクスチャを作成.
	D3D11_TEXTURE2D_DESC	descDepth;
	descDepth.Width		= WND_W;	// 幅.
	descDepth.Height	= WND_H;	// 高さ.
	descDepth.MipLevels	= 1;		// ミップマップレベル：1.
	descDepth.ArraySize	= 1;		// 配列数：1.
	descDepth.SampleDesc.Count	 = 1;	// マルチサンプルの数.
	descDepth.SampleDesc.Quality = 0;	// マルチサンプルのクオリティ.
	descDepth.CPUAccessFlags	 = 0;	// CPUからはアクセスしない.
	descDepth.MiscFlags			 = 0;	// その他の設定なし.
	descDepth.Format	= DXGI_FORMAT_D32_FLOAT;	// 32ビットフォーマット.
	descDepth.Usage		= D3D11_USAGE_DEFAULT;		// 使用方法：デフォルト.
	descDepth.BindFlags	= D3D11_BIND_DEPTH_STENCIL;	// 深度（ステンシルとして使用）.

	if( FAILED(
		m_pDevice11->CreateTexture2D(
			&descDepth,
			nullptr,
			&m_pBackBuffer_DSTex ) ) )	// (out)デプスステンシル用テクスチャ.
	{
		return E_FAIL;
	}

	// そのテクスチャに対してデプスステンシルビュー（DSV）を作成.
	if( FAILED(m_pDevice11->CreateDepthStencilView(
		m_pBackBuffer_DSTex,
		nullptr,
		&m_pBackBuffer_DSTexDSV ) ) )	// (out)DSV.
	{
		_ASSERT_EXPR( false, _T( "デプスステンシルビュー作成失敗." ) );
		return E_FAIL;
	}

	return S_OK;
}
