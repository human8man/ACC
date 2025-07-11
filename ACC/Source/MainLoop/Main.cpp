#include "Main.h"
#include "Time/Time.h"
#include "Camera/Camera.h"
#include "Effect/Effect.h"
#include "DirectX/DirectX9.h"
#include "DirectX/DirectX11.h"
#include "DirectInput/DirectInput.h"
#include "DirectSound/SoundManager.h"
#include "Scenes/SceneManager/SceneManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "ImGui/CImGui.h"


#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <dwmapi.h>	//ダークモード用
#pragma comment(lib, "dwmapi.lib")


const TCHAR WND_TITLE[] = _T( "ACC" );
const TCHAR APP_NAME[]	= _T( "ACC" );


Main::Main()
	: m_hWnd		( nullptr )
	, m_ColorStep	(0)
{
}

Main::~Main()
{
	DeleteObject( m_hWnd );
}


//=================================================
//		構築処理
//=================================================
HRESULT Main::Create() const
{
	// DirectX9構築
	if (FAILED(DirectX9::GetInstance()		->Create(m_hWnd)))	{ return E_FAIL; }
	// DirectX11構築
	if (FAILED(DirectX11::GetInstance()	->Create(m_hWnd)))	{ return E_FAIL; }

	// サウンドデータの読み込み
	if (FAILED(SoundManager::GetInstance()	->Load(m_hWnd)))	{ return E_FAIL; }
	// SpriteManagerの読み込み
	if (FAILED(CSpriteManager::GetInstance()->SpriteLoad()))	{ return E_FAIL; }
	// シーンマネージャー構築
	if (FAILED(SceneManager::GetInstance()	->Create(m_hWnd)))	{ return E_FAIL; }
	// DirectInput構築
	if (FAILED(DirectInput::GetInstance()		->Create(m_hWnd)))	{ return E_FAIL; }

	// EffectManagerの構築
	if (FAILED(Effect::GetInstance()->Create(
		DirectX11::GetInstance()->GetDevice(),
		DirectX11::GetInstance()->GetContext()))) 
	{
		return E_FAIL;
	}

	// EffectManagerのデータ読み込み
	if (FAILED(Effect::GetInstance()->LoadData())) { return E_FAIL; }

	if (ISDEBUG) {
		// ImGuiの構築
		CImGui::GetInstance()->Create(m_hWnd);
	}

	return S_OK;
}


//=================================================
//		更新処理
//=================================================
void Main::Update()
{
	// タイムクラスの更新
	Time::GetInstance()->Update();

	// DirectInputの更新処理
	DirectInput::GetInstance()->InputUpdate();

	// 更新処理
	SceneManager::GetInstance()->Update();
	
	// バックバッファをクリアにする
	DirectX11::GetInstance()->ClearBackBuffer();

	// 描画処理
	SceneManager::GetInstance()->Draw();

	// 画面に表示
	DirectX11::GetInstance()->Present();
}

//=================================================
//		解放処理
//=================================================
void Main::Release()
{
	SoundManager::GetInstance()->Release();
	CSpriteManager::GetInstance()->Release();
	SceneManager::GetInstance()->Release();
	DirectInput::GetInstance()->Release();
}


//=================================================
//		メッセージループ
//=================================================
void Main::Loop()
{
	//------------------------------------------------
	//		フレームレート調整準備
	//------------------------------------------------
	float Rate = 0.0f;	// レート
	DWORD sync_old = timeGetTime();	// 過去時間
	DWORD sync_now;					// 現在時間

	// 時間処理のため、最小単位を1ミリ秒に変更
	timeBeginPeriod( 1 );
	Rate = 1000.0f / static_cast<float>(FPS); // 理想時間を算出

	// メッセージループ
	MSG msg = { 0 };
	ZeroMemory( &msg, sizeof( msg ) );

	// ダークモードに変更
	SetUseImmersiveDarkMode( m_hWnd, true );

	while( msg.message != WM_QUIT )
	{
		sync_now = timeGetTime();	// 現在の時間を取得

		if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else if( sync_now - sync_old >= Rate )
		{
			sync_old = sync_now;	// 現在時間に置き換え

			// ウィンドウのボーダーを虹色にする
			if (RAINBOW_WINDOW) {
				SetRainbowBorder(m_hWnd);
			}
			else { ResetBorder(m_hWnd); }

			// ウィンドウ座標の計算
			WindowPosMath(m_hWnd);

			// 更新処理
			Update();
		}
	}
	// アプリケーションの終了
	Release();
}


