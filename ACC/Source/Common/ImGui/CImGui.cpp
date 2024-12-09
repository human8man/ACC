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
	//ImGui�̔j��.
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CImGui::Create(HWND hwnd)
{
	m_hwnd = hwnd;

	// �R���e�L�X�g�̐ݒ�.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// ImGui�Ɋւ���ݒ�.
	// �R�[�h�������Ȃ邽��io��ImGui��IO(����\�o�̗͂�)�̎Q�Ƃ�n��.
	ImGuiIO& io = ImGui::GetIO();(void)io;	

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// �L�[�{�[�h�����L��.
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// �Q�[���p�b�h�ł̑����L��.
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// �E�B���h�E�̃h�b�L���O��L��.
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// �}���`�r���[�|�[�g��L��.
	io.DeltaTime = 1.0f / 60.0f;	// �t���[�����Ԃ̐ݒ�.
	io.DisplaySize.x = 1200.0f;		// ���̐ݒ�.
	io.DisplaySize.y = 720.0f;		// �����̐ݒ�.
	ImFont* font = io.Fonts->AddFontFromFileTTF(
		"Data\\Font\\YOzRSX_.ttf",
		15.0f,
		NULL,
		io.Fonts->GetGlyphRangesJapanese());

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	io.FontGlobalScale = 1.0f;

	// ImGui�̃X�^�C���ݒ�(StyelColorDark() or StyleColorLight()).
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


	// �t�H���g�̓ǂݍ��݂��ł��邯�ǂ��������e�L�X�g�n�̃t�@�C��������炵��.
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);
}

// �X�V����.
void CImGui::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

// �`�揈��.
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

// �`�揈��.
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

// �E�B���h�E�n���h���̐ݒ�.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// �E�B���h�E�n���h���̐ݒ�.
bool CImGui::SetWin(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
}