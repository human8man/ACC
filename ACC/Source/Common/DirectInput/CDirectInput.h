#pragma once
#define DIRECTINPUT_VERSION 0x0800  // DirectXのバージョン設定.

#pragma warning(disable : 28251)
#include <dinput.h> // これを使うにはdinput8.lib dxguid.libのリンクも必用となる.
#pragma comment( lib, "dinput8.lib" )

#include "Common/Singleton/CSingleton.h"

// キーボード.
class CKey
{
private:
	friend class CDInput;

	static constexpr int Button_MAX = 256;
public:
	// 押し続けているかの判定を返す.
	bool IsKeyDown(int key)	  const { return m_KeyState[key];  }
	// 押した瞬間の判定を返す.
	bool IsKeyAction(int key) const { return m_KeyAction[key]; }

	bool Init(CKey& Key, IDirectInput8* pDInput, HWND hWnd);
private:
	CKey();

	bool Create(IDirectInput8* pDCInput, HWND hWnd);
	void Update();
	void Release();

private:
	IDirectInputDevice8* m_pDIDevKB;	// デバイス(Dev:Development(開発)).
	bool m_KeyState[Button_MAX];
	bool m_KeyAction[Button_MAX];
};

class CMouse
{
private:
	friend class CDInput;
public:
	// 押した直後の判定関数.
	bool IsLAction() const { return m_LAction; }
	bool IsRAction() const { return m_RAction; }
	bool IsMAction() const { return m_MAction; }

	// 押している間の判定.
	bool IsLDown() const { return m_LDown; }
	bool IsMDown() const { return m_MDown; }
private:
	CMouse();

	bool Create(IDirectInput8* pDInput, HWND hWnd);
	void Update();
	void Release();

private:
	IDirectInputDevice8* m_pDIMouse;

	HWND m_hWnd;

	//---------Update時点のボタン状態---------.
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
	bool GetPadButton		(int buttonIndex)	const;
	//ボタンが押された瞬間かどうか.
	bool GetPadButtonDown	(int buttonIndex)	const;
	//十字キーが押されているかどうかを取得.//Direction = 0(上), 1(右), 2(下), 3(左).
	//十字キーが押されている間を取得.
	bool GetPovAction		(int Direction);
	bool GetPovDown			(int Direction);

	// 接続中か.
	bool IsValid() const;

	HRESULT GetDeviceState() const;

private:
	CGamePad();

	int  Create();
	void Update();
	void Release();
private:
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

	HRESULT Create(HWND hWnd, int useDevice = UseInputDevice_ALL);

	//コントローラー接続確認.
	bool GamePadConnect();

	void Release();
public:
	//入力状態を更新する.
	void InputUpdate()
	{
		if (!GamePadConnect())
		{
			m_Key.Update();
			m_Mouse.Update();
		}
		else
		{
			m_GamePad.Update();
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
