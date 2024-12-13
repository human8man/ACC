#include "CDirectInput.h"

CKey::CKey()
{
	m_pDIDevKB = NULL;

	ZeroMemory(m_KeyState, sizeof(m_KeyState));
	ZeroMemory(m_KeyAction, sizeof(m_KeyAction));
}

//キーボードデバイスを作成.
bool CKey::Create(IDirectInput8* pDInput, HWND hWnd)
{
	HRESULT hr;

	if (!pDInput)
	{
		return false;
	}

	//キーボードデバイスを作成.
	hr = pDInput->CreateDevice( GUID_SysKeyboard, &m_pDIDevKB, NULL );
	if (FAILED(hr))
	{
		return false;
	}
	
	//データフォーマットの設定.
	hr = m_pDIDevKB->SetDataFormat( &c_dfDIKeyboard );
	if (FAILED(hr))
	{
		return false;
	}

	//バッファサイズの設定.
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

	//協調モードの設定.
	hr = m_pDIDevKB->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
	if (FAILED(hr))
	{
		return false;
	}

	//入力を許可する.
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
	//CKeyクラスのCreateメソッドを呼び出してキーボードデバイスの初期化.
	if (!Key.Create(pDInput, hWnd))
	{
		return false;   //キーボードデバイスの初期化に失敗した場合.
	}

	return true;    //初期化成功.

}

//キーボードデバイスの解放.
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