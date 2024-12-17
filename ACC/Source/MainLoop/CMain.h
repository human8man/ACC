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

	HRESULT Create() const;
	void Update();
	void Release();
	void Loop();

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

private:
	HWND	m_hWnd;	// �E�B���h�E�n���h��.
};