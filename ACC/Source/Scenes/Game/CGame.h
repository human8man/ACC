#pragma once
#include "Scenes/CSceneBase.h"
#include "DirectX/CDirectX9.h"
#include "DirectX/CDirectX11.h"

#include "DebugText/CDebugText.h"
#include "Collision/GJK/CGJK.h"


class CStaticMesh;
class CGJK;
class CRay;
class CGround;
class CEnemy;
class CPlayer;
class CCharacter;

//============================================================================
//		�Q�[���N���X.
//============================================================================
class CGame
	: public CSceneBase
{
public:
	CGame( HWND hWnd );
	~CGame();

	void Create()	override;
	HRESULT LoadData()override;
	void Release()	override;
	void Init()		override;
	void Update()	override;
	void Draw()		override;


private:
	// �����蔻��֐�.
	void CollisionJudge();
private:
	HWND	m_hWnd;	// �E�B���h�E�n���h��.
	LIGHT	m_Light;// ���C�g���.

	D3DXMATRIX	m_mView;	// �r���[(�J����)�s��.
	D3DXMATRIX	m_mProj;	// �ˉe�i�v���W�F�N�V�����j�s��.

	std::unique_ptr<CStaticMesh> m_pEgg;		// ���܂�.
	std::unique_ptr<CStaticMesh> m_pFloor;		// �n��.
	std::unique_ptr<CStaticMesh> m_pCylinder;	// ��.
		
	std::unique_ptr<CPlayer>	m_pPlayer;
	std::unique_ptr<CEnemy>		m_pEnemy;
	std::unique_ptr<CGround>	m_pGround;

	// GJK�N���X.
	std::unique_ptr<CGJK> m_pGJK;

	std::unique_ptr<CRay> m_pCamRay;

	float m_Angle;
};