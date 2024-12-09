#pragma once

#include "GameObject/Mesh/Static/CStaticMeshObject.h"
#include "Collision/Ray/CRay.h"

/**************************************************
*	�L�����N�^�[�N���X.
**/
class CCharacter
	: public CStaticMeshObject
{
public:
	CCharacter();
	virtual ~CCharacter();

	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj,
		LIGHT& Light, CAMERA& Camera) override;

	//�e���΂��������m�F.
	bool IsShot() const { return m_Shot; }

	//Y�������֐L�΂������C���擾.
	RAY GetRayY() const { return *m_pRayY;  }
	//�\�����C���擾.
	CROSSRAY GetCrossRay() const { return *m_pCrossRay;  }

protected:
	bool		m_Shot;			//�e���΂��t���O.

	RAY*		m_pRayY;		//Y�����֐L�΂������C.
	CROSSRAY*	m_pCrossRay;	//�\���i�O�㍶�E�ɐL�΂����j���C
};

