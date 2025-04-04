#include "CMain.h"
#include "DirectX/CDirectX9.h"
#include "DirectX/CDirectX11.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
#include "Camera/CCamera.h"
#include "Effect/CEffect.h"
#include "Time/CTime.h"


// ImGuiはデバッグ時のみ使用する.
#ifdef _DEBUG
#include "ImGui/CImGui.h"
#endif


#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <dwmapi.h>	//ダークモード用.
#pragma comment(lib, "dwmapi.lib")


const TCHAR WND_TITLE[] = _T( "ACC" );
const TCHAR APP_NAME[]	= _T( "ACC" );


//=================================================
//		メインループクラス.
//=================================================
CMain::CMain()
	: m_hWnd	  ( nullptr )
	, m_ColorStep (0)
{
}

CMain::~CMain()
{
	DeleteObject( m_hWnd );
}


//=================================================
//		構築処理.
//=================================================
HRESULT CMain::Create() const
{
	// DirectX9構築.
	if (FAILED(CDirectX9::GetInstance()		->Create(m_hWnd))) { return E_FAIL; }
	// DirectX11構築.
	if (FAILED(CDirectX11::GetInstance()	->Create(m_hWnd))) { return E_FAIL; }
	// シーンマネージャー構築.
	if (FAILED(CSceneManager::GetInstance()	->Create(m_hWnd))) { return E_FAIL; }
	// DirectInput構築.
	if (FAILED(CInput::GetInstance()		->Create(m_hWnd))) { return E_FAIL; }

	// EffectManagerの構築.
	if (FAILED(CEffect::GetInstance()->Create(
		CDirectX11::GetInstance()->GetDevice(),
		CDirectX11::GetInstance()->GetContext()))) 
	{
		return E_FAIL;
	}

	// EffectManagerのデータ読み込み.
	if (FAILED(CEffect::GetInstance()->LoadData())) { return E_FAIL; }

#ifdef _DEBUG
	// ImGuiの構築.
	CImGui::GetInstance()->Create(m_hWnd);
#endif

	return S_OK;
}


//=================================================
//		更新処理.
//=================================================
void CMain::Update()
{
	// タイムクラスの更新.
	CTime::GetInstance()->Update();

	// CInputの更新処理.
	CInput::GetInstance()->InputUpdate();

	// 更新処理.
	CSceneManager::GetInstance()->Update();
	
	// バックバッファをクリアにする.
	CDirectX11::GetInstance()->ClearBackBuffer();

	// 描画処理.
	CSceneManager::GetInstance()->Draw();

	// 画面に表示.
	CDirectX11::GetInstance()->Present();
}

//=================================================
//		解放処理.
//=================================================
void CMain::Release()
{
	CSoundManager::GetInstance()->Release();
	CSceneManager::GetInstance()->Release();
	CInput::GetInstance()->Release();
}


//=================================================
//		メッセージループ.
//=================================================
void CMain::Loop()
{
	//------------------------------------------------
	//		フレームレート調整準備.
	//------------------------------------------------
	float Rate = 0.0f;	// レート.
	DWORD sync_old = timeGetTime();	// 過去時間.
	DWORD sync_now;					// 現在時間.

	// 時間処理のため、最小単位を1ミリ秒に変更.
	timeBeginPeriod( 1 );
	Rate = 1000.0f / static_cast<float>(FPS); // 理想時間を算出.

	// メッセージループ.
	MSG msg = { 0 };
	ZeroMemory( &msg, sizeof( msg ) );

	// ダークモードに変更.
	setUseImmersiveDarkMode( m_hWnd, true );

	while( msg.message != WM_QUIT )
	{
		sync_now = timeGetTime();	// 現在の時間を取得.

		if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else if( sync_now - sync_old >= Rate )
		{
			sync_old = sync_now;	// 現在時間に置き換え.

			// ウィンドウのボーダーを虹色にする.
			// SetRainbowBorder(m_hWnd);

			// 更新処理.
			Update();
		}
	}
	// アプリケーションの終了.
	Release();
}


