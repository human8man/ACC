#pragma once
#include "Character/CCharacter.h"

//============================================================================
//		�v���C���[�N���X.
//============================================================================
class CPlayer
	: public CCharacter
{
public:
	// �ړ���ԗ񋓌^.
	enum enMoveState
	{
		Stop	= 0, // ��~.
		Forward,	 // �O�i.
		Backward,	 // ���.
		TurnLeft,	 // ����].
		TurnRight,	 // �E��].
	};

public:
	CPlayer();
	virtual ~CPlayer() override;

	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

private:
	// �L�[���͏���.
	void KeyInput();

private:
	float		m_TurnSpeed;	// ��]���x.
	float		m_MoveSpeed;	// �ړ����x.
	float		m_CamRevision;	// �J�������W�̕␳�l.
	enMoveState	m_MoveState;	// �ړ����.
};