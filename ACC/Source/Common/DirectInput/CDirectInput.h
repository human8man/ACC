#pragma once
#define DIRECTINPUT_VERSION 0x0800  //DirectXのバージョン設定.

#pragma warning(disable : 28251)
#include <dinput.h> //これを使うにはdinput8.lib dxguid.libのリンクも必用となる.
#pragma comment( lib, "dinput8.lib" )

#include "Common/Singleton/CSingleton.h"

//キーボード.
class CKey
{
private:
	friend class CDInput;

	static constexpr int Button_MAX = 256;
public:
	//押し続けているかの判定する関数.
	bool IsKeyDown(int key) { return m_KeyState[key]; }
	//押した直後の判定関数.
	bool ISKeyAction(int key) { return m_KeyAction[key]; }

	bool Init(CKey& Key, IDirectInput8* pDInput, HWND hWnd);
private:
	CKey();

	bool Create(IDirectInput8* pDCInput, HWND hWnd);
	void Release();
	void UpDate();
private:
	IDirectInputDevice8* m_pDIDevKB;	//デバイス(Dev:Development(開発)).
	bool m_KeyState[Button_MAX];
	bool m_KeyAction[Button_MAX];
};

class CMouse
{
private:
	friend class CDInput;
public:
	//押した直後の判定関数.
	bool ISLAction() { return m_LAction; }
	bool ISRAction() { return m_RAction; }
	bool ISMAction() { return m_MAction; }

	//押している間の判定.
	bool ISLDown() { return m_LDown; }
	bool ISMDown() { return m_MDown; }
private:
	CMouse();

	bool Create(IDirectInput8* pDInput, HWND hWnd);
	void Release();
	void Update();

private:
	IDirectInputDevice8* m_pDIMouse;

	HWND m_hWnd;

	//---------UpDate時点のボタン状態---------.
	bool m_LDown;
	bool m_RDown;
	bool m_MDown;
	//---------ボタン押した直後の判定---------.
	bool m_LAction;
	bool m_RAction;
	bool m_MAction;
};

class CGamePad
{
private:
	friend class CDInput;

	//定数宣言.
	static constexpr int ButtonMAX = 32;
public:
	DIJOYSTATE GetPadpos() const { return m_Pad; }
	//-------------他クラスでのボタン情報を取得する用------------
	//ボタンが押されているかどうかを取得.
	bool GetPadButton(int buttonIndex);
	//ボタンが押された瞬間かどうか.
	bool GetPadButtonDown(int buttonIndex);
	//十字キーが押されているかどうかを取得.//Direction = 0(上), 1(右), 2(下), 3(左).
	bool GetPovAction(int Direction);
	//十字キーが押されている間を取得.
	bool GetPovDown(int Direction);
	//接続が有効か.
	bool IsValid() const;

	HRESULT GetDeviceState();

private:
	//関数定義.
	CGamePad();
	void UpDate();
	int  Create();
	void Release();
private:
	//変数宣言.
	bool m_ButtonPressed[ButtonMAX];	//ボタン配列.
	bool m_PreviousButtonPressed[32];	//前フレームのボタン状態.
	bool m_BPOVButtonPressed[4];
	DIJOYSTATE m_Pad;
	DIJOYSTATE m_PrevPad;

	bool m_IsConect;

};

class CDInput
{
private:
	friend class CSingleton<CDInput>; //シングルトンクラスをフレンド宣言

	//このクラス内でしか使わない定義.
#define UseInputDevice_KEYBOARD 0x01
#define UseInputDevice_MOUSE	0x02
#define UseInputDevice_GAMEPAD	0x04	//0x04にするのはデバイスの重複を避けるため.
#define UseInputDevice_ALL		(UseInputDevice_KEYBOARD | UseInputDevice_MOUSE | UseInputDevice_GAMEPAD)
public:
	//インスタンス生成(唯一のアクセス経路).
	static CDInput* GetInstance()
	{
		static CDInput s_Instance;
		return &s_Instance;
	}
	CDInput();
	~CDInput();

	bool Create(HWND hWnd, int useDevice = UseInputDevice_ALL);

	//コントローラー接続確認.
	bool GamePadConnect();

	void Release();
public:
	//入力状態を更新する.
	void InputUpDate()
	{
		if (!GamePadConnect())
		{
			m_Key.UpDate();
			m_Mouse.Update();
		}
		else
		{
			m_GamePad.UpDate();
		}
	}

	//各デバイスを参照する(ここから入力状態を取得する).
	CKey* CDKeyboard() { return &m_Key; }
	CMouse* CDMouse() { return &m_Mouse; }
	CGamePad* CDGamePad() { return &m_GamePad; }

private:
	HWND m_hWnd;

	//DirectInputオブジェクト.
	IDirectInput8* m_pDCInput;

	//CInputで使うデバイス(初期化対象にする).
	int m_UseDevice;

	//各デバイスの変数宣言.
	CKey	 m_Key;
	CMouse	 m_Mouse;
	CGamePad m_GamePad;

private:
	CDInput(const CDInput& rhs) = delete;
	CDInput& operator = (const CDInput& rhs) = delete;
};
