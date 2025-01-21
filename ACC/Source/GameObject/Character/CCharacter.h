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
	// �L������ԗ񋓑�.
	enum CharaState
	{
		// �����ɏ�ԂƂ������Ă���.
	};
	// �`�[�g��ԗ�.
	enum CheatState
	{
		// �����Ɏg���`�[�g�Ƃ������Ă���.
	};

	// �L�������\����.
	struct CharaInfo
	{
		int HP;
		int MaxHP;
		int Ammo;
		int MaxAmmo;
		CharaState Status;
		CheatState UsingCheat1;
		CheatState UsingCheat2;
		CheatState UsingCheat3;
		CheatState UsingCheat4;
	};
public:
	CCharacter();
	virtual ~CCharacter();

	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj,LIGHT& Light) override;

	// �W�����v�֘A.
	void CanJump() { m_CanJump = true; } // �W�����v���\�ɂ���.

	// �d�͊֘A.
	void ResetGravity()	{ m_Gravity		 = m_GravityValue; } // �d�͂����Z�b�g����.
	void AddGravity()	{ m_Gravity		+= m_GravityValue; } // �d�͂����Z���Ă���.
	void UseGravity()	{ m_vPosition.y -= m_Gravity;	   } // ���Z�����d�͂��L������Y�l�Ɍ��Z.

	void DecreHP()		{ m_CharaInfo.HP--; }	 // HP�����炷.
	void DubleDecreHP()	{ m_CharaInfo.HP -= 2; } // HP���{���炷.


	// Y�������֐L�΂������C���擾.
	RAY GetRayY() const { return *m_pRayY; }
	// �\�����C���擾.
	CROSSRAY GetCrossRay() const { return *m_pCrossRay; }
	// �L�����̏����擾.
	CharaInfo GetCharaInfo() const { return m_CharaInfo; }
protected:
	RAY*		m_pRayY;		// Y�����֐L�΂������C.
	CROSSRAY*	m_pCrossRay;	// �\���i�O�㍶�E�ɐL�΂����j���C
	
	CStaticMesh*			m_pMeshBullet;	// �e���b�V��.
	CStaticMesh*			m_pMeshGun;		// �e���b�V��.
	std::vector<CBullet*>	m_pBullets;		// �e�z��.
	CGun*					m_pGun;			// �e.

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




	float m_EggAirRoomY;	// �C���̍�������p.

	CharaInfo m_CharaInfo;	// �L�����̏��.
};