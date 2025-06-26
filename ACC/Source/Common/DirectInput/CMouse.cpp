#include "CDirectInput.h"
#include "Global.h"


//--------------------------------------------------------------------------------
//		マウスクラス
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
//		作成処理
//------------------------------------------------------------------------------
bool CMouse::Create(IDirectInput8* pDInput, HWND hWnd)
{
	HRESULT hr;

	if ( !pDInput ){ return false; }
	
	m_hWnd = hWnd;

	// マウスデバイスを作成
	hr = pDInput->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL);
	if ( FAILED(hr) ){ return false; }

	// データフォーマットの設定
	hr = m_pDIMouse->SetDataFormat(&c_dfDIMouse2);
	if ( FAILED(hr) ){ return false; }

	// バッファサイズの設定
	DIPROPDWORD diprop			= {};
	diprop.diph.dwSize			= sizeof(diprop);		// 構造体サイズ
	diprop.diph.dwHeaderSize	= sizeof(diprop.diph);	// ヘッダサイズ
	diprop.diph.dwObj			= 0;					// オブジェクトID
	diprop.diph.dwHow			= DIPH_DEVICE;			// デバイスに設定
	diprop.dwData				= 1000;					// バッファサイズ

	hr = m_pDIMouse->SetProperty(DIPROP_BUFFERSIZE, &diprop.diph);
	if ( FAILED(hr) ){ return false; }

	// 協調モードの設定 (非排他モードでフォアグラウンドで動作するように設定)
	hr = m_pDIMouse->SetCooperativeLevel(m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)) { return false; }

	// 入力を許可する
	m_pDIMouse->Acquire();

	return true;
}


//------------------------------------------------------------------------------
//		更新処理
//------------------------------------------------------------------------------
void CMouse::Update()
{
	// リセットする
	m_LAction = m_RAction = m_MAction = false;

	DIDEVICEOBJECTDATA od;	// マウス入力データを格納する構造体
	DWORD dwItems = 1;		// 取得する入力データの数
	HRESULT hr;				// 関数の戻り値
	while (true)
	{
		// マウスの入力データを取得
		hr = m_pDIMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwItems, 0);
		
		// 入力が失われていた場合、デバイスを再取得
		if (hr == DIERR_INPUTLOST)
		{
			m_pDIMouse->Acquire();
		}
		// 取得するデータがない、またはエラーが発生した場合
		else if (dwItems == 0 || FAILED(hr))
		{
			// デバイスが未取得なら再取得を試みる
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
			case DIMOFS_BUTTON0: m_LAction = m_LDown = (od.dwData & 0x80) != 0; break; // 左クリック
			case DIMOFS_BUTTON1: m_RAction = m_RDown = (od.dwData & 0x80) != 0; break; // 右クリック
			case DIMOFS_BUTTON2: m_MAction = m_MDown = (od.dwData & 0x80) != 0; break; // ホイール
			}
			od.dwData = static_cast<DWORD>(0.0f);
		}
	}
}


//------------------------------------------------------------------------------
//		解放処理
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