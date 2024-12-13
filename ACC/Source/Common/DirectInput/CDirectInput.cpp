#include "CDirectInput.h"

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

HRESULT CDInput::Create(HWND hWnd, int useDevice)
{
	m_hWnd = hWnd;	// ウィンドウハンドルを保存

	// すでに存在する場合は開放し初期化.
	if (m_pDCInput) { Release(); }

	//DirectInputオブジェクトの生成.
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

	if (useDevice & UseInputDevice_KEYBOARD) {
		m_Key.Create(m_pDCInput, hWnd);
	}
	if (useDevice & UseInputDevice_GAMEPAD) {
		m_Mouse.Create(m_pDCInput, hWnd);
	}

	m_GamePad.Create();

	return S_OK;
}

bool CDInput::GamePadConnect()
{
	if (!m_GamePad.IsValid())
	{
		//ゲームパッドが作成されていないとき.
		return false;
	}

	HRESULT hr = m_GamePad.GetDeviceState();

	//失敗した場合、
	if (FAILED(hr) || hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
	{
		return false;
	}

	return true;
}

void CDInput::Release()
{
	//DirectInputオブジェクトの解放.
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