//=================================================
//		ウィンドウ初期化関数
//=================================================
HRESULT Main::InitWindow(
	HINSTANCE hInstance,	// インスタンス
	INT x, INT y,			// ウィンドウx,y座標
	INT width, INT height)	// ウィンドウ幅,高さ
{
	// ウィンドウの定義
	WNDCLASSEX wc;
	ZeroMemory( &wc, sizeof( wc ) ); // 初期化(0を設定)
	wc.cbSize			= sizeof( wc );
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= MsgProc;//WndProc;
	wc.hInstance		= hInstance;
	
	HICON hIcon = (HICON)LoadImage(
		nullptr,
		_T("Data\\ACC.ico"),
		IMAGE_ICON,
		0, 0,
		LR_LOADFROMFILE | LR_DEFAULTSIZE);

	wc.hIcon = hIcon;
	wc.hIconSm = hIcon;
	
	wc.hCursor			= LoadCursor( nullptr, IDC_ARROW );
	wc.hbrBackground	= (HBRUSH)GetStockObject( LTGRAY_BRUSH );
	wc.lpszClassName	= APP_NAME;

	// ウィンドウクラスをWindowsに登録
	if( !RegisterClassEx( &wc ) ) {
		_ASSERT_EXPR( false, _T( "ウィンドウクラスの登録に失敗" ) );
		return E_FAIL;
	}

	//--------------------------------------
	//		ウィンドウ表示位置の調整
	//--------------------------------------
	// この関数内でのみ使用する構造体をここで定義
	struct RECT_WND
	{
		INT x, y, w, h;
		RECT_WND() : x(), y(), w(), h() {}
	} rectWindow; // ここに変数宣言もする

#ifdef ENABLE_WINDOWS_CENTERING
	//ディスプレイの幅、高さを取得
	HWND hDeskWnd = nullptr;
	RECT recDisplay;
	hDeskWnd = GetDesktopWindow();
	GetWindowRect( hDeskWnd, &recDisplay );

	//センタリング
	rectWindow.x = ( recDisplay.right - width ) / 2;	//表示位置x座標
	rectWindow.y = ( recDisplay.bottom - height ) / 2;	//表示位置y座標
#endif//ENABLE_WINDOWS_CENTERING

	//--------------------------------------
	//		ウィンドウ領域の調整
	//--------------------------------------
	RECT	rect = {};		// 矩形構造体
	DWORD	dwStyle;		// ウィンドウスタイル
	rect.top = 0;			// 上
	rect.left = 0;			// 左
	rect.right = width;		// 右
	rect.bottom = height;	// 下
	dwStyle = WS_OVERLAPPEDWINDOW;	// ウィンドウ種別

	if( AdjustWindowRect(
		&rect,			// (in)画面サイズが入った矩形構造体.(out)計算結果
		dwStyle,		// ウィンドウスタイル
		FALSE ) == 0 )	// メニューを持つかどうかの指定
	{
		MessageBox(
			nullptr,
			_T( "ウィンドウ領域の調整に失敗" ),
			_T( "エラーメッセージ" ),
			MB_OK );
		return 0;
	}

	// ウィンドウの幅高さ調節
	rectWindow.w = rect.right - rect.left;
	rectWindow.h = rect.bottom - rect.top;

	// 自宅時のx,y座標
	// rectWindow.x = 1920;
	rectWindow.x = 0;
	rectWindow.y = 0;

	//ウィンドウの作成
	m_hWnd = CreateWindow(
		APP_NAME,					// アプリ名
		WND_TITLE,					// ウィンドウタイトル
		dwStyle,					// ウィンドウ種別(普通)
		rectWindow.x, rectWindow.y,	// 表示位置x,y座標
		rectWindow.w, rectWindow.h,	// ウィンドウ幅,高さ
		nullptr,					// 親ウィンドウハンドル
		nullptr,					// メニュー設定
		hInstance,					// インスタンス番号
		nullptr );					// ウィンドウ作成時に発生するイベントに渡すデータ
	if( !m_hWnd ) {
		_ASSERT_EXPR( false, _T( "ウィンドウ作成失敗" ) );
		return E_FAIL;
	}

	//ウィンドウの表示
	ShowWindow( m_hWnd, SW_SHOW );
	UpdateWindow( m_hWnd );

	return S_OK;
}