//=================================================
//		ウィンドウ初期化関数.
//=================================================
HRESULT CMain::InitWindow(
	HINSTANCE hInstance,	// インスタンス.
	INT x, INT y,			// ウィンドウx,y座標.
	INT width, INT height)	// ウィンドウ幅,高さ.
{
	// ウィンドウの定義.
	WNDCLASSEX wc;
	ZeroMemory( &wc, sizeof( wc ) ); // 初期化(0を設定).

	wc.cbSize			= sizeof( wc );
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= MsgProc;//WndProc;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon( nullptr, IDI_APPLICATION );
	wc.hCursor			= LoadCursor( nullptr, IDC_ARROW );
	wc.hbrBackground	= (HBRUSH)GetStockObject( LTGRAY_BRUSH );
	wc.lpszClassName	= APP_NAME;
	wc.hIconSm			= LoadIcon( nullptr, IDI_APPLICATION );

	// ウィンドウクラスをWindowsに登録.
	if( !RegisterClassEx( &wc ) ) {
		_ASSERT_EXPR( false, _T( "ウィンドウクラスの登録に失敗" ) );
		return E_FAIL;
	}

	//--------------------------------------.
	//		ウィンドウ表示位置の調整.
	//--------------------------------------.
	// この関数内でのみ使用する構造体をここで定義.
	struct RECT_WND
	{
		INT x, y, w, h;
		RECT_WND() : x(), y(), w(), h() {}
	} rectWindow; // ここに変数宣言もする.

#ifdef ENABLE_WINDOWS_CENTERING
	//ディスプレイの幅、高さを取得.
	HWND hDeskWnd = nullptr;
	RECT recDisplay;
	hDeskWnd = GetDesktopWindow();
	GetWindowRect( hDeskWnd, &recDisplay );

	//センタリング.
	rectWindow.x = ( recDisplay.right - width ) / 2;	//表示位置x座標.
	rectWindow.y = ( recDisplay.bottom - height ) / 2;	//表示位置y座標.
#endif//ENABLE_WINDOWS_CENTERING

	//--------------------------------------.
	//		ウィンドウ領域の調整.
	//--------------------------------------.
	RECT	rect = {};		// 矩形構造体.
	DWORD	dwStyle;		// ウィンドウスタイル.
	rect.top = 0;			// 上.
	rect.left = 0;			// 左.
	rect.right = width;		// 右.
	rect.bottom = height;	// 下.
	dwStyle = WS_OVERLAPPEDWINDOW;	// ウィンドウ種別.

	if( AdjustWindowRect(
		&rect,			// (in)画面サイズが入った矩形構造体.(out)計算結果.
		dwStyle,		// ウィンドウスタイル.
		FALSE ) == 0 )	// メニューを持つかどうかの指定.
	{
		MessageBox(
			nullptr,
			_T( "ウィンドウ領域の調整に失敗" ),
			_T( "エラーメッセージ" ),
			MB_OK );
		return 0;
	}

	// ウィンドウの幅高さ調節.
	rectWindow.w = rect.right - rect.left;
	rectWindow.h = rect.bottom - rect.top;

	// 自宅テレワーク時のx,y座標.
	//rectWindow.x = 1920;
	rectWindow.x = 0;
	rectWindow.y = 0;

	//ウィンドウの作成.
	m_hWnd = CreateWindow(
		APP_NAME,					// アプリ名.
		WND_TITLE,					// ウィンドウタイトル.
		dwStyle,					// ウィンドウ種別(普通).
		rectWindow.x, rectWindow.y,	// 表示位置x,y座標.
		rectWindow.w, rectWindow.h,	// ウィンドウ幅,高さ.
		nullptr,					// 親ウィンドウハンドル.
		nullptr,					// メニュー設定.
		hInstance,					// インスタンス番号.
		nullptr );					// ウィンドウ作成時に発生するイベントに渡すデータ.
	if( !m_hWnd ) {
		_ASSERT_EXPR( false, _T( "ウィンドウ作成失敗" ) );
		return E_FAIL;
	}

	//ウィンドウの表示.
	ShowWindow( m_hWnd, SW_SHOW );
	UpdateWindow( m_hWnd );

	return S_OK;
}


