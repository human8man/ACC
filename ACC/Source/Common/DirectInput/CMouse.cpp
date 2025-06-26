#include "CDirectInput.h"
#include "Global.h"


//--------------------------------------------------------------------------------
//		�}�E�X�N���X
//--------------------------------------------------------------------------------
CMouse::CMouse()
{
	m_hWnd		= NULL;
	m_pDIMouse	= NULL;
	m_LDown		= false;
	m_RDown		= false;
	m_MDown		= false;
	m_LAction	= false;
	m_RAction	= false;
	m_MAction	= false;
}


//------------------------------------------------------------------------------
//		�쐬����
//------------------------------------------------------------------------------
bool CMouse::Create(IDirectInput8* pDInput, HWND hWnd)
{
	HRESULT hr;

	if ( !pDInput ){ return false; }
	
	m_hWnd = hWnd;

	// �}�E�X�f�o�C�X���쐬
	hr = pDInput->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL);
	if ( FAILED(hr) ){ return false; }

	// �f�[�^�t�H�[�}�b�g�̐ݒ�
	hr = m_pDIMouse->SetDataFormat(&c_dfDIMouse2);
	if ( FAILED(hr) ){ return false; }

	// �o�b�t�@�T�C�Y�̐ݒ�
	DIPROPDWORD diprop			= {};
	diprop.diph.dwSize			= sizeof(diprop);		// �\���̃T�C�Y
	diprop.diph.dwHeaderSize	= sizeof(diprop.diph);	// �w�b�_�T�C�Y
	diprop.diph.dwObj			= 0;					// �I�u�W�F�N�gID
	diprop.diph.dwHow			= DIPH_DEVICE;			// �f�o�C�X�ɐݒ�
	diprop.dwData				= 1000;					// �o�b�t�@�T�C�Y

	hr = m_pDIMouse->SetProperty(DIPROP_BUFFERSIZE, &diprop.diph);
	if ( FAILED(hr) ){ return false; }

	// �������[�h�̐ݒ� (��r�����[�h�Ńt�H�A�O���E���h�œ��삷��悤�ɐݒ�)
	hr = m_pDIMouse->SetCooperativeLevel(m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)) { return false; }

	// ���͂�������
	m_pDIMouse->Acquire();

	return true;
}


//------------------------------------------------------------------------------
//		�X�V����
//------------------------------------------------------------------------------
void CMouse::Update()
{
	// ���Z�b�g����
	m_LAction = m_RAction = m_MAction = false;

	DIDEVICEOBJECTDATA od;	// �}�E�X���̓f�[�^���i�[����\����
	DWORD dwItems = 1;		// �擾������̓f�[�^�̐�
	HRESULT hr;				// �֐��̖߂�l
	while (true)
	{
		// �}�E�X�̓��̓f�[�^���擾
		hr = m_pDIMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwItems, 0);
		
		// ���͂������Ă����ꍇ�A�f�o�C�X���Ď擾
		if (hr == DIERR_INPUTLOST)
		{
			m_pDIMouse->Acquire();
		}
		// �擾����f�[�^���Ȃ��A�܂��̓G���[�����������ꍇ
		else if (dwItems == 0 || FAILED(hr))
		{
			// �f�o�C�X�����擾�Ȃ�Ď擾�����݂�
			if (hr == DIERR_NOTACQUIRED)
			{
				m_pDIMouse->Acquire();
			}
			break;
		}
		else
		{
			switch (od.dwOfs)
			{
			case DIMOFS_BUTTON0: m_LAction = m_LDown = (od.dwData & 0x80) != 0; break; // ���N���b�N
			case DIMOFS_BUTTON1: m_RAction = m_RDown = (od.dwData & 0x80) != 0; break; // �E�N���b�N
			case DIMOFS_BUTTON2: m_MAction = m_MDown = (od.dwData & 0x80) != 0; break; // �z�C�[��
			}
			od.dwData = static_cast<DWORD>(0.0f);
		}
	}
}


//------------------------------------------------------------------------------
//		�������
//------------------------------------------------------------------------------
void CMouse::Release()
{
	if (m_pDIMouse != NULL) {
		m_pDIMouse->Unacquire();
		m_pDIMouse->Release();
		m_pDIMouse = NULL;
	}

	m_hWnd		= NULL;
	m_LDown		= false;
	m_RDown		= false;
	m_LAction	= false;
	m_RAction	= false;
}