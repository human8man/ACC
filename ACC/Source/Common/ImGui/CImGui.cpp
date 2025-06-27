#include "CImGui.h"
#include "Psapi.h"
#include "library/imgui_internal.h"
#include "DirectX/DirectX11.h"

// ImGuiを省略
using namespace ImGui;

//=============================================================================
//		ImGuiクラス
//=============================================================================
CImGui::CImGui() 
	:m_hwnd			(nullptr)
{
}

CImGui::~CImGui()
{
	// ImGuiの破棄
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();
}


//=============================================================================
//		作成処理
//=============================================================================
void CImGui::Create(HWND hwnd)
{
	m_hwnd = hwnd;

	// コンテキストの設定
	IMGUI_CHECKVERSION();
	CreateContext();


	// 入出力設定
	ImGuiIO& io = GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// キーボード操作有効
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// ゲームパッド操作有効
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// ドッキング有効
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// マルチビューポート有効
	io.DeltaTime = 1.0f / 60.0f;
	io.DisplaySize = ImVec2(1200.0f, 720.0f);

	// フォント設定
	io.Fonts->AddFontFromFileTTF(
		"Data\\Font\\YOzRSX_.ttf",
		15.0f,
		NULL,
		io.Fonts->GetGlyphRangesJapanese());


	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones
	ImGuiStyle& style = GetStyle();
	io.FontGlobalScale = 1.0f;

	// ImGuiのスタイル設定(StyelColorDark() or StyleColorLight())
	StyleColorsDark();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	IsWindowFocused();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(DirectX11::GetInstance()->GetDevice(), DirectX11::GetInstance()->GetContext());

}


//=============================================================================
//		更新処理
//=============================================================================
void CImGui::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	NewFrame();
}


//=============================================================================
//		描画処理
//=============================================================================
void CImGui::Draw()
{
	// ImGuiが反映されているかの確認用にテストウィンドウを作成する
	Begin(IMGUI_JP("ImGuiテストウィンドウ"));

	// 画面サイズ表示
	Text(IMGUI_JP("画面サイズ: %.0f * %.0f"),
		GetIO().DisplaySize.x,
		GetIO().DisplaySize.y);

	// メモリ使用量取得
	HANDLE hProc = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (GetProcessMemoryInfo(hProc, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
	{
		Text(IMGUI_JP("メモリ使用量: %d MB"), static_cast<int>(pmc.PrivateUsage / (1024 * 1024)));
	}
	CloseHandle(hProc);

	// FPS表示
	Text("FPS: %.1f", GetIO().Framerate);

	End();


	// メインのレンダリング処理を実行
	Render();

	// ImGui の描画データを DirectX へ送信して描画
	ImGui_ImplDX11_RenderDrawData(GetDrawData());

	// マルチビューポートが有効なら、追加のウィンドウもレンダリング
	if (GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		// 各プラットフォームウィンドウの更新
		UpdatePlatformWindows();

		// 各プラットフォームウィンドウの描画処理
		RenderPlatformWindowsDefault();
	}
}

//=============================================================================
//		ウィンドウハンドルの設定
//=============================================================================
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool CImGui::SetWin(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
}