//-----------------------------------------------
//		ウィンドウ関数（メッセージ毎の処理）.
//-----------------------------------------------
LRESULT CALLBACK CMain::MsgProc(
	HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam )
{
#ifdef _DEBUG
	if (CImGui::SetWin(hWnd, uMsg, wParam, lParam)) {
		return true;
	}
#endif

	switch( uMsg ) {
	//---------------------------------------------------------------
	//		タイトルバーのカスタマイズ（デフォルトのものを非表示にする）
	//---------------------------------------------------------------
	case WM_CREATE:
		break;
	//---------------------------------------------------------------
	//		ウィンドウが破棄されたとき.
	//---------------------------------------------------------------
	case WM_DESTROY:
		// アプリケーションの終了をWindowsに通知する.
		PostQuitMessage( 0 );
		break;
	//---------------------------------------------------------------
	//		ウィンドウが起動中の場合.
	//---------------------------------------------------------------
	case WM_ACTIVATE:
		// ウィンドウがアクティブの場合.
		if (LOWORD(wParam) == WA_INACTIVE) {
			// マウスの操作を有効.
			CCamera::GetInstance()->SetUseMouse(true);
		}
		// ウィンドウが非アクティブの場合.
		else {
			// マウスの操作を無効.
			CCamera::GetInstance()->SetUseMouse(false);
		}
		break;
	//---------------------------------------------------------------
	//		ウィンドウのDPI(Dot Per Inch)が変更されたとき.
	//---------------------------------------------------------------
	case WM_DPICHANGED:
#ifdef _DEBUG
		// ImGuiの設定フラグでDPIスケーリングが有効か確認.
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			// 新しいDPIの値を取得
			// const int dpi = HIWORD(wParam);

			// DPI変更をログに出力
			// printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);

			// lParamから新しいウィンドウの位置とサイズを示すRECT構造体を取得
			const RECT* suggested_rect = (RECT*)lParam;

			// SetWindowPos関数を使ってウィンドウの位置とサイズを更新
			::SetWindowPos(
				hWnd,											//ウィンドウハンドル.
				nullptr,										//ウィンドウのレイヤーは変更しないためnull.
				suggested_rect->left,							//新規の左座標を設定.
				suggested_rect->top,							//新規の上座標を設定.
				suggested_rect->right - suggested_rect->left,	//新規の幅を設定.
				suggested_rect->bottom - suggested_rect->top,	//新規の高さを設定.
				SWP_NOZORDER | SWP_NOACTIVATE);					//レイヤーを変更せず、ウィンドウをアクティブにしない.
		}
#endif
		break;
	}

	//メインに返す情報.
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


//------------------------------------------------------------------------------
//		ウィンドウをダークモードにする関数.
//------------------------------------------------------------------------------
bool CMain::setUseImmersiveDarkMode(HWND hwnd, bool dark_mode)
{
	// ダークモードにする.
	BOOL value = dark_mode;
	DwmSetWindowAttribute(hwnd, 
		DWMWA_USE_IMMERSIVE_DARK_MODE, 
		&value, 
		sizeof(value));

	// ボタンの位置とサイズ.
	RECT buttonBounds = { 100, 100, 200, 200 };
	DwmSetWindowAttribute(hwnd, 
		DWMWA_CAPTION_BUTTON_BOUNDS, 
		&buttonBounds, 
		sizeof(buttonBounds));

	return false;
}


//------------------------------------------------------------------------------
//		ウィンドウの枠を虹色に変色させる関数.
//------------------------------------------------------------------------------
void CMain::SetRainbowBorder(HWND hwnd)
{
	// sin関数を使用して、赤、緑、青の色成分を計算.
	COLORREF color = RGB(
		static_cast<BYTE>((sin(m_ColorStep * 0.3) * 127) + 128),		// 赤.
		static_cast<BYTE>((sin(m_ColorStep * 0.3 + 2) * 127) + 128),	// 緑.
		static_cast<BYTE>((sin(m_ColorStep * 0.3 + 4) * 127) + 128)		// 青.
	);

	// ウィンドウの枠線の色を変更.
	DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &color, sizeof(color));
	
	// ステップを進めることで色が変化.
	m_ColorStep++;

	// 一定数のステップでリセットして色を最初に戻す.
	if (m_ColorStep > 1000) {
		m_ColorStep = 0;
	}
}
