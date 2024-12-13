#include "CDirectInput.h"

CKey::CKey()
{
	m_pDIDevKB = NULL;

	ZeroMemory(m_KeyState, sizeof(m_KeyState));
	ZeroMemory(m_KeyAction, sizeof(m_KeyAction));
}

//�L�[�{�[�h�f�o�C�X���쐬.
bool CKey::Create(IDirectInput8* pDInput, HWND hWnd)
{
	HRESULT hr;

	if (!pDInput)
	{
		return false;
	}

	//�L�[�{�[�h�f�o�C�X���쐬.
	hr = pDInput->CreateDevice( GUID_SysKeyboard, &m_pDIDevKB, NULL );
	if (FAILED(hr))
	{
		return false;
	}
	
	//�f�[�^�t�H�[�}�b�g�̐ݒ�.
	hr = m_pDIDevKB->SetDataFormat( &c_dfDIKeyboard );
	if (FAILED(hr))
	{
		return false;
	}

	//�o�b�t�@�T�C�Y�̐ݒ�.
	DIPROPDWORD diprop = {};
	diprop.diph.dwSize = sizeof( diprop );
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = 1000;
	hr = m_pDIDevKB->SetProperty( DIPROP_BUFFERSIZE, &diprop.diph );
	if (FAILED(hr))
	{
		return false;
	}

	//�������[�h�̐ݒ�.
	hr = m_pDIDevKB->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
	if (FAILED(hr))
	{
		return false;
	}

	//���͂�������.
	m_pDIDevKB->Acquire();
	return true;
}

void CKey::Update()
{
	ZeroMemory(m_KeyAction, sizeof(m_KeyAction));

	DIDEVICEOBJECTDATA od;
	DWORD dwItems = 1;
	HRESULT hr;
	while (true)
	{
		hr = m_pDIDevKB->GetDeviceData( sizeof( DIDEVICEOBJECTDATA ), &od, &dwItems, 0 );

		if (hr == DIERR_INPUTLOST)
		{
			m_pDIDevKB->Acquire();
		}
		else if (dwItems == 0 || FAILED(hr))
		{
			if (hr == DIERR_NOTACQUIRED)
			{
				m_pDIDevKB->Acquire();
			}
			break;
		}
		else
		{
			m_KeyState[od.dwOfs] = ( od.dwData & 0x80 ) ? true : false;

			if (m_KeyState[od.dwOfs])
			{
				m_KeyAction[od.dwOfs] = true;
			}
		}
	}
}

bool CKey::Init(CKey& Key, IDirectInput8* pDInput, HWND hWnd)
{
	//CKey�N���X��Create���\�b�h���Ăяo���ăL�[�{�[�h�f�o�C�X�̏�����.
	if (!Key.Create(pDInput, hWnd))
	{
		return false;   //�L�[�{�[�h�f�o�C�X�̏������Ɏ��s�����ꍇ.
	}

	return true;    //����������.

}

//�L�[�{�[�h�f�o�C�X�̉��.
void CKey::Release()
{
	if (m_pDIDevKB)
	{
		m_pDIDevKB->Unacquire();
		m_pDIDevKB->Release();
		m_pDIDevKB = NULL;
	}

	ZeroMemory( m_KeyState, sizeof( m_KeyState ) );
	ZeroMemory( m_KeyAction, sizeof( m_KeyAction ) );
}