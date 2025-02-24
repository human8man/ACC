#include "CXInput.h"
#include <crtdbg.h>


// 値を範囲内に収める関数.
template<typename T>
T Clamp( T val, T min, T max )
{
	return ( val < min ) ? min : ( max < val ) ? max : val;
}


// KEY列挙体に対応したXINPUT_GAMEPADのテーブル.
const WORD KEY_TABLE[CXInput::MAX] =
{
	XINPUT_GAMEPAD_DPAD_UP,			// 方向パッド:上.
	XINPUT_GAMEPAD_DPAD_DOWN,		// 方向パッド:下.
	XINPUT_GAMEPAD_DPAD_LEFT,		// 方向パッド:左.
	XINPUT_GAMEPAD_DPAD_RIGHT,		// 方向パッド:右.
	XINPUT_GAMEPAD_START,			// ボタン:スタート.
	XINPUT_GAMEPAD_BACK,			// ボタン:バック.
	XINPUT_GAMEPAD_LEFT_THUMB,		// ボタン:左スティック.
	XINPUT_GAMEPAD_RIGHT_THUMB,		// ボタン:右スティック.
	XINPUT_GAMEPAD_LEFT_SHOULDER,	// ボタン:LB.
	XINPUT_GAMEPAD_RIGHT_SHOULDER,	// ボタン:RB.
	XINPUT_GAMEPAD_A,				// ボタン:A.
	XINPUT_GAMEPAD_B,				// ボタン:B.
	XINPUT_GAMEPAD_X,				// ボタン:X.
	XINPUT_GAMEPAD_Y,				// ボタン:Y.
};


//=================================================================================================
//		XInputクラス.
//=================================================================================================
CXInput::CXInput( DWORD padId )
	: m_PadId		( padId )
	, m_State		()
	, m_StateOld	()
	, m_Vibration	()
	, m_Connect		( false )
{
}

CXInput::~CXInput()
{
	EndProc();
}


//=================================================================================================
//		キー入力の更新.
//=================================================================================================
bool CXInput::Update()
{
	// キー情報を更新する前に退避.
	m_StateOld = m_State;
	if( UpdateStatus() == false ){ return false; }
	return true;
}


//=================================================================================================
//		終了処理.
//=================================================================================================
void CXInput::EndProc()
{
	SetVibration( 0, 0 );
}


//=================================================================================================
//		押した.
//=================================================================================================
bool CXInput::IsDown( KEY key, bool Just )
{
	WORD GamePad = GenerateGamePadValue( key );

	if( IsKeyCore( GamePad, m_State ) ) {
		if( Just ) {
			// 今回入力で前回未入力 → 押した瞬間.
			if( !IsKeyCore( GamePad, m_StateOld ) ) { return true; }
		}
		return true;
	}
	return false;
}


//=================================================================================================
//		離した.
//=================================================================================================
bool CXInput::IsUp( KEY key )
{
	WORD GamePad = GenerateGamePadValue( key );

	// 前回入力で今回未入力→離した瞬間.
	if( IsKeyCore( GamePad, m_StateOld ) && !IsKeyCore( GamePad, m_State ) )
	{
		return true;
	}
	return false;
}


//=================================================================================================
//		押し続けている.
//=================================================================================================
bool CXInput::IsRepeat( KEY key )
{
	WORD GamePad = GenerateGamePadValue( key );

	//前回入力で今回入力→押し続けている.
	if( IsKeyCore( GamePad, m_StateOld ) == true &&
		IsKeyCore( GamePad, m_State )	 == true )
	{
		return true;
	}
	return false;
}


//=================================================================================================
//		振動設定.
//=================================================================================================
bool CXInput::SetVibration( WORD LMotorSpd, WORD RMotorSpd )
{
	m_Vibration.wLeftMotorSpeed  = Clamp( LMotorSpd, VIBRATION_MIN, VIBRATION_MAX );
	m_Vibration.wRightMotorSpeed = Clamp( RMotorSpd, VIBRATION_MIN, VIBRATION_MAX );

	if( ERROR_SUCCESS == XInputSetState( m_PadId, &m_Vibration ) ){ return true; }
	return false;
}


//-------------------------------------------------------------------------------------------------
//		キー入力の更新.
//-------------------------------------------------------------------------------------------------
bool CXInput::UpdateStatus()
{
	m_Connect = false;
	if( ERROR_SUCCESS == XInputGetState( m_PadId, &m_State ) )
	{
		m_Connect = true;
		return true;
	}
	return false;
}


//-------------------------------------------------------------------------------------------------
//		IsKeyDown,Up,Repeat内で使用する関数.
//-------------------------------------------------------------------------------------------------
bool CXInput::IsKeyCore( WORD GamePad, const XINPUT_STATE& State )
{
	if( ( State.Gamepad.wButtons & GamePad ) != 0 ){ return true; }
	return false;
}


//-------------------------------------------------------------------------------------------------
//		KEY列挙体を対応したXINPUT_GAMEPADの値に変換.
//-------------------------------------------------------------------------------------------------
WORD CXInput::GenerateGamePadValue( KEY key )
{
	// 範囲外の場合はアサート出す.
	if( key < KEY::FIRST || KEY::LAST < key ){
		_ASSERT_EXPR( false, L"キーの範囲外を指定" );
	}
	return KEY_TABLE[key];
}