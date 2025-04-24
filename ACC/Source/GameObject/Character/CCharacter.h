#pragma once

#include "Mesh/Static/CStaticMeshObject.h"
#include "Collision/Ray/CRay.h"
#include "Object/Bullet/CBullet.h"
#include "Object/Gun/CGun.h"
#include "Collision/GJK/CGJK.h"
#include "Common/Time/CTime.h"


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


	void CanJump()		{ m_CanJump = true;					}	// �W�����v���\�ɂ���.
	void ResetGravity()	{ m_Gravity = m_GravityValue;		}	// �d�͂����Z�b�g����.
	void AddGravity()	{ m_Gravity += m_GravityValue;		}	// �d�͂����Z���Ă���.
	void UseGravity()	{ m_vPosition.y -= m_Gravity;		}	// ���Z�����d�͂��L������Y�l�Ɍ��Z.
	void BodyDamage()	{ m_CharaInfo.HP -= m_BodyDamage;	}	// HP�𓷑̃_���[�W�����炷.
	void CritDamage()	{ m_CharaInfo.HP -= m_CritDamage;	}	// HP���N���e�B�J���_���[�W�����炷.

	
	RAY GetRayY()			const { return *m_pRayY; }		// Y�������֐L�΂������C���擾.
	float GetReloadTime()	const { return m_ReloadTime; }	// �����[�h���Ԃ�Ԃ�.
	CROSSRAY GetCrossRay()	const { return *m_pCrossRay; }	// �\�����C���擾.
	CharaInfo GetCharaInfo()const { return m_CharaInfo; }	// �L�����̏����擾.

protected:
	std::unique_ptr<RAY>		m_pRayY;		// Y�����֐L�΂������C.
	std::unique_ptr<CROSSRAY>	m_pCrossRay;	// �\���i�O�㍶�E�ɐL�΂����j���C
	
	std::unique_ptr<CStaticMesh>			m_pMeshGun;			// �e���b�V��.
	std::unique_ptr<CStaticMesh>			m_pMeshBullet;		// �e���b�V��.
	std::unique_ptr<CGun>					m_pGun;				// �e�N���X.
	std::vector<std::unique_ptr<CBullet>>	m_pBullets;			// �e�N���X�z��.

	// �q�b�g���Ɏg���_���[�W�ϐ�.
	int m_BodyDamage;	// ���̂ɖ������Ă����ꍇ.
	int m_CritDamage;	// �C���ɖ���(�w�b�h�V���b�g)���Ă����ꍇ.

	// �e�֘A.
	float m_GunRadius;		// �e�̔��a.
	float m_GunRotRevision;	// �e�̌����␳�l.
	float m_GunPosRevision;	// �e�̉�]���W�␳�l(�x).
	float m_Gravity;		// ���ۂɊ|����d��.
	float m_GravityValue;	// �d�́i�萔�j.

	// �e�֘A.
	float m_ReloadTime;			// �����[�h�^�C��.
	float m_ReloadTimeMax;		// �����[�h�ő�^�C��.
	float m_BulletCoolTime;		// �e�̔��˃N�[���^�C��.
	float m_BulletCoolTimeMax;	// �e�̔��ˍő�N�[���^�C��.
	float m_BulletSpeed;		// �e���x.
	bool  m_CanShot;			// ���ˉ\��.

	// �W�����v�֘A.
	float m_JumpPower;		// �W�����v��.
	float m_JumpPowerMax;	// �ő�W�����v��.
	bool m_CanJump;			// �W�����v���\��.

	// �_�b�V���֘A.
	float m_DashCoolTime;	// �_�b�V���̃N�[���^�C��.
	float m_DashCoolTimeMax;// �_�b�V���̍ő�N�[���^�C��.
	float m_DashTime;		// �_�b�V���̔��f����.
	float m_DashTimeMax;	// �_�b�V���̍ő唽�f����.
	float m_DashSpeed;		// �_�b�V���{��.
	bool m_CanDash;			// �_�b�V�����\��.
	D3DXVECTOR3	DashVec;	// �_�b�V���̃x�N�g���ۑ��p.

	// �C��(�w�b�h�V���b�g)�̔���p.
	float m_EggAirRoomY;

	// �L�������.
	CharaInfo m_CharaInfo;
};