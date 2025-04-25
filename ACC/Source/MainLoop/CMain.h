#pragma once
#include <Windows.h>


//==========================================
//		メインクラス.
//==========================================
class CMain
{
public:
	CMain();
	~CMain();

	
	HRESULT Create() const;	// 作成処理.
	void Update();			// 更新処理.
	void Release();			// 解放処理.
	void Loop();			// メインループ.

	// ウィンドウ初期化関数.
	HRESULT InitWindow(
		HINSTANCE hInstance,
		INT x,	   INT y,
		INT width, INT height );

private:
	// ウィンドウ関数（メッセージ毎の処理）.
	static LRESULT CALLBACK MsgProc(
		HWND hWnd,	   UINT uMsg,
		WPARAM wParam, LPARAM lParam );

	// ウィンドウの位置を計算.
	void WindowPosMath(HWND hwnd);
	// ウィンドウをダークモードにする関数.
	bool setUseImmersiveDarkMode(HWND hwnd, bool dark_mode);
	// ウィンドウの枠を虹色に変色させる関数.
	void SetRainbowBorder(HWND hwnd);

private:
	HWND	m_hWnd;			// ウィンドウハンドル.

	int		m_ColorStep;	// ウィンドウ枠を虹色にする関数用の変数.
	static bool	m_WasWindowActive;	// 前フレームのウィンドウのアクティブ状態.
};