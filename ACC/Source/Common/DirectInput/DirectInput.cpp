#include "DirectInput.h"


//==============================================================================
//		ダイレクトインプットクラス
//==============================================================================
DirectInput::DirectInput()
	: m_hWnd		(NULL)
	, m_pDDirectInput	(NULL)
	, m_UseDevice	(0)
{	
}

DirectInput::~DirectInput()
{
	Release();
}


//==============================================================================
//		作成処理
//==============================================================================
HRESULT DirectInput::Create(HWND hWnd, int useDevice)
{
	m_hWnd = hWnd;	// ウィンドウハンドルを保存

	// すでに存在する場合は解放
	if (m_pDDirectInput) { Release(); }

	// DirectInputオブジェクトの生成
	HRESULT hr = DirectInput8Create(
		::GetModuleHandle(NULL),	// アプリケーションのモジュールハンドル
		DIRECTINPUT_VERSION,		// 使用するDirectInputのバージョン
		IID_IDirectInput8,			// DirectInputのインターフェースID
		(void**)&m_pDDirectInput,		// 作成されたDirectInputオブジェクトを保存するポインタ
		NULL);						// 未使用の引数（NULLを指定）

	// 生成に失敗した場合
	if ( FAILED(hr) ) { return E_FAIL; }

	// 使用するデバイスの設定を保存
	m_UseDevice = useDevice;

	if (useDevice & UseInputDevice_KEYBOARD){ m_Key.Create(m_pDDirectInput, hWnd);   }
	if (useDevice & UseInputDevice_GAMEPAD) { m_Mouse.Create(m_pDDirectInput, hWnd); }

	return S_OK;
}


//==============================================================================
//		解放処理
//==============================================================================
void DirectInput::Release()
{
	// DirectInputオブジェクトの解放
	if (m_pDDirectInput != NULL) {
		m_pDDirectInput->Release();
		m_pDDirectInput = NULL;
	}

	m_Key.Release();
	m_Mouse.Release();

	m_hWnd = NULL;
	m_UseDevice = 0;
}


//-----------------------------------------------------------------------------
//		入力状態の更新
//-----------------------------------------------------------------------------
void DirectInput::InputUpdate()
{
	// キーとマウスの更新
	m_Key.Update();
	m_Mouse.Update();
}