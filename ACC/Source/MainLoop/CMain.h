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

	HRESULT Create() const;
	void Update();
	void Release();
	void Loop();

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

	bool setUseImmersiveDarkMode(HWND hwnd, bool dark_mode);
	void SetRainbowBorder(HWND hwnd);

private:
	HWND	m_hWnd;	// ウィンドウハンドル.

	int		m_ColorStep;
};