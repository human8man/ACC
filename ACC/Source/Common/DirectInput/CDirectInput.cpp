#include "CDirectInput.h"


//==============================================================================
//		ダイレクトインプットクラス.
//==============================================================================
CInput::CInput()
	: m_hWnd		(NULL)
	, m_pDCInput	(NULL)
	, m_UseDevice	(0)
{	
}

CInput::~CInput()
{
	Release();
}


//==============================================================================
//		作成処理.
//==============================================================================
HRESULT CInput::Create(HWND hWnd, int useDevice)
{
	m_hWnd = hWnd;	// ウィンドウハンドルを保存.

	// すでに存在する場合は解放.
	if (m_pDCInput) { Release(); }

	// DirectInputオブジェクトの生成.
	HRESULT hr = DirectInput8Create(
		::GetModuleHandle(NULL),	// アプリケーションのモジュールハンドル.
		DIRECTINPUT_VERSION,		// 使用するDirectInputのバージョン.
		IID_IDirectInput8,			// DirectInputのインターフェースID.
		(void**)&m_pDCInput,		// 作成されたDirectInputオブジェクトを保存するポインタ.
		NULL);						// 未使用の引数（NULLを指定）.

	// 生成に失敗した場合.
	if ( FAILED(hr) ) { return E_FAIL; }

	// 使用するデバイスの設定を保存.
	m_UseDevice = useDevice;

	if (useDevice & UseInputDevice_KEYBOARD){ m_Key.Create(m_pDCInput, hWnd);   }
	if (useDevice & UseInputDevice_GAMEPAD) { m_Mouse.Create(m_pDCInput, hWnd); }

	return S_OK;
}


//==============================================================================
//		解放処理.
//==============================================================================
void CInput::Release()
{
	// DirectInputオブジェクトの解放.
	if (m_pDCInput != NULL) {
		m_pDCInput->Release();
		m_pDCInput = NULL;
	}

	m_Key.Release();
	m_Mouse.Release();

	m_hWnd = NULL;
	m_UseDevice = 0;
}


//-----------------------------------------------------------------------------
//		入力状態の更新.
//-----------------------------------------------------------------------------
void CInput::InputUpdate()
{
	XINPUT_STATE state;
	bool isConnected = (XInputGetState(0, &state) == ERROR_SUCCESS);

	// キーとマウスの更新.
	m_Key.Update();
	m_Mouse.Update();
}