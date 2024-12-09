#include "CImGui.h"
#include "Psapi.h"
#include "library/imgui_internal.h"
#include "DirectX/CDirectX11.h"


CImGui::CImGui() 
	:m_hwnd			(nullptr)
{
}

CImGui::~CImGui()
{
	//ImGuiの破棄.
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CImGui::Create(HWND hwnd)
{
	m_hwnd = hwnd;

	// コンテキストの設定.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// ImGuiに関する設定.
	// コードが長くなるためioにImGuiのIO(入力\出力の略)の参照を渡す.
	ImGuiIO& io = ImGui::GetIO();(void)io;	

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// キーボード操作を有効.
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// ゲームパッドでの操作を有効.
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// ウィンドウのドッキングを有効.
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// マルチビューポートを有効.
	io.DeltaTime = 1.0f / 60.0f;	// フレーム時間の設定.
	io.DisplaySize.x = 1200.0f;		// 幅の設定.
	io.DisplaySize.y = 720.0f;		// 高さの設定.
	ImFont* font = io.Fonts->AddFontFromFileTTF(
		"Data\\Font\\YOzRSX_.ttf",
		15.0f,
		NULL,
		io.Fonts->GetGlyphRangesJapanese());

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	io.FontGlobalScale = 1.0f;

	// ImGuiのスタイル設定(StyelColorDark() or StyleColorLight()).
	ImGui::StyleColorsDark();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui::IsWindowFocused();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(CDirectX11::GetInstance()->GetDevice(), CDirectX11::GetInstance()->GetContext());


	// フォントの読み込みができるけどそういうテキスト系のファイルがいるらしい.
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);
}

// 更新処理.
void CImGui::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

// 描画処理.
void CImGui::Render()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Render();
	ID3D11RenderTargetView* TargetView = CDirectX11::GetInstance()->GetTexRTV();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags && ImGuiConfigFlags_ViewportsEnable) 
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

// 描画処理.
void CImGui::Draw()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Begin("ImGui");

	ImGui::Text(("DiplaySize%.0f * %.0f"), io.DisplaySize.x, io.DisplaySize.y);
	ImGui::Button("test");

	HANDLE hProc = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	BOOL isSuccess = GetProcessMemoryInfo(
		hProc,
		(PROCESS_MEMORY_COUNTERS*)&pmc,
		sizeof(pmc));
	CloseHandle(hProc);

	ImGui::Text("used memory: %d MB", pmc.PrivateUsage / (1024 * 1024));

	ImGui::Text("Fps: %f", io.Framerate);

	ImGui::End();
	Render();
}

// ウィンドウハンドルの設定.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ウィンドウハンドルの設定.
bool CImGui::SetWin(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
}