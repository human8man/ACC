#pragma once
#include "Character/CCharacter.h"

class CEnemy;

// Hit���.
enum HitKind {
	Hit = 1,
	Crit
};

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


	// �����t���O��n��.
	const bool& GetHit() { return m_Hit; }
	const int& GetHitKind() { return m_HitKind; }
	// �ړ��ʂ̎擾.
	const D3DXVECTOR3& GetMoveVec() { return m_SumVec; }

	// �ړ��ʂ̒ǉ�.
	void AddVec(D3DXVECTOR3 vec) { m_vPosition += vec; }
	void JumpPowerDec() { if (m_JumpPower > 0) { m_JumpPower -= m_Gravity; } }

	// �����蔻�菈��(�G�l�~�[�A���A��).
	void Collision(std::unique_ptr<CEnemy>& egg, MeshCollider floor, MeshCollider cylinder);

private:
	// �L�[���͏���.
	void KeyInput();

private:
	// GJK�N���X.
	std::unique_ptr<CGJK> m_pGJK;

	float m_TurnSpeed;	 // ��]���x.
	float m_MoveSpeed;	 // �ړ����x.
	float m_CamRevision; // �J�������W�̕␳�l.

	bool m_Hit;		// ����������.
	int	 m_HitKind;	// �����̎��.

	D3DXVECTOR3 m_SumVec;	// ���v�̃x�N�g����.
};