bool Main::m_WasWindowActive = false;

//-----------------------------------------------
//		ウィンドウ関数（メッセージ毎の処理）
//-----------------------------------------------
LRESULT CALLBACK Main::MsgProc(
	HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
if(ISDEBUG){
	if (CImGui::SetWin(hWnd, uMsg, wParam, lParam)) { return true; }
}

	switch (uMsg)
	{
	//---------------------------------------------------------------
	//		タイトルバーのカスタマイズ（デフォルトのものを非表示にする）
	//---------------------------------------------------------------
	case WM_CREATE:
		break;
	//---------------------------------------------------------------
	//		ウィンドウが閉じようとしたとき
	//---------------------------------------------------------------
	case WM_CLOSE:
		if (UIEDIOR_MOVEANY) {
			if (MessageBox(hWnd,
				L"本当に終了しますか？\n(保存していない場合、編集前にリセットされます)", L"確認",
				MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				DestroyWindow(hWnd);
			}
		}
		else {
			DestroyWindow(hWnd);
		}
		return 0;
	//---------------------------------------------------------------
	//		ウィンドウが破棄されたとき
	//---------------------------------------------------------------
	case WM_DESTROY:
		// アプリケーションの終了をWindowsに通知する
		PostQuitMessage(0);
		break;
	//---------------------------------------------------------------
	//		ウィンドウが起動中の場合
	//---------------------------------------------------------------
	case WM_ACTIVATE:
	{
		bool isActive = (LOWORD(wParam) != WA_INACTIVE);

		if (m_WasWindowActive != isActive) {
			m_WasWindowActive = isActive;
			SceneManager* scene = SceneManager::GetInstance();

			// カーソル状態の取得
			CURSORINFO cursorInfo = {};
			cursorInfo.cbSize = sizeof(CURSORINFO);
			GetCursorInfo(&cursorInfo);

			// アクティブに切り替わった場合
			if (isActive) {
				if (scene->GetSceneNo() == SceneList::eTitle 
				||  scene->GetSceneNo() == SceneList::eUIEditor) {
					Camera::GetInstance()->SetUseMouse(true);
					if (cursorInfo.flags != CURSOR_SHOWING) { ShowCursor(true); }
				}
				else {
					Camera::GetInstance()->SetUseMouse(false);
					if (cursorInfo.flags == CURSOR_SHOWING) { ShowCursor(false); }
				}
			}
			else {
				Camera::GetInstance()->SetUseMouse(true);
				if (cursorInfo.flags != CURSOR_SHOWING) { ShowCursor(true); }
			}
		}
		break;
	}
	//---------------------------------------------------------------
	//		ウィンドウのDPI(Dot Per Inch)が変更されたとき
	//---------------------------------------------------------------
	case WM_DPICHANGED:
		if (ISDEBUG) {
			// ImGuiの設定フラグでDPIスケーリングが有効か確認
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
					hWnd,											//ウィンドウハンドル
					nullptr,										//ウィンドウのレイヤーは変更しないためnull
					suggested_rect->left,							//新規の左座標を設定
					suggested_rect->top,							//新規の上座標を設定
					suggested_rect->right - suggested_rect->left,	//新規の幅を設定
					suggested_rect->bottom - suggested_rect->top,	//新規の高さを設定
					SWP_NOZORDER | SWP_NOACTIVATE);					//レイヤーを変更せず、ウィンドウをアクティブにしない
			}
		}
		break;
	}

	//メインに返す情報
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//------------------------------------------------------------------------------
//		ウィンドウをダークモードにする関数
//------------------------------------------------------------------------------
void Main::WindowPosMath(HWND hwnd)
{
	// ウィンドウ全体の位置とサイズを取得（ウィンドウタブや枠を含む）
	RECT WindowRect;
	GetWindowRect(hwnd, &WindowRect);

	// クライアント領域の位置とサイズを取得（ウィンドウ内の描画範囲）
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	// クライアント領域の左上と右下の座標を初期化
	POINT topLeft		= { clientRect.left, clientRect.top };
	POINT bottomRight	= { clientRect.right, clientRect.bottom };

	// クライアント領域の座標をスクリーン座標系に変換
	ClientToScreen(hwnd, &topLeft);
	ClientToScreen(hwnd, &bottomRight);

	// ウィンドウ全体の左上座標とクライアント領域の左上座標の差分を計算
	int borderLeft	= topLeft.x - WindowRect.left;
	int borderTop	= topLeft.y - WindowRect.top;
	
	WINDOWRECT = D3DXVECTOR2(
		static_cast<float>(WindowRect.left),
		static_cast<float>(WindowRect.right));
	CLIENTRECT = D3DXVECTOR2(
		static_cast<float>(borderLeft + WindowRect.left),
		static_cast<float>(borderTop + WindowRect.top));
}


