#pragma once
#include "DirectX/CDirectX9.h"
#include "DirectX/CDirectX11.h"

//===================================================
//		�V�[���x�[�X�N���X.
//===================================================
class CSceneBase
{
public:
	CSceneBase();
	~CSceneBase();
	
	virtual void	Create	()	= 0; // �\�z.
	virtual HRESULT LoadData()	= 0; // �f�[�^�Ǎ�.
	virtual void	Init	()	= 0; // ������.	
	virtual void	Update	()	= 0; // �X�V.
	virtual void	Draw	()	= 0; // �`��.
	virtual void	Release	()	= 0; // �j��.

	// �v���W�F�N�V�����֐�.
	void Projection( D3DXMATRIX& Proj );

protected:
	HWND		m_hWnd;		// �E�B���h�E�n���h��.
	CAMERA		m_Camera;	// �J�������.
	LIGHT		m_Light;	// ���C�g���.
	D3DXMATRIX	m_mView;	// �r���[(�J����)�s��.
	D3DXMATRIX	m_mProj;	// �ˉe�i�v���W�F�N�V�����j�s��.
};