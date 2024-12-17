#include "CDirectInput.h"

//==============================================================================
//		�L�[�{�[�h�N���X.
//==============================================================================
CKey::CKey()
{
	m_pDIDevKB = NULL;

	ZeroMemory(m_KeyState, sizeof(m_KeyState));
	ZeroMemory(m_KeyAction, sizeof(m_KeyAction));
}

//==============================================================================
//		�L�[�{�[�h�f�o�C�X���쐬.
//==============================================================================
bool CKey::Create(IDirectInput8* pDInput, HWND hWnd)
{
	HRESULT hr;

	if ( !pDInput ){ return false; }

	//�L�[�{�[�h�f�o�C�X���쐬.
	hr = pDInput->CreateDevice( GUID_SysKeyboard, &m_pDIDevKB, NULL );
	if ( FAILED(hr) ){ return false; }
	
	//�f�[�^�t�H�[�}�b�g�̐ݒ�.
	hr = m_pDIDevKB->SetDataFormat( &c_dfDIKeyboard );
	if ( FAILED(hr) ){ return false; }

	//�o�b�t�@�T�C�Y�̐ݒ�.
	DIPROPDWORD diprop = {};
	diprop.diph.dwSize = sizeof( diprop );
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = 1000;

	hr = m_pDIDevKB->SetProperty( DIPROP_BUFFERSIZE, &diprop.diph );
	if ( FAILED(hr) ){ return false; }

	//�������[�h�̐ݒ�.
	hr = m_pDIDevKB->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
	if ( FAILED(hr) ){ return false; }

	//���͂�������.
	m_pDIDevKB->Acquire();
	return true;
}

//==============================================================================
//		�X�V.
//==============================================================================
void CKey::Update()
{
	// m_KeyAction�����Z�b�g.
	ZeroMemory(m_KeyAction, sizeof(m_KeyAction));

	DIDEVICEOBJECTDATA od;
	DWORD dwItems = 1;
	HRESULT hr;
	while (true)
	{
		// �f�o�C�X������̓f�[�^���擾.
		hr = m_pDIDevKB->GetDeviceData( sizeof( DIDEVICEOBJECTDATA ), &od, &dwItems, 0 );

		// ���͂�����ꂽ�ꍇ�A�f�o�C�X���Ď擾.
		if (hr == DIERR_INPUTLOST)
		{
			m_pDIDevKB->Acquire();
		}
		// �擾�ł��Ȃ������A�܂��̓G���[�����������ꍇ.
		else if (dwItems == 0 || FAILED(hr))
		{
			// �܂��f�o�C�X���擾����Ă��Ȃ��ꍇ�A�Ď擾�����݂�.
			if (hr == DIERR_NOTACQUIRED)
			{
				m_pDIDevKB->Acquire();
			}
			break;
		}
		else
		{
			// �L�[�̏�Ԃ��X�V.
			//	0x80�r�b�g���Z�b�g����Ă���΃L�[��������Ă���.
			m_KeyState[od.dwOfs] = ( od.dwData & 0x80 ) ? true : false;

			// �L�[��������Ă���ꍇ�A�A�N�V�����t���O���Z�b�g.
			if (m_KeyState[od.dwOfs])
			{
				m_KeyAction[od.dwOfs] = true;
			}
		}
	}
}

//==============================================================================
//		������.
//==============================================================================
bool CKey::Init(CKey& Key, IDirectInput8* pDInput, HWND hWnd)
{
	//�@�L�[�{�[�h�f�o�C�X�̍쐬.
	if (!Key.Create(pDInput, hWnd)) 
	{
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------
//		�L�[�{�[�h�f�o�C�X�̉��.
//--------------------------------------------------------------------------------
void CKey::Release()
{
	if (m_pDIDevKB)
	{
		m_pDIDevKB->Unacquire();
		m_pDIDevKB->Release();
		m_pDIDevKB = NULL;
	}

	ZeroMemory( m_KeyState,  sizeof( m_KeyState )  );
	ZeroMemory( m_KeyAction, sizeof( m_KeyAction ) );
}