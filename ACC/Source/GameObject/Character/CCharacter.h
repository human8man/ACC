#pragma once

#include "Mesh/Static/CStaticMeshObject.h"
#include "Collision/Ray/CRay.h"
#include "Object/Bullet/CBullet.h"
#include "Object/Gun/CGun.h"

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


	void UseGravity() { m_vPosition.y -= m_Gravity; }







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

	float m_GunRadius;		// �e�̔��a.
	float m_GunRotRevision;	// �e�̌����␳�l.
	float m_Gravity;		// �d��.

	CharaInfo m_CharaInfo;	// �L�����̏��.
};