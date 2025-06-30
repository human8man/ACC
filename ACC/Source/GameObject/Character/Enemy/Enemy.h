#pragma once

#include "Character/Character.h"
#include "Character/Player/Player.h"


class Enemy
	: public Character
{
public :
	// �ړ��̎��
	enum MoveKind {
		Straight,
		Left,
		Right,
		Back,
		Wait,

		max,
	};

public:
	Enemy();
	virtual ~Enemy();

	// �X�V����
	void Update(std::unique_ptr<Player>& chara);
	// �`�揈��
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light );

	// �O�I�ȃx�N�g���̒ǉ�
	void AddVec(D3DXVECTOR3 vec) { m_vPosition += vec; }

private:
	// �s�����܂Ƃ߂�
	void Act(std::unique_ptr<Player>& chara);

private:
	// �v���C���[���擾�p
	Player*  m_pPlayer;
	// GJK�N���X
	std::unique_ptr<GJK> m_pGJK;

	float m_MoveSpeed;	// �ړ����x

	bool m_WallHack;	// �v���C���[���E�H�[���n�b�N�g�p����
	bool m_Hit;			// ����������
	int	 m_HitKind;		// �����̎��

	float m_SelectMoveTime;		// ���̍s������܂ł̎���
	float m_SelectMoveTimeMax;	// ���̍s������܂ł̍ő厞��

	int	m_MoveKind;			// �s�����
	int	m_MoveToPlayer;		// �v���C���[�ւ̈ړ��m��(�ő�100%)
	int m_DashProbability;	// �_�b�V�����g���m��
	int m_JumpProbability;	// �W�����v���g���m��
	int m_ShootProbability;	// �ˌ�����m��
};