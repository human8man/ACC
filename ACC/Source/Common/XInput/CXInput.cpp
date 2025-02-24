#include "CXInput.h"
#include <crtdbg.h>


// �l��͈͓��Ɏ��߂�֐�.
template<typename T>
T Clamp( T val, T min, T max )
{
	return ( val < min ) ? min : ( max < val ) ? max : val;
}


// KEY�񋓑̂ɑΉ�����XINPUT_GAMEPAD�̃e�[�u��.
const WORD KEY_TABLE[CXInput::MAX] =
{
	XINPUT_GAMEPAD_DPAD_UP,			// �����p�b�h:��.
	XINPUT_GAMEPAD_DPAD_DOWN,		// �����p�b�h:��.
	XINPUT_GAMEPAD_DPAD_LEFT,		// �����p�b�h:��.
	XINPUT_GAMEPAD_DPAD_RIGHT,		// �����p�b�h:�E.
	XINPUT_GAMEPAD_START,			// �{�^��:�X�^�[�g.
	XINPUT_GAMEPAD_BACK,			// �{�^��:�o�b�N.
	XINPUT_GAMEPAD_LEFT_THUMB,		// �{�^��:���X�e�B�b�N.
	XINPUT_GAMEPAD_RIGHT_THUMB,		// �{�^��:�E�X�e�B�b�N.
	XINPUT_GAMEPAD_LEFT_SHOULDER,	// �{�^��:LB.
	XINPUT_GAMEPAD_RIGHT_SHOULDER,	// �{�^��:RB.
	XINPUT_GAMEPAD_A,				// �{�^��:A.
	XINPUT_GAMEPAD_B,				// �{�^��:B.
	XINPUT_GAMEPAD_X,				// �{�^��:X.
	XINPUT_GAMEPAD_Y,				// �{�^��:Y.
};


//=================================================================================================
//		XInput�N���X.
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
//		�L�[���͂̍X�V.
//=================================================================================================
bool CXInput::Update()
{
	// �L�[�����X�V����O�ɑޔ�.
	m_StateOld = m_State;
	if( UpdateStatus() == false ){ return false; }
	return true;
}


//=================================================================================================
//		�I������.
//=================================================================================================
void CXInput::EndProc()
{
	SetVibration( 0, 0 );
}


//=================================================================================================
//		������.
//=================================================================================================
bool CXInput::IsDown( KEY key, bool Just )
{
	WORD GamePad = GenerateGamePadValue( key );

	if( IsKeyCore( GamePad, m_State ) ) {
		if( Just ) {
			// ������͂őO�񖢓��� �� �������u��.
			if( !IsKeyCore( GamePad, m_StateOld ) ) { return true; }
		}
		return true;
	}
	return false;
}


//=================================================================================================
//		������.
//=================================================================================================
bool CXInput::IsUp( KEY key )
{
	WORD GamePad = GenerateGamePadValue( key );

	// �O����͂ō��񖢓��́��������u��.
	if( IsKeyCore( GamePad, m_StateOld ) && !IsKeyCore( GamePad, m_State ) )
	{
		return true;
	}
	return false;
}


//=================================================================================================
//		���������Ă���.
//=================================================================================================
bool CXInput::IsRepeat( KEY key )
{
	WORD GamePad = GenerateGamePadValue( key );

	//�O����͂ō�����́����������Ă���.
	if( IsKeyCore( GamePad, m_StateOld ) == true &&
		IsKeyCore( GamePad, m_State )	 == true )
	{
		return true;
	}
	return false;
}


//=================================================================================================
//		�U���ݒ�.
//=================================================================================================
bool CXInput::SetVibration( WORD LMotorSpd, WORD RMotorSpd )
{
	m_Vibration.wLeftMotorSpeed  = Clamp( LMotorSpd, VIBRATION_MIN, VIBRATION_MAX );
	m_Vibration.wRightMotorSpeed = Clamp( RMotorSpd, VIBRATION_MIN, VIBRATION_MAX );

	if( ERROR_SUCCESS == XInputSetState( m_PadId, &m_Vibration ) ){ return true; }
	return false;
}


//-------------------------------------------------------------------------------------------------
//		�L�[���͂̍X�V.
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
//		IsKeyDown,Up,Repeat���Ŏg�p����֐�.
//-------------------------------------------------------------------------------------------------
bool CXInput::IsKeyCore( WORD GamePad, const XINPUT_STATE& State )
{
	if( ( State.Gamepad.wButtons & GamePad ) != 0 ){ return true; }
	return false;
}


//-------------------------------------------------------------------------------------------------
//		KEY�񋓑̂�Ή�����XINPUT_GAMEPAD�̒l�ɕϊ�.
//-------------------------------------------------------------------------------------------------
WORD CXInput::GenerateGamePadValue( KEY key )
{
	// �͈͊O�̏ꍇ�̓A�T�[�g�o��.
	if( key < KEY::FIRST || KEY::LAST < key ){
		_ASSERT_EXPR( false, L"�L�[�͈̔͊O���w��" );
	}
	return KEY_TABLE[key];
}