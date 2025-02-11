#include "CDirectInput.h"


// ヘッダーで定義すると.objで定義されると出るため、ここで定義.
LPDIRECTINPUT8 lpDI8;
LPDIRECTINPUTDEVICE8 lpJoystick;


//--------------------------------------------------------------------------------
//		ゲームパッドクラス.
//--------------------------------------------------------------------------------
CGamePad::CGamePad()
	: m_ButtonPressed			()
	, m_PreviousButtonPressed	()
	, m_BPOVButtonPressed		()
	, m_Pad						()
	, m_PrevPad					()
	, m_IsConect				(true)
{
}


//--------------------------------------------------------------------------------
//		ジョイスティックデバイスの列挙時に呼ばれるコールバック関数.
//--------------------------------------------------------------------------------
BOOL EnumJoyDeviceProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	// ジョイスティックデバイスを作成.
	HRESULT ret = lpDI8->CreateDevice(lpddi->guidInstance, &lpJoystick, NULL);
	if (FAILED(ret)) { return DIENUM_STOP; }

	// データフォーマットの設定.
	ret = lpJoystick->SetDataFormat(&c_dfDIJoystick);
	if (FAILED(ret))
	{
		lpJoystick->Release();
		return DIENUM_STOP;
	}

	// 入力範囲設定（X, Y, Z, RX, RY, RZ 軸）.
	DIPROPRANGE diprg = {};
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMax = 1000; // 最大値.
	diprg.lMin = -1000; // 最小値.

	// 各軸に対して範囲設定.
	for (DWORD axis : {DIJOFS_X, DIJOFS_Y, DIJOFS_Z, DIJOFS_RX, DIJOFS_RY, DIJOFS_RZ})
	{
		diprg.diph.dwObj = axis;
		lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);
	}

	// デバイスのポーリングを実行.
	lpJoystick->Poll();

	// デバイス名を取得（使用していないが処理内容として残す）.
	char tmp[260];
	WideCharToMultiByte(CP_ACP, 0, lpddi->tszInstanceName, -1, tmp, sizeof(tmp), NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, lpddi->tszProductName, -1, tmp, sizeof(tmp), NULL, NULL);

	return DIENUM_STOP; // 次のデバイスを列挙したい場合は DIENUM_CONTINUE.
}


//--------------------------------------------------------------------------------
//		作成処理.
//--------------------------------------------------------------------------------
int CGamePad::Create()
{
	// IDirectInput8の作成.
	HRESULT ret = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpDI8, NULL);
	if (FAILED(ret)) { return -1; }

	// 接続されているゲームコントローラー（ジョイスティック）を列挙.
	ret = lpDI8->EnumDevices(
		DI8DEVCLASS_GAMECTRL,	// ジョイスティックの列挙.
		EnumJoyDeviceProc,		// 上記のコールバック関数のポインタを指定.
		NULL,					// コールバックの第二引数にそのまま値が渡される.
		DIEDFL_ATTACHEDONLY);	// 現在接続済みのデバイスのみを列挙する.

	// 列挙に失敗した場合、もしくはジョイスティックが存在しない場合.
	if (FAILED(ret) || !lpJoystick)
	{
		lpDI8->Release();
		return -1;
	}

	return 0;
}


//--------------------------------------------------------------------------------
//		更新処理.
//--------------------------------------------------------------------------------
void CGamePad::Update()
{
	// ジョイスティックがある状態なら.
	if (lpJoystick != NULL)
	{
		DIDEVCAPS dc = {};
		dc.dwSize = sizeof(dc);
		lpJoystick->GetCapabilities(&dc);

		// 動作開始.
		lpJoystick->Acquire();
		// データー取得前にPollが必要なら.
		if (dc.dwFlags & DIDC_POLLEDDATAFORMAT)
		{
			lpJoystick->Poll();
		}

		// 前フレームのボタン状態を取得.
		for (DWORD i = 0; i < 32; i++)
		{
			m_PreviousButtonPressed[i] = m_ButtonPressed[i];
		}

		// ジョイスティックの入力.
		ZeroMemory(&m_Pad, sizeof(m_Pad));  // ジョイスティックの初期化.
		HRESULT ret = lpJoystick->GetDeviceState(sizeof(m_Pad), &m_Pad);
		m_IsConect = true;
		if (FAILED(ret))
		{
			// 失敗なら再び動作開始を行う.
			lpJoystick->Acquire();
			m_IsConect = false;
		}

		// ボタンの最大数を取得.
		DWORD buttonCount = dc.dwButtons;

		// 毎フレーム各ボタンを監視.
		for (DWORD i = 0; i < buttonCount; i++)
		{
			// ボタンが押されたら下のフラグがONになる.
			m_ButtonPressed[i] = m_Pad.rgbButtons[i] & 0x80;
		}

		DWORD pov = m_Pad.rgdwPOV[0];   // POVは四方向の入力情報.
	}
}