//------------------------------------------------------------------------------
//		ウィンドウをダークモードにする関数
//------------------------------------------------------------------------------
bool Main::SetUseImmersiveDarkMode(HWND hwnd, bool dark_mode)
{
	// ダークモードにする
	BOOL value = dark_mode;
	DwmSetWindowAttribute(hwnd, 
		DWMWA_USE_IMMERSIVE_DARK_MODE, 
		&value, 
		sizeof(value));

	// ボタンの位置とサイズ
	RECT buttonBounds = { 100, 100, 200, 200 };
	DwmSetWindowAttribute(hwnd, 
		DWMWA_CAPTION_BUTTON_BOUNDS, 
		&buttonBounds, 
		sizeof(buttonBounds));

	return false;
}


//------------------------------------------------------------------------------
//		ウィンドウの枠を虹色に変色させる関数
//------------------------------------------------------------------------------
void Main::SetRainbowBorder(HWND hwnd)
{
	// sin関数を使用して、赤、緑、青の色成分を計算
	COLORREF color = RGB(
		static_cast<BYTE>((sin(m_ColorStep * 0.3) * 127) + 128),		// 赤
		static_cast<BYTE>((sin(m_ColorStep * 0.3 + 2) * 127) + 128),	// 緑
		static_cast<BYTE>((sin(m_ColorStep * 0.3 + 4) * 127) + 128)		// 青
	);

	// ウィンドウの枠線の色を変更
	DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &color, sizeof(color));
	
	// ステップを進めることで色が変化
	m_ColorStep++;

	// 一定数のステップでリセットして色を最初に戻す
	if (m_ColorStep > 1000) {
		m_ColorStep = 0;
	}
}


//------------------------------------------------------------------------------
//		ウィンドウの枠リセット関数
//------------------------------------------------------------------------------
void Main::ResetBorder(HWND hwnd)
{
	// sin関数を使用して、赤、緑、青の色成分を計算
	COLORREF color = RGB(0,0,0);

	// ウィンドウの枠線の色を変更
	DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &color, sizeof(color));
}