#include "CDirectInput.h"


//==============================================================================
//		�_�C���N�g�C���v�b�g�N���X.
//==============================================================================
CDInput::CDInput()
{
	m_pDCInput = NULL;
	m_hWnd = NULL;
	m_UseDevice = 0;
}

CDInput::~CDInput()
{
	Release();
}


//==============================================================================
//		�쐬����.
//==============================================================================
HRESULT CDInput::Create(HWND hWnd, int useDevice)
{
	m_hWnd = hWnd;	// �E�B���h�E�n���h����ۑ�.

	// ���łɑ��݂���ꍇ�͉��.
	if (m_pDCInput) { Release(); }

	//DirectInput�I�u�W�F�N�g�̐���.
	HRESULT hr = DirectInput8Create(
		::GetModuleHandle(NULL),	// �A�v���P�[�V�����̃��W���[���n���h��.
		DIRECTINPUT_VERSION,		// �g�p����DirectInput�̃o�[�W����.
		IID_IDirectInput8,			// DirectInput�̃C���^�[�t�F�[�XID.
		(void**)&m_pDCInput,		// �쐬���ꂽDirectInput�I�u�W�F�N�g��ۑ�����|�C���^.
		NULL);						// ���g�p�̈����iNULL���w��j.

	// �����Ɏ��s�����ꍇ.
	if ( FAILED(hr) ) { return E_FAIL; }

	// �g�p����f�o�C�X�̐ݒ��ۑ�.
	m_UseDevice = useDevice;

	if (useDevice & UseInputDevice_KEYBOARD) {
		m_Key.Create(m_pDCInput, hWnd);
	}
	if (useDevice & UseInputDevice_GAMEPAD) {
		m_Mouse.Create(m_pDCInput, hWnd);
	}

	m_GamePad.Create();

	return S_OK;
}


//==============================================================================
//		�쐬����.
//==============================================================================
bool CDInput::GamePadConnect()
{
	// �Q�[���p�b�h���쐬����Ă��Ȃ��Ƃ�.
	if (!m_GamePad.IsValid())
	{
		return false;
	}

	HRESULT hr = m_GamePad.GetDeviceState();

	// ���s�����ꍇ.
	if (FAILED(hr) || hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
	{
		return false;
	}

	return true;
}


//==============================================================================
//		�������.
//==============================================================================
void CDInput::Release()
{
	// DirectInput�I�u�W�F�N�g�̉��.
	if (m_pDCInput != NULL)
	{
		m_pDCInput->Release();
		m_pDCInput = NULL;
	}

	m_Key.Release();
	m_Mouse.Release();
	m_GamePad.Release();

	m_hWnd = NULL;
	m_UseDevice = 0;
}


//-----------------------------------------------------------------------------
//		���͏�Ԃ̍X�V.
//-----------------------------------------------------------------------------
void CDInput::InputUpdate()
{
	if (!GamePadConnect())
	{
		m_Key.Update();
		m_Mouse.Update();
	}
	else
	{
		m_GamePad.Update();
	}
}