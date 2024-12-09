#include "CDirectInput.h"

//ヘッダーで定義すると.objで定義されると出るため、ここで定義.
LPDIRECTINPUT8 lpDI8;
LPDIRECTINPUTDEVICE8 lpJoystick;


CGamePad::CGamePad()
	: m_ButtonPressed()
	, m_Pad()

	, m_IsConect(true)
{
}

//1つのデバイスごとに呼び出されるコールバック関数.
//この関数は引数と戻り値が同じ.
//ヘッダーでは定義せず、ここで定義し、そのまま使う.
BOOL EnumJoyDeviceProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	//LPCDIDEVICEINSTANCE：DirectInputのデータ型.この中にデバイスの識別子や特性などが含まれる.
	//LPVOID：Long Pointer VOIDの略.具体的な型指定が不要な場合に使用することで柔軟性も持たせれる.
	//lpddi：この引数には呼び出された時のジョイスティック情報が入っている.

	//ジョイスティックデバイスの作成.
	HRESULT ret = lpDI8->CreateDevice(lpddi->guidInstance, &lpJoystick, NULL);
	if (FAILED(ret))
	{
		return DIENUM_STOP;
	}

	//入力データー形式のセット.
	//ジョイスティックには2種類の入力フォーマットがあり、
	//今回の[c_dfDIJoystick]とボタンやアナログの数が多い[c_dfDIJoystick2]が存在する.
	//入力が多いデバイを使用する場合はc_dfDIJoystick2を使う必要がある.
	ret = lpJoystick->SetDataFormat(&c_dfDIJoystick);
	if (FAILED(ret))
	{
		lpJoystick->Release();
		return DIENUM_STOP;
	}

	//今回のコードは排他的処理がないが、排他的処理を入れたい場合はこの↓に入れてください.

	//ここから↓が一番重要なジョイスティックのアナログに関する設定.
	//入力範囲のセット.
	DIPROPRANGE diprg;
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMax = 1000; //アナログ各軸の最高値.
	diprg.lMin = -1000;//アナログ各軸の最低値.

	//x軸.
	diprg.diph.dwObj = DIJOFS_X;    //オフセットマクロを使用して軸を特定.
	lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	//y軸.
	diprg.diph.dwObj = DIJOFS_Y;
	lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	//z軸.
	diprg.diph.dwObj = DIJOFS_Z;
	lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	//rx軸.
	diprg.diph.dwObj = DIJOFS_RX;
	lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	//ry軸.
	diprg.diph.dwObj = DIJOFS_RY;
	lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	//rz軸.
	diprg.diph.dwObj = DIJOFS_RZ;
	lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	//起動準備完了.
	lpJoystick->Poll();

	//構築完了なら.
	char tmp[260];
	WideCharToMultiByte(CP_ACP, 0, lpddi->tszInstanceName, -1, tmp, sizeof(tmp), NULL, NULL);

	WideCharToMultiByte(CP_ACP, 0, lpddi->tszProductName, -1, tmp, sizeof(tmp), NULL, NULL);

	//最初の1つのみで終わる.
	//↓のコメント文大事!.
	/*勘違いしないように説明をするが、ジョイスティックが1つもないとまずこのコールバック関数は一切呼ばれることはない.
	  またすべてのジョイスティックの列挙が終われば、もうこの関数は呼ばれることはない.
	  つまり、列挙をやめるには必ずどこかでDIENUM_STOPをしないといけないというわけではない.*/

	return DIENUM_STOP; //次のデバイスを列挙したい場合はDIENUM_CONTINUEを返す必要がある.
}

void CGamePad::UpDate()
{
	//ジョイスティックがある状態なら.
	if (lpJoystick != NULL)
	{
		DIDEVCAPS dc;
		dc.dwSize = sizeof(dc);
		lpJoystick->GetCapabilities(&dc);

		//動作開始.
		lpJoystick->Acquire();
		//データー取得前にPollが必要なら.
		if (dc.dwFlags & DIDC_POLLEDDATAFORMAT)
		{
			lpJoystick->Poll();
		}

		//前フレームのボタン状態を取得.
		for (DWORD i = 0; i < 32; i++)
		{
			m_PreviousButtonPressed[i] = m_ButtonPressed[i];
		}

		//ジョイスティックの入力.
		ZeroMemory(&m_Pad, sizeof(m_Pad));  //ジョイスティックの初期化.
		HRESULT ret = lpJoystick->GetDeviceState(sizeof(m_Pad), &m_Pad);
		m_IsConect = true;
		if (FAILED(ret))
		{
			//失敗なら再び動作開始を行う.
			lpJoystick->Acquire();
			m_IsConect = false;
		}

		//ボタンの最大数を取得.
		DWORD buttonCount = dc.dwButtons;

		//毎フレーム各ボタンを監視.
		for (DWORD i = 0; i < buttonCount; i++)
		{
			//ボタンが押されたら下のフラグがONになる.
			m_ButtonPressed[i] = m_Pad.rgbButtons[i] & 0x80;
		}

		DWORD pov = m_Pad.rgdwPOV[0];   //POVは四方向の入力情報.

		////十字キーの各方向について押されたかどうかを確認.
		//for (int i = 0; i < 4; i++)
		//{
		//	//POV値を各方向にマッピングして判断.
		//	bool IsPressed = false;
		//	switch (i)
		//	{
		//	case 0: 
		//		IsPressed == (pov == 0);    //上
		//		break;
		//	case 1:
		//		IsPressed == (pov == 9000);    //右
		//		break;
		//	case 2:
		//		IsPressed == (pov == 18000);    //下.
		//		break;
		//	case 3:
		//		IsPressed == (pov == 27000);	//左.
		//	}

		//	//十字キーの状態が変更された場合にフラグを管理.
		//	if (IsPressed && !m_BPOVButtonPressed[i])
		//	{
		//		m_BPOVButtonPressed[i] = true;	//押された.
		//	}
		//	else if (!IsPressed)
		//	{
		//		m_BPOVButtonPressed[i] = false;	//離された.
		//	}
		//}

	}


}

