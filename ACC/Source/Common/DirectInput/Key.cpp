#include "DirectInput.h"


Key::Key()
{
	m_pDIDevKB = NULL;

	ZeroMemory(m_KeyState, sizeof(m_KeyState));
	ZeroMemory(m_KeyAction, sizeof(m_KeyAction));
}


//==============================================================================
//		初期化
//==============================================================================
bool Key::Init(Key& Key, IDirectInput8* pDInput, HWND hWnd)
{
	//　キーボードデバイスの作成
	if (!Key.Create(pDInput, hWnd)){ return false; }
	return true;
}


//------------------------------------------------------------------------------
//		作成処理
//------------------------------------------------------------------------------
bool Key::Create(IDirectInput8* pDInput, HWND hWnd)
{
	// 引数チェック（DirectInput が nullptr の場合は失敗）
	if (!pDInput) { return false; }

	HRESULT hr;

	// キーボードデバイスを作成
	hr = pDInput->CreateDevice( GUID_SysKeyboard, &m_pDIDevKB, NULL );
	if ( FAILED(hr) ){ return false; }
	
	// データフォーマットの設定
	hr = m_pDIDevKB->SetDataFormat( &c_dfDIKeyboard );
	if ( FAILED(hr) ){ return false; }

	// バッファサイズの設定
	DIPROPDWORD diprop = {};
	diprop.diph.dwSize = sizeof( diprop );
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = 1000;

	hr = m_pDIDevKB->SetProperty( DIPROP_BUFFERSIZE, &diprop.diph );
	if ( FAILED(hr) ){ return false; }

	// 協調モードの設定
	hr = m_pDIDevKB->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
	if ( FAILED(hr) ){ return false; }

	// 入力を許可する
	m_pDIDevKB->Acquire();

	return true;
}


//------------------------------------------------------------------------------
//		更新処理
//------------------------------------------------------------------------------
void Key::Update()
{
	// キー入力状態のリセット
	ZeroMemory(m_KeyAction, sizeof(m_KeyAction));

	DIDEVICEOBJECTDATA od;
	DWORD dwItems = 1;
	HRESULT hr;
	while (true)
	{
		// デバイスから入力データを取得
		hr = m_pDIDevKB->GetDeviceData( sizeof( DIDEVICEOBJECTDATA ), &od, &dwItems, 0 );

		// 入力が失われた場合、デバイスを再取得
		if (hr == DIERR_INPUTLOST)
		{
			m_pDIDevKB->Acquire();
		}
		// 取得できなかった、またはエラーが発生した場合
		else if (dwItems == 0 || FAILED(hr))
		{
			// まだデバイスが取得されていない場合、再取得を試みる
			if (hr == DIERR_NOTACQUIRED)
			{
				m_pDIDevKB->Acquire();
			}
			break;
		}
		else
		{
			// キーの状態を更新
			//	0x80ビットがセットされていればキーが押されている
			m_KeyState[od.dwOfs] = ( od.dwData & 0x80 ) ? true : false;

			// キーが押されている場合、アクションフラグをセット
			if (m_KeyState[od.dwOfs])
			{
				m_KeyAction[od.dwOfs] = true;
			}
		}
	}
}


//--------------------------------------------------------------------------------
//		解放処理
//--------------------------------------------------------------------------------
void Key::Release()
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