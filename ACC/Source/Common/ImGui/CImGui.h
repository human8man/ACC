#pragma once
#include "Common/Singleton/CSingleton.h"
#include "Common/ImGui/library/imgui.h"
#include "Common/ImGui/library/imgui_impl_win32.h"
#include "Common/ImGui/library/imgui_impl_dx11.h"

// staticで更新後のサイズ変数を宣言.
static UINT g_ResizeWidth, g_ResizeHeight;


//=============================================================================
//		ImGuiクラス.
//=============================================================================
class CImGui
	: public CSingleton<CImGui>
{
private:
	friend class CSingleton<CImGui>; //シングルトンクラスをフレンド宣言.
	CImGui();
	~CImGui();

public:
	// 作成処理.
	void Create(HWND hwnd);
	
	// 更新処理.
	void Update();

	// 描画処理
	void Draw();

	//ウィンドウハンドルをImGuiに渡す.
	static bool SetWin(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HWND	m_hwnd;	//ウィンドウハンドル.
};