#pragma once

#include "DirectX/DirectX9.h"
#include "DirectX/DirectX11.h"


class SceneBase
{
public:
	SceneBase();
	~SceneBase();
	
	virtual void	Create	()	= 0; // �쐬����
	virtual HRESULT LoadData()	= 0; // �Ǎ�����
	virtual void	Init	()	= 0; // ����������
	virtual void	Update	()	= 0; // �X�V����
	virtual void	Draw	()	= 0; // �`�揈��
	virtual void	Release	()	= 0; // �������

	// �v���W�F�N�V�����֐�
	void Projection( D3DXMATRIX& Proj );

protected:
	HWND		m_hWnd;	 // �E�B���h�E�n���h��
	LIGHT		m_Light; // ���C�g���
	D3DXMATRIX	m_mView; // �r���[(�J����)�s��
	D3DXMATRIX	m_mProj; // �ˉe�i�v���W�F�N�V�����j�s��
};