#include "DirectInput.h"


//==============================================================================
//		�_�C���N�g�C���v�b�g�N���X
//==============================================================================
DirectInput::DirectInput()
	: m_hWnd		(NULL)
	, m_pDDirectInput	(NULL)
	, m_UseDevice	(0)
{	
}

DirectInput::~DirectInput()
{
	Release();
}


//==============================================================================
//		�쐬����
//==============================================================================
HRESULT DirectInput::Create(HWND hWnd, int useDevice)
{
	m_hWnd = hWnd;	// �E�B���h�E�n���h����ۑ�

	// ���łɑ��݂���ꍇ�͉��
	if (m_pDDirectInput) { Release(); }

	// DirectInput�I�u�W�F�N�g�̐���
	HRESULT hr = DirectInput8Create(
		::GetModuleHandle(NULL),	// �A�v���P�[�V�����̃��W���[���n���h��
		DIRECTINPUT_VERSION,		// �g�p����DirectInput�̃o�[�W����
		IID_IDirectInput8,			// DirectInput�̃C���^�[�t�F�[�XID
		(void**)&m_pDDirectInput,		// �쐬���ꂽDirectInput�I�u�W�F�N�g��ۑ�����|�C���^
		NULL);						// ���g�p�̈����iNULL���w��j

	// �����Ɏ��s�����ꍇ
	if ( FAILED(hr) ) { return E_FAIL; }

	// �g�p����f�o�C�X�̐ݒ��ۑ�
	m_UseDevice = useDevice;

	if (useDevice & UseInputDevice_KEYBOARD){ m_Key.Create(m_pDDirectInput, hWnd);   }
	if (useDevice & UseInputDevice_GAMEPAD) { m_Mouse.Create(m_pDDirectInput, hWnd); }

	return S_OK;
}


//==============================================================================
//		�������
//==============================================================================
void DirectInput::Release()
{
	// DirectInput�I�u�W�F�N�g�̉��
	if (m_pDDirectInput != NULL) {
		m_pDDirectInput->Release();
		m_pDDirectInput = NULL;
	}

	m_Key.Release();
	m_Mouse.Release();

	m_hWnd = NULL;
	m_UseDevice = 0;
}


//-----------------------------------------------------------------------------
//		���͏�Ԃ̍X�V
//-----------------------------------------------------------------------------
void DirectInput::InputUpdate()
{
	// �L�[�ƃ}�E�X�̍X�V
	m_Key.Update();
	m_Mouse.Update();
}