#pragma once

// �x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`
#pragma warning(disable:4005)

#include "Singleton/CSingleton.h"


//==================================================
//		DirectX11 �Z�b�g�A�b�v
//==================================================
class CDirectX11
	: public CSingleton<CDirectX11>
{
private:
	friend class CSingleton<CDirectX11>;

public:
	CDirectX11();
	~CDirectX11();

	// �쐬����
	HRESULT Create(HWND hWnd);
	// �������
	void Release();

	// �[�x�i�y�j�e�X�gON/OFF�؂�ւ�
	void SetDepth( bool flag );

	// �A���t�@�u�����hON/OFF�؂�ւ�
	void SetAlphaBlend( bool flag );

	// �o�b�N�o�b�t�@�N���A�֐�
	void ClearBackBuffer();

	// �\������
	void Present();
	// �f�o�C�X���擾
	ID3D11Device* GetDevice()			const { return m_pDevice11; }
	// �X���b�v�`�F�[�����擾
	IDXGISwapChain* GetSwapChain()		const { return m_pSwapChain; }
	// �f�o�C�X�R���e�L�X�g���擾
	ID3D11DeviceContext* GetContext()	const { return m_pContext11; }
	// �����_�[�^�[�Q�b�g�r���[���擾
	ID3D11RenderTargetView* GetTexRTV()	const { return m_pBackBuffer_TexRTV; }
	// �f�o�b�O���[�h�ؑ�
	void SetChangeDebugBuffer(bool flag) { m_IsDebug = flag; }

private:
	// �f�o�C�X�ƃX���b�v�`�F�C���쐬
	HRESULT CreateDeviceAndSwapChain( HWND hWnd, UINT uFPS, UINT uWidth, UINT uHeight );

	// �o�b�N�o�b�t�@�쐬:�J���[�p�����_�[�^�[�Q�b�g�r���[�쐬
	HRESULT CreateColorBackBufferRTV();

	// �o�b�N�o�b�t�@�쐬:�f�v�X�X�e���V���p�����_�[�^�[�Q�b�g�r���[�쐬
	HRESULT CreateDepthStencilBackBufferRTV();

	// ���X�^���C�U�쐬
	HRESULT CreateRasterizer();

	// �f�v�X�X�e���V���X�e�[�g�쐬
	HRESULT CreateDepthStencilState();

	// �A���t�@�u�����h�X�e�[�g�쐬
	HRESULT CreateAlphaBlendState();

private:
	ID3D11Device*				m_pDevice11;			// �f�o�C�X�I�u�W�F�N�g
	ID3D11DeviceContext*		m_pContext11;			// �f�o�C�X�R���e�L�X�g
	IDXGISwapChain*				m_pSwapChain;			// �X���b�v�`�F�[��
	ID3D11RenderTargetView*		m_pBackBuffer_TexRTV;	// �����_�[�^�[�Q�b�g�r���[
	ID3D11Texture2D*			m_pBackBuffer_DSTex;	// �f�v�X�X�e���V���p�e�N�X�`��
	ID3D11DepthStencilView*		m_pBackBuffer_DSTexDSV;	// �f�v�X�X�e���V���r���[

	// �[�x�i�y�j�e�X�g�ݒ�
	ID3D11DepthStencilState*	m_pDepthStencilStateOn;		// �L���ݒ�
	ID3D11DepthStencilState*	m_pDepthStencilStateOff;	// �����ݒ�

	// �A���t�@�u�����h
	ID3D11BlendState*		m_pAlphaBlendOn;	// �L���ݒ�
	ID3D11BlendState*		m_pAlphaBlendOff;	// �����ݒ�

	// �o�b�N�o�b�t�@�̃J���[
	D3DXVECTOR4 m_BufferColor;

	bool m_IsDebug;
};