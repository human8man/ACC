#pragma once

// �x���ɂ��ẴR�[�h���͂𖳌��ɂ���
#pragma warning(disable:4005)

#include <d3dx9.h>
#include "Singleton/Singleton.h"


class DirectX9
	: public Singleton<DirectX9>
{
private:
	friend class Singleton<DirectX9>;

public:
	DirectX9();
	~DirectX9();

	// �쐬����
	HRESULT Create( HWND hWnd );
	// �������
	void Release();

	// �f�o�C�X�I�u�W�F�N�g���擾
	LPDIRECT3DDEVICE9 GetDevice() const { return m_pDevice9; }

private:
	LPDIRECT3DDEVICE9	m_pDevice9;	// �f�o�C�X�I�u�W�F�N�g
};