int CGamePad::Create()
{
	//IDirectInput8の作成.
	HRESULT ret = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpDI8, NULL);
	if (FAILED(ret))
	{
		return -1;
	}

	//ジョイスティックの列挙.
	//第一引数：ジョイスティックの列挙なのでDI8DEVCLASS_GAMECTRLを指定.
	//第二引数：上で作ったコールバック関数のポインタを指定.
	//第三引数：ここにはコールバックの第二引数にそのまま値が渡されるので、今回はNULLでよい.
	// ※何らかのデータをコールバックに渡したり、逆にそこに値を返してもらうことも可能.
	//第四引数：現在つながっているデバイスのみを列挙するようにDIEDFL_ATTACHEDONLYのみ指定.
	//	※フォースフィードバック対応のデバイスのみを列挙する場合は「DIEDFL_ATTACHEDONLY|DIEDFL_FORCEFEEDBACK」とすれば良い.
	//
	//EnumDeveices関数では1つもジョイスティックがなかった場合でも成功を返すため
	//ジョイスティックが存在するかどうかは別の方法で判定する必要がある.

	//ちなみにこのコードでは最初に見つかったデバイスを構築するようになっているが、
	//この時正しく構築されていればグローバル変数のlpJoystickにデバイスのポインタが入っているはず.
	//そこでEnumDevicesから戻った後にlpJoystick変数がNULLでないかチェックすることで、
	//ジョイスティックが存在していたか、またジョイスティックが正しく初期化されたかを判断している.
	ret = lpDI8->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyDeviceProc, NULL, DIEDFL_ATTACHEDONLY);
	if (FAILED(ret))
	{
		lpDI8->Release();
		return -1;
	}

	if (!lpJoystick)
	{
		//ジョイスティック1つも見つからない.
		lpDI8->Release();
		return -1;
	}
	//デバイス情報.
	//ジョイスティックの中身がなかった場合は処理を飛ばす.
	return 0;
}

void CGamePad::Release()
{
	if (lpJoystick != NULL)
	{
		lpDI8->Release();
		lpJoystick->Release();
	}
}

//他クラスでボタンの情報を取得する用.
bool CGamePad::GetPadButton(int buttonIndex)
{
	//外からどのボタンかを設定し、そのボタンが押されたらフラグが返るようになっている.
	return m_ButtonPressed[buttonIndex];
}

bool CGamePad::GetPadButtonDown(int buttonIndex)
{
	//押した瞬間用.
	return !m_PreviousButtonPressed[buttonIndex] && m_ButtonPressed[buttonIndex];
}

bool CGamePad::GetPovAction(int Direction)
{
	//Direction = 0(上), 1(右), 2(下), 3(左).
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

	// 前回の状態（ボタンが押されていたかどうか）
	bool WasPressed = m_BPOVButtonPressed[Direction];

	// 状態が変わった瞬間（前回は押されていなかった、今回押された）
	if (!WasPressed && IsPressed)
	{
		// 状態が変わったので、現在押された状態として記録
		m_BPOVButtonPressed[Direction] = true;
		return true;  // 押された瞬間のみ反応
	}

	// 前回押されていて、現在も押されている場合は何もしない
	// もし、現在押されていない場合、次回の判定のために状態をリセット
	if (IsPressed)
	{
		m_BPOVButtonPressed[Direction] = true;
	}
	else
	{
		m_BPOVButtonPressed[Direction] = false;
	}

	return false;  // 継続的に押されている場合は反応しない
}

bool CGamePad::GetPovDown(int Direction)
{
	DWORD pov = m_Pad.rgdwPOV[0];   //POVは四方向の入力情報.

	//十字キーの各方向について押されたかどうかを確認.
	//POV値を各方向にマッピングして判断.

	//0(上), 1(右), 2(下), 3(左).
	bool IsPressed = false;
	switch (Direction)
	{
	case 0:
		IsPressed = (pov == 0);    //上
		break;
	case 1:
		IsPressed = (pov == 9000);    //右
		break;
	case 2:
		IsPressed = (pov == 18000);    //下.
		break;
	case 3:
		IsPressed = (pov == 27000);	//左.
		break;
	}

	//十字キーの状態が変更された場合にフラグを管理.
	if (IsPressed)
	{
		m_BPOVButtonPressed[Direction] = true;	//押された.
		return true;
	}
	else
	{
		m_BPOVButtonPressed[Direction] = false;	//離された.
	}
	return false;
}

bool CGamePad::IsValid() const
{
	//ゲームパッドが作成されているか確認.
	return lpJoystick != nullptr;
}

HRESULT CGamePad::GetDeviceState()
{
	if (m_IsConect)
	{
		return S_OK;
	}

	return E_FAIL;
}
