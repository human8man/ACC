#pragma once

#include "Time/CTime.h"
#include "Object/Gun/CGun.h"
#include "Collision/GJK/CGJK.h"
#include "Collision/Ray/CRay.h"
#include "Object/Bullet/CBullet.h"
#include "Collision/Line/CMeshLine.h"
#include "Mesh/Static/CStaticMeshObject.h"


//============================================================================
//		�L�����N�^�[�N���X.
//============================================================================
class CCharacter
	: public CStaticMeshObject
{
public:
	// �L�������\����.
	struct CharaInfo
	{
		int HP;
		int MaxHP;
		int Ammo;
		int MaxAmmo;
	};
public:
	CCharacter();
	virtual ~CCharacter();

	// �X�V����.
	virtual void Update() override;
	// �`�揈��.
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj,LIGHT& Light) override;


	void JumpMath();		// �W�����v�֌W�̌v�Z.
	void CanJump()		{ m_CanJump = true;}	// �W�����v���\�ɂ���.
	void BodyDamage()	{ m_CharaInfo.HP -= m_BodyDamage;	}	// HP�𓷑̃_���[�W�����炷.
	void CritDamage()	{ m_CharaInfo.HP -= m_CritDamage;	}	// HP���N���e�B�J���_���[�W�����炷.


	const D3DXVECTOR3& GetMoveVec();	// �ŏI�ړ��x�N�g����n��.
	RAY GetRayY()			const { return *m_pRayY; }		// Y�������֐L�΂������C���擾.
	float GetReloadTime()	const { return m_ReloadTime; }	// �����[�h���Ԃ�Ԃ�.
	CROSSRAY GetCrossRay()	const { return *m_pCrossRay; }	// �\�����C���擾.
	CharaInfo GetCharaInfo()const { return m_CharaInfo; }	// �L�����̏����擾.
	bool GetCanJump()		const{ return m_CanJump; }		// �W�����v�\�����擾.
	bool GetLanding()		const{ return m_Landing; }		// ���n�������擾.
	void SetLanding(bool flag)	{ m_Landing = flag; }		// ���n������ݒ�.

protected:
	std::unique_ptr<RAY>		m_pRayY;		// Y�����֐L�΂������C.
	std::unique_ptr<CROSSRAY>	m_pCrossRay;	// �\���i�O�㍶�E�ɐL�΂����j���C.
	
	std::unique_ptr<CStaticMesh>			m_pMeshGun;		// �e���b�V��.
	std::unique_ptr<CStaticMesh>			m_pMeshBullet;	// �e���b�V��.
	std::unique_ptr<CMeshLine>				m_pMeshLine;	// ���b�V�����N���X.
	std::unique_ptr<CGun>					m_pGun;			// �e�N���X.
	std::vector<std::unique_ptr<CBullet>>	m_pBullets;		// �e�N���X�z��.

	// �q�b�g���Ɏg���_���[�W�ϐ�.
	int m_BodyDamage;	// ���̂ɖ������Ă����ꍇ.
	int m_CritDamage;	// �C���ɖ���(�w�b�h�V���b�g)���Ă����ꍇ.

	// �e�֘A.
	float m_GunRadius;		// �e�̔��a.
	float m_GunRotRevision;	// �e�̌����␳�l.
	float m_GunPosRevision;	// �e�̉�]���W�␳�l(�x).

	// �e�֘A.
	float m_ReloadTime;			// �����[�h�^�C��.
	float m_ReloadTimeMax;		// �����[�h�ő�^�C��.
	float m_BulletCoolTime;		// �e�̔��˃N�[���^�C��.
	float m_BulletCoolTimeMax;	// �e�̔��ˍő�N�[���^�C��.
	float m_BulletSpeed;		// �e���x.
	bool  m_CanShot;			// ���ˉ\��.

	// �W�����v�֘A.
	float	m_Gravity;		// �d��.
	float	m_JumpPower;	// �W�����v��.
	float	m_JumpPowerMax;	// �ő�W�����v��.
	bool	m_CanJump;		// �W�����v���\��.
	bool	m_Landing;		// ���n���Ă��邩.

	// �_�b�V���֘A.
	float	m_DashCoolTime;		// �_�b�V���̃N�[���^�C��.
	float	m_DashCoolTimeMax;	// �_�b�V���̍ő�N�[���^�C��.
	float	m_DashTime;			// �_�b�V���̔��f����.
	float	m_DashTimeMax;		// �_�b�V���̍ő唽�f����.
	float	m_DashSpeed;		// �_�b�V���{��.
	bool	m_CanDash;			// �_�b�V�����\��.
	D3DXVECTOR3	m_DashVec;		// �_�b�V���̃x�N�g���ۑ��p.

	// �C��(�w�b�h�V���b�g)�̔���p.
	float m_EggAirRoomY;

	// �L�������.
	CharaInfo m_CharaInfo;
	// ���v�̈ړ��x�N�g����.
	D3DXVECTOR3 m_SumVec;
};