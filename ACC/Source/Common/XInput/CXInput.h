#pragma once

#include <Xinput.h>
#pragma comment( lib, "xinput.lib" )


//=============================================================================
//		XInput�N���X.
//=============================================================================
class CXInput
{
public:
	// �g���K�[���͔͈�.
	static constexpr BYTE	TRIGGER_MIN		= 0;
	static constexpr BYTE	TRIGGER_MAX		= 255;
	// �X�e�B�b�N���͔͈�.
	static constexpr SHORT	THUMB_MIN		=-32768;
	static constexpr SHORT	THUMB_MAX		= 32767;
	// �o�C�u���[�V�����l�͈�.
	static constexpr WORD	VIBRATION_MIN	= 0;
	static constexpr WORD	VIBRATION_MAX	= 65535;

	// �L�[�񋓑�.
	enum KEY
	{
		UP,		// �����p�b�h:��.
		DOWN,	// �����p�b�h:��.
		LEFT,	// �����p�b�h:��.
		RIGHT,	// �����p�b�h:�E.
		START,	// �{�^��:�X�^�[�g.
		BACK,	// �{�^��:�o�b�N.
		LSTICK,	// �{�^��:���X�e�B�b�N.
		RSTICK,	// �{�^��:�E�X�e�B�b�N.
		LB,		// �{�^��:LB.
		RB,		// �{�^��:RB.
		A,		// �{�^��:A.
		B,		// �{�^��:B.
		X,		// �{�^��:X.
		Y,		// �{�^��:Y.

		MAX,
		FIRST	= UP,
		LAST	= Y,
	};

public:
	CXInput( DWORD padId );
	~CXInput();

	// �X�V����.
	bool Update();
	// �I������.
	void EndProc();
	
	bool IsDown( KEY key, bool Just = false );	// ������.
	bool IsRepeat( KEY key );					// ���������Ă���.
	bool IsUp( KEY key );						// ������.
	
	// �U���ݒ�.
	bool SetVibration( WORD LMotorSpd, WORD RMotorSpd );

	// �ڑ��`�F�b�N.
	bool IsConnect() const { return m_Connect; }
	// �p�b�h�ԍ��擾.
	DWORD GetPadID() const { return m_PadId; }

	BYTE GetLTrigger() const { return m_State.Gamepad.bLeftTrigger;  } // ���g���K�[�擾.
	BYTE GetRTrigger() const { return m_State.Gamepad.bRightTrigger; } // �E�g���K�[�擾.
	SHORT GetLThumbX() const { return m_State.Gamepad.sThumbLX; } // ���X�e�B�b�NX���擾.
	SHORT GetLThumbY() const { return m_State.Gamepad.sThumbLY; } // ���X�e�B�b�NY���擾.
	SHORT GetRThumbX() const { return m_State.Gamepad.sThumbRX; } // �E�X�e�B�b�NX���擾.
	SHORT GetRThumbY() const { return m_State.Gamepad.sThumbRY; } // �E�X�e�B�b�NY���擾.

private:
	// �L�[���͂̍X�V.
	bool UpdateStatus();
	// IsDown,IsUp,IsRepeat�֐����Ŏg�p����֐�.
	bool IsKeyCore( WORD GamePad, const XINPUT_STATE& State );
	// KEY�񋓑̂�Ή�����XINPUT_GAMEPAD�̒l�ɕϊ�.
	WORD GenerateGamePadValue( KEY key );

private:
	DWORD			 m_PadId;		// �p�b�h�ԍ�(0~3).
	XINPUT_STATE	 m_State;		// �L�[���͏��.
	XINPUT_STATE	 m_StateOld;	// �L�[���͏��(�L�[�X�g���[�N����p).
	XINPUT_VIBRATION m_Vibration;	// �U��.
	bool			 m_Connect;		// �ڑ�����.
};