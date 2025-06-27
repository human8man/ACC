#include "CImGui.h"
#include "Psapi.h"
#include "library/imgui_internal.h"
#include "DirectX/DirectX11.h"

// ImGui���ȗ�
using namespace ImGui;

//=============================================================================
//		ImGui�N���X
//=============================================================================
CImGui::CImGui() 
	:m_hwnd			(nullptr)
{
}

CImGui::~CImGui()
{
	// ImGui�̔j��
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();
}


//=============================================================================
//		�쐬����
//=============================================================================
void CImGui::Create(HWND hwnd)
{
	m_hwnd = hwnd;

	// �R���e�L�X�g�̐ݒ�
	IMGUI_CHECKVERSION();
	CreateContext();


	// ���o�͐ݒ�
	ImGuiIO& io = GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// �L�[�{�[�h����L��
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// �Q�[���p�b�h����L��
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// �h�b�L���O�L��
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// �}���`�r���[�|�[�g�L��
	io.DeltaTime = 1.0f / 60.0f;
	io.DisplaySize = ImVec2(1200.0f, 720.0f);

	// �t�H���g�ݒ�
	io.Fonts->AddFontFromFileTTF(
		"Data\\Font\\YOzRSX_.ttf",
		15.0f,
		NULL,
		io.Fonts->GetGlyphRangesJapanese());


	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones
	ImGuiStyle& style = GetStyle();
	io.FontGlobalScale = 1.0f;

	// ImGui�̃X�^�C���ݒ�(StyelColorDark() or StyleColorLight())
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
//		�X�V����
//=============================================================================
void CImGui::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	NewFrame();
}


//=============================================================================
//		�`�揈��
//=============================================================================
void CImGui::Draw()
{
	// ImGui�����f����Ă��邩�̊m�F�p�Ƀe�X�g�E�B���h�E���쐬����
	Begin(IMGUI_JP("ImGui�e�X�g�E�B���h�E"));

	// ��ʃT�C�Y�\��
	Text(IMGUI_JP("��ʃT�C�Y: %.0f * %.0f"),
		GetIO().DisplaySize.x,
		GetIO().DisplaySize.y);

	// �������g�p�ʎ擾
	HANDLE hProc = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (GetProcessMemoryInfo(hProc, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
	{
		Text(IMGUI_JP("�������g�p��: %d MB"), static_cast<int>(pmc.PrivateUsage / (1024 * 1024)));
	}
	CloseHandle(hProc);

	// FPS�\��
	Text("FPS: %.1f", GetIO().Framerate);

	End();


	// ���C���̃����_�����O���������s
	Render();

	// ImGui �̕`��f�[�^�� DirectX �֑��M���ĕ`��
	ImGui_ImplDX11_RenderDrawData(GetDrawData());

	// �}���`�r���[�|�[�g���L���Ȃ�A�ǉ��̃E�B���h�E�������_�����O
	if (GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		// �e�v���b�g�t�H�[���E�B���h�E�̍X�V
		UpdatePlatformWindows();

		// �e�v���b�g�t�H�[���E�B���h�E�̕`�揈��
		RenderPlatformWindowsDefault();
	}
}

//=============================================================================
//		�E�B���h�E�n���h���̐ݒ�
//=============================================================================
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool CImGui::SetWin(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
}