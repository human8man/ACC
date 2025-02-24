#pragma once
#include "Common/Singleton/CSingleton.h"
#include "Common/ImGui/library/imgui.h"
#include "Common/ImGui/library/imgui_impl_win32.h"
#include "Common/ImGui/library/imgui_impl_dx11.h"

// static�ōX�V��̃T�C�Y�ϐ���錾.
static UINT g_ResizeWidth, g_ResizeHeight;


//=============================================================================
//		ImGui�N���X.
//=============================================================================
class CImGui
	: public CSingleton<CImGui>
{
private:
	friend class CSingleton<CImGui>; //�V���O���g���N���X���t�����h�錾.
	CImGui();
	~CImGui();

public:
	// �쐬����.
	void Create(HWND hwnd);
	
	// �X�V����.
	void Update();

	// �`�揈��
	void Draw();

	//�E�B���h�E�n���h����ImGui�ɓn��.
	static bool SetWin(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HWND	m_hwnd;	//�E�B���h�E�n���h��.
};