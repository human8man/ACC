#pragma once

#define DIRECTINPUT_VERSION 0x0800  // DirectXのバージョン設定

#pragma warning(disable : 28251)
#include <dinput.h>
#pragma comment( lib, "dinput8.lib" )

#include "Singleton/Singleton.h"


//==============================================================================
//		キーボードクラス
//==============================================================================
class Key
{
private:
	friend class DirectInput;

	static constexpr int Button_MAX = 256;
public:
	bool IsKeyDown(int key)	  const { return m_KeyState[key];  } // 入力中の判定を返す
	bool IsKeyAction(int key) const { return m_KeyAction[key]; } // 入力時の判定を返す
	
	bool Init(Key& Key, IDirectInput8* pDInput, HWND hWnd);
private:
	Key();
	
	// 作成処理
	bool Create(IDirectInput8* pDInput, HWND hWnd);
	// 更新処理
	void Update();
	// 解放処理
	void Release();

private:
	IDirectInputDevice8* m_pDIDevKB;
	bool m_KeyState[Button_MAX];
	bool m_KeyAction[Button_MAX];
};


//==============================================================================
//		マウスクラス
//==============================================================================
class Mouse
{
private:
	friend class DirectInput;

public:
	bool IsLAction() const { return m_LAction; }	// 左クリック時
	bool IsRAction() const { return m_RAction; }	// 右クリック時
	bool IsMAction() const { return m_MAction; }	// ミドルクリック時

	bool IsLDown() const { return m_LDown; }	// 左クリック中
	bool IsRDown() const { return m_RDown; }	// 右クリック中
	bool IsMDown() const { return m_MDown; }	// ミドルクリック中

private:
	Mouse();

	// 作成処理
	bool Create(IDirectInput8* pDInput, HWND hWnd);
	// 更新処理
	void Update();
	// 解放処理
	void Release();

private:
	HWND m_hWnd;
	IDirectInputDevice8* m_pDIMouse;

	// 入力時の判定
	bool m_LAction;	// 左クリック
	bool m_RAction;	// 右クリック
	bool m_MAction;	// ミドルクリック

	// 入力中の判定
	bool m_LDown;	// 左クリック
	bool m_RDown;	// 右クリック
	bool m_MDown;	// ミドルクリック
};


//=============================================================================
//		DirectInputクラス
//=============================================================================
class DirectInput
	: public Singleton<DirectInput>
{
private:
	friend class Singleton<DirectInput>;

	// このクラス内でしか使わない定義
#define UseInputDevice_KEYBOARD 0x01
#define UseInputDevice_MOUSE	0x02
#define UseInputDevice_GAMEPAD	0x04	// 0x04にするのはデバイスの重複を避けるため
#define UseInputDevice_ALL		(UseInputDevice_KEYBOARD | UseInputDevice_MOUSE | UseInputDevice_GAMEPAD)
public:
	DirectInput();
	~DirectInput();

	// 作成処理
	HRESULT Create(HWND hWnd, int useDevice = UseInputDevice_ALL);
	// 解放処理
	void Release();

public:
	// 入力状態の更新
	void InputUpdate();

	// 各デバイスを参照
	Key*		CDKeyboard() { return &m_Key;	}
	Mouse*		CDMouse()	 { return &m_Mouse;	}
private:
	HWND m_hWnd;
	IDirectInput8* m_pDDirectInput;	// DirectInputオブジェクト
	int m_UseDevice;			// 使用するデバイス

	// 各デバイスの変数宣言
	Key	 m_Key;
	Mouse	 m_Mouse;
};