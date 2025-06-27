#pragma once
#include "Character/Character.h"
#include "DirectInput/DirectInput.h"

#pragma warning(push)
#pragma warning(disable: 4834)

class Enemy;

// Hit���
enum HitKind {
	Hit = 1,
	Crit
};


class Player
	: public Character
{
public:
	Player();
	virtual ~Player() override;

	// �X�V����
	void Update(std::unique_ptr<Enemy>& chara);
	// �`�揈��
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light);

	void SetLanding(bool flag)		{ m_Landing = flag; }
	const bool& GetLanding()		const { return m_Landing; }

	const int& GetHitKind()			const { return m_HitKind; }
	const bool& GetHit()			const { return m_Hit; }
	const bool& GetHoming()			const { return m_Homing; }
	const bool& GetAutoAim()		const { return m_AutoAim; }
	const bool& GetWallHack()		const { return m_WallHack; }
	const bool& GetTriggerHappy()	const { return m_TriggerHappy; }

	// �ړ��ʂ̒ǉ�
	void AddVec(D3DXVECTOR3 vec) { m_vPosition += vec; }
	// �����蔻�菈��(�G�l�~�[�A���A��)
	void Collision(std::unique_ptr<Enemy>& egg, Collider floor, Collider cylinder);
private:
	// �L�[���͏���
	void KeyInput(std::unique_ptr<Enemy>& chara);
	// �ړ�����
	void MoveProcess(std::unique_ptr<Enemy>&chara);
	// �_�b�V������
	void DashProcess();
	// ���ˏ���
	void ShotProcess();

	// �f�o�b�O���Ɏg�p����֐�
	void PlayerImGui();

private:
	// GJK�N���X
	std::unique_ptr<GJK> m_pGJK;

	float m_MoveSpeed;	 // �ړ����x
	float m_CamRevision; // �J�������W�̕␳�l

	bool m_Hit;			// ����������
	int	 m_HitKind;		// �����̎��

	bool m_AutoAim;		// �I�[�g�G�C��
	bool m_Homing;		// �z�[�~���O
	bool m_WallHack;	// �E�H�[���n�b�N
	bool m_TriggerHappy;// �A�˃��[�h
};