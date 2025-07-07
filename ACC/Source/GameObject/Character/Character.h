#pragma once

#include "Time/Time.h"
#include "Collision/GJK/GJK.h"
#include "Collision/Ray/Ray.h"
#include "Collision/Line/MeshLine.h"
#include "Mesh/Static/StaticMeshObject.h"
#include "Mesh/Static/Object/Gun/Gun.h"
#include "Mesh/Static/Object/Bullet/Bullet.h"


class Character
	: public StaticMeshObject
{
public:
	// �L�������\����
	struct CharaInfo
	{
		int HP;
		int MaxHP;
		int Ammo;
		int MaxAmmo;
	};
public:
	Character();
	virtual ~Character();

	// �X�V����
	virtual void Update() override;
	// �`�揈��
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj,LIGHT& Light) override;


	void JumpMath();		// �W�����v�֌W�̌v�Z
	void CanJump()		{ m_CanJump = true;}	// �W�����v���\�ɂ���
	// HP�𓷑̃_���[�W�����炷
	void BodyDamage();
	// HP���N���e�B�J���_���[�W�����炷
	void CritDamage();

	// �ŏI�ړ����̃x�N�g����n��
	D3DXVECTOR3 GetMoveVec();
	RAY GetRayY()			const { return *m_pRayY; }
	float GetReloadTime()	const { return m_ReloadTime; }
	float GetReloadTimeMax()const { return m_ReloadTimeMax; }
	float GetEggAirRoomY()	const { return m_EggAirRoomY; }
	CROSSRAY GetCrossRay()	const { return *m_pCrossRay; }
	CharaInfo GetCharaInfo()const { return m_CharaInfo; }
	bool GetDamage()		const{ return m_Damage; }
	bool GetCanJump()		const{ return m_CanJump; }
	bool GetLanding()		const{ return m_Landing; }
	bool GetRotGun()		const { return m_RotGun; }
	void SetLanding(bool flag)	{ m_Landing = flag; }

	std::vector<std::unique_ptr<Bullet>>& GetBullets()	{ return m_pBullets; }
protected:
	std::unique_ptr<RAY>		m_pRayY;		// Y�����֐L�΂������C
	std::unique_ptr<CROSSRAY>	m_pCrossRay;	// �\���i�O�㍶�E�ɐL�΂����j���C
	
	std::unique_ptr<StaticMesh>			m_pMeshGun;		// �e���b�V��
	std::unique_ptr<StaticMesh>			m_pMeshBullet;	// �e���b�V��
	std::unique_ptr<MeshLine>				m_pMeshLine;	// ���b�V�����N���X
	std::unique_ptr<Gun>					m_pGun;			// �e�N���X
	std::vector<std::unique_ptr<Bullet>>	m_pBullets;		// �e�N���X�z��

	// �q�b�g���Ɏg���_���[�W�ϐ�
	int m_BodyDamage;	// ���̂ɖ������Ă����ꍇ
	int m_CritDamage;	// �C���ɖ���(�w�b�h�V���b�g)���Ă����ꍇ
	bool m_Damage;		// �_���[�W�t���O(���t���[���ŏ�����)

	// �e�֘A
	float m_GunRadius;		// �e�̔��a
	float m_GunRotRevision;	// �e�̌����␳�l
	float m_GunPosRevision;	// �e�̉�]���W�␳�l(�x)

	// �e�֘A
	float m_ReloadTime;			// �����[�h�^�C��
	float m_ReloadTimeMax;		// �����[�h�ő�^�C��
	float m_BulletCoolTime;		// �e�̔��˃N�[���^�C��
	float m_BulletCoolTimeMax;	// �e�̔��ˍő�N�[���^�C��
	float m_BulletCoolTimeMin;	// �e�̔��ˍŏ��N�[���^�C��
	float m_BulletSpeed;		// �e���x
	bool  m_CanShot;			// ���ˉ\��

	// �W�����v�֘A
	float	m_Gravity;		// �d��
	float	m_JumpPower;	// �W�����v��
	float	m_JumpPowerMax;	// �ő�W�����v��
	bool	m_CanJump;		// �W�����v���\��
	bool	m_Landing;		// ���n���Ă��邩

	// �_�b�V���֘A
	float	m_DashCoolTime;		// �_�b�V���̃N�[���^�C��
	float	m_DashCoolTimeMax;	// �_�b�V���̍ő�N�[���^�C��
	float	m_DashTime;			// �_�b�V���̔��f����
	float	m_DashTimeMax;		// �_�b�V���̍ő唽�f����
	float	m_DashSpeed;		// �_�b�V���{��
	bool	m_CanDash;			// �_�b�V�����\��
	D3DXVECTOR3	m_DashVec;		// �_�b�V���̃x�N�g���ۑ��p

	// �C��(�w�b�h�V���b�g)�̔���p
	float m_EggAirRoomY;

	// �L�������
	CharaInfo m_CharaInfo;
	// ���v�̈ړ��x�N�g����
	D3DXVECTOR3 m_SumVec;

	bool m_RotGun;
	float m_RotSpeed;
};