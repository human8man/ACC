#pragma once
#define DIRECTINPUT_VERSION 0x0800  // DirectXのバージョン設定.

#pragma warning(disable : 28251)
#include <dinput.h>
#pragma comment( lib, "dinput8.lib" )

#include "Common/Singleton/CSingleton.h"

//==============================================================================
//		キーボードクラス.
//==============================================================================
class CKey
{
private:
	friend class CDInput;

	static constexpr int Button_MAX = 256;
public:
	bool IsKeyDown(int key)	  const { return m_KeyState[key];  } // 入力中の判定を返す.
	bool IsKeyAction(int key) const { return m_KeyAction[key]; } // 入力時の判定を返す.
	
	bool Init(CKey& Key, IDirectInput8* pDInput, HWND hWnd);
private:
	CKey();

	bool Create(IDirectInput8* pDCInput, HWND hWnd);
	void Update();
	void Release();

private:
	IDirectInputDevice8* m_pDIDevKB;
	bool m_KeyState[Button_MAX];
	bool m_KeyAction[Button_MAX];
};

//==============================================================================
//		マウスクラス.
//==============================================================================
class CMouse
{
private:
	friend class CDInput;
public:
	// 入力時の判定.
	bool IsLAction() const { return m_LAction; }
	bool IsRAction() const { return m_RAction; }
	bool IsMAction() const { return m_MAction; }

	// 入力中の判定.
	bool IsLDown() const { return m_LDown; }
	bool IsRDown() const { return m_RDown; }
	bool IsMDown() const { return m_MDown; }
private:
	CMouse();

	bool Create(IDirectInput8* pDInput, HWND hWnd);
	void Update();
	void Release();
private:
	HWND m_hWnd;
	IDirectInputDevice8* m_pDIMouse;

	// 入力時の判定.
	bool m_LAction;
	bool m_RAction;
	bool m_MAction;

	// 入力中の判定.
	bool m_LDown;
	bool m_RDown;
	bool m_MDown;
};


//==============================================================================
//		ゲームパッドクラス.
//==============================================================================
class CGamePad
{
private:
	friend class CDInput;

	//定数宣言.
	static constexpr int ButtonMAX = 32;
public:
	DIJOYSTATE GetPadpos() const { return m_Pad; }
	
	bool IsValid() const;			// ジョイスティックが作成されているか.
	HRESULT GetDeviceState() const;	// ゲームパッドが接続中か.
	
	bool GetPadButtonAction	(int buttonIndex)	const;	// ボタン入力時判定を取得.
	bool GetPadButtonDown	(int buttonIndex)	const;	// ボタン入力中判定を取得.
	bool GetPovAction		(int Direction);	// 十字キー入力時判定.
	bool GetPovDown			(int Direction);	// 十字キー入力中判定.
private:
	CGamePad();

	int  Create();
	void Update();
	void Release();

private:
	bool m_ButtonPressed[ButtonMAX];	// ボタン配列.
	bool m_PreviousButtonPressed[32];	// 前フレームのボタン状態.
	bool m_BPOVButtonPressed[4];
	DIJOYSTATE m_Pad;
	DIJOYSTATE m_PrevPad;

	bool m_IsConect;

};


//==============================================================================
//		DirectInputクラス.
//==============================================================================
class CDInput
	: public CSingleton<CDInput>
{
private:
	friend class CSingleton<CDInput>;

	// このクラス内でしか使わない定義.
#define UseInputDevice_KEYBOARD 0x01
#define UseInputDevice_MOUSE	0x02
#define UseInputDevice_GAMEPAD	0x04	// 0x04にするのはデバイスの重複を避けるため.
#define UseInputDevice_ALL		(UseInputDevice_KEYBOARD | UseInputDevice_MOUSE | UseInputDevice_GAMEPAD)
public:
	CDInput();
	~CDInput();

	HRESULT Create(HWND hWnd, int useDevice = UseInputDevice_ALL);
	bool GamePadConnect();	//コントローラー接続確認.
	void Release();
public:
	// 入力状態の更新.
	void InputUpdate();

	// 各デバイスを参照.
	CKey*		CDKeyboard() { return &m_Key;	  }
	CMouse*		CDMouse()	 { return &m_Mouse;	  }
	CGamePad*	CDGamePad()	 { return &m_GamePad; }

private:
	HWND m_hWnd;
	IDirectInput8* m_pDCInput;	// DirectInputオブジェクト.
	int m_UseDevice;			// 使用するデバイス.

	//各デバイスの変数宣言.
	CKey	 m_Key;
	CMouse	 m_Mouse;
	CGamePad m_GamePad;
};