//--------------------------------------------------------------------------------
//		解放処理.
//--------------------------------------------------------------------------------
void CGamePad::Release()
{
	if (lpJoystick != NULL) {
		lpDI8->Release();
		lpJoystick->Release();
	}
}


//==============================================================================
//		十字キー入力時判定.
//==============================================================================
bool CGamePad::GetPovAction(int Direction)
{
	// Direction = 0(上), 1(右), 2(下), 3(左).
	bool IsPressed = false;
	switch (Direction)
	{
	case 0:
		IsPressed = (m_Pad.rgdwPOV[0] == 0);
		break;
	case 1:
		IsPressed = (m_Pad.rgdwPOV[0] == 9000);
		break;
	case 2:
		IsPressed = (m_Pad.rgdwPOV[0] == 18000);
		break;
	case 3:
		IsPressed = (m_Pad.rgdwPOV[0] == 27000);
		break;
	}

	// 前回の状態を保存.
	bool WasPressed = m_BPOVButtonPressed[Direction];

	// 状態変化時.
	if (!WasPressed && IsPressed) {
		// 状態が変わったので、現在入力中として記録.
		m_BPOVButtonPressed[Direction] = true;
		return true; // 入力時のみ反応.
	}

	// 前回押されていて、現在も押されている場合は何もしない.
	//	もし、現在押されていない場合、次回の判定のために状態をリセット.
	m_BPOVButtonPressed[Direction] = IsPressed;

	return false; // 継続的に押されている場合は反応しない.
}


//==============================================================================
//		十字キー入力中判定.
//==============================================================================
bool CGamePad::GetPovDown(int Direction)
{
	DWORD pov = m_Pad.rgdwPOV[0];	// POVは四方向の入力情報.

	// 十字キーの各方向について押されたかどうかを確認.
	//	POV値を各方向にマッピングして判断.
	bool IsPressed = false;
	switch (Direction)
	{
	case 0:	// 上.
		IsPressed = (pov == 0);
		break;
	case 1:	// 右.
		IsPressed = (pov == 9000);
		break;
	case 2:	// 下.
		IsPressed = (pov == 18000);
		break;
	case 3:	// 左.
		IsPressed = (pov == 27000);
		break;
	}

	// 十字キーの状態が変更された場合にフラグを管理.
	if (IsPressed) {
		m_BPOVButtonPressed[Direction] = true;	// 押された.
		return true;
	}
	else {
		m_BPOVButtonPressed[Direction] = false;	// 離された.
	}
	return false;
}


//==============================================================================
//		値を返すだけの処理達.
//			単純な処理の割に長いためここに中身を書く.
//==============================================================================
// ジョイスティックが作成されているか.
bool CGamePad::IsValid() const
{
	return lpJoystick != nullptr;
}

// ゲームパッドが接続中か.
HRESULT CGamePad::GetDeviceState() const
{
	if ( m_IsConect ) { return S_OK; }
	return E_FAIL;
}

// ボタンの情報を取得する.
bool CGamePad::GetPadButtonAction(int buttonIndex) const
{
	return m_ButtonPressed[buttonIndex];
}

// 押されているボタンの情報を取得.
bool CGamePad::GetPadButtonDown(int buttonIndex)const
{
	return !m_PreviousButtonPressed[buttonIndex] && m_ButtonPressed[buttonIndex];
}