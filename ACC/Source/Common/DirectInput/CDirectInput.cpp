#include "CDirectInput.h"


//==============================================================================
//		�_�C���N�g�C���v�b�g�N���X.
//==============================================================================
CInput::CInput()
	: m_hWnd		(NULL)
	, m_pDCInput	(NULL)
	, m_UseDevice	(0)
	, m_GamePad		(NULL)
{	
}

CInput::~CInput()
{
	Release();
}


//==============================================================================
//		�쐬����.
//==============================================================================
HRESULT CInput::Create(HWND hWnd, int useDevice)
{
	m_hWnd = hWnd;	// �E�B���h�E�n���h����ۑ�.

	// ���łɑ��݂���ꍇ�͉��.
	if (m_pDCInput) { Release(); }

	// DirectInput�I�u�W�F�N�g�̐���.
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

	if (useDevice & UseInputDevice_KEYBOARD){ m_Key.Create(m_pDCInput, hWnd);   }
	if (useDevice & UseInputDevice_GAMEPAD) { m_Mouse.Create(m_pDCInput, hWnd); }

	return S_OK;
}


//==============================================================================
//		�������.
//==============================================================================
void CInput::Release()
{
	// DirectInput�I�u�W�F�N�g�̉��.
	if (m_pDCInput != NULL) {
		m_pDCInput->Release();
		m_pDCInput = NULL;
	}

	m_Key.Release();
	m_Mouse.Release();
	SAFE_DELETE(m_GamePad);

	m_hWnd = NULL;
	m_UseDevice = 0;
}


//-----------------------------------------------------------------------------
//		���͏�Ԃ̍X�V.
//-----------------------------------------------------------------------------
void CInput::InputUpdate()
{
	XINPUT_STATE state;
	bool isConnected = (XInputGetState(0, &state) == ERROR_SUCCESS);

	// �R���g���[���[���ڑ�����Ă���ꍇ.
	if (isConnected)
	{
		if (!m_GamePad) {
			m_GamePad = new CXInput(0);
		}
	}
	else
	{
		if (m_GamePad) {
			delete m_GamePad;
			m_GamePad = nullptr;
		}
	}
	// �R���g���[���[���ڑ�����Ă���ꍇ�X�V.
	if (m_GamePad) {
		m_GamePad->Update();
	}

	// �L�[�ƃ}�E�X�̍X�V.
	m_Key.Update();
	m_Mouse.Update();
}