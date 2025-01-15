#pragma once
#include "Character/CCharacter.h"

//============================================================================
//		�v���C���[�N���X.
//============================================================================
class CPlayer
	: public CCharacter
{
public:
	CPlayer();
	virtual ~CPlayer() override;

	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

private:
	// �L�[���͏���.
	void KeyInput();

private:
	float m_TurnSpeed;	 // ��]���x.
	float m_MoveSpeed;	 // �ړ����x.
	float m_CamRevision; // �J�������W�̕␳�l.
	float m_JumpPower;	 // �W�����v��.
};