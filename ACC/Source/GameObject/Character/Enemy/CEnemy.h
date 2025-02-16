#pragma once

#include "Character/CCharacter.h"
#include "Character/Player/CPlayer.h"


//=============================================================================
//		�G�l�~�[�N���X.
//=============================================================================
class CEnemy
	: public CCharacter
{
public :
	// �ړ��̎��.
	enum MoveKind {
		Straight,
		Left,
		Right,
		Back,
		Wait,

		max,
	};

public:
	CEnemy();
	virtual ~CEnemy();

	// �X�V����.
	void Update(std::unique_ptr<CPlayer>& chara);
	// �`�揈��.
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light );
	
	// �����蔻�菈��(�v���C���[�A���A��).
	void Collision(std::unique_ptr<CPlayer>& egg, Collider floor, Collider cylinder);


	// �ړ��ʂ̎擾.
	const D3DXVECTOR3& GetMoveVec() { return m_SumVec; }
	// �O�I�ȃx�N�g���̒ǉ�.
	void AddVec(D3DXVECTOR3 vec) { m_vPosition += vec; }
	// �W�����v�͂̌���.
	void JumpPowerDec() { if (m_JumpPower > 0) { m_JumpPower -= m_Gravity; } }

private:
	// �s�����܂Ƃ߂�.
	void Act(std::unique_ptr<CPlayer>& chara);

private:
	// �v���C���[���擾�p.
	CPlayer*  m_pPlayer;
	// GJK�N���X.
	std::unique_ptr<CGJK> m_pGJK;

	float m_TurnSpeed;			// ��]���x.
	float m_MoveSpeed;			// �ړ����x.
	float m_CamRevision;		// �J�������W�̕␳�l.

	bool m_Hit;					// ����������.
	int	 m_HitKind;				// �����̎��.

	float m_SelectMoveTime;		// ���̍s������܂ł̎���.
	float m_SelectMoveTimeMax;	// ���̍s������܂ł̍ő厞��.

	int			m_MoveKind;		// �s�����.
	D3DXVECTOR3 m_SumVec;		// ���v�̃x�N�g����.
};