#pragma once
#include <Windows.h>


//==========================================
//		���C���N���X.
//==========================================
class CMain
{
public:
	CMain();
	~CMain();

	
	HRESULT Create() const;	// �쐬����.
	void Update();			// �X�V����.
	void Release();			// �������.
	void Loop();			// ���C�����[�v.

	// �E�B���h�E�������֐�.
	HRESULT InitWindow(
		HINSTANCE hInstance,
		INT x,	   INT y,
		INT width, INT height );

private:
	// �E�B���h�E�֐��i���b�Z�[�W���̏����j.
	static LRESULT CALLBACK MsgProc(
		HWND hWnd,	   UINT uMsg,
		WPARAM wParam, LPARAM lParam );

	// �E�B���h�E�̈ʒu���v�Z.
	void WindowPosMath(HWND hwnd);
	// �E�B���h�E���_�[�N���[�h�ɂ���֐�.
	bool setUseImmersiveDarkMode(HWND hwnd, bool dark_mode);
	// �E�B���h�E�̘g����F�ɕϐF������֐�.
	void SetRainbowBorder(HWND hwnd);

private:
	HWND	m_hWnd;			// �E�B���h�E�n���h��.

	int		m_ColorStep;	// �E�B���h�E�g����F�ɂ���֐��p�̕ϐ�.
	static bool	m_WasWindowActive;	// �O�t���[���̃E�B���h�E�̃A�N�e�B�u���.
};