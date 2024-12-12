#pragma once
#include "Scenes/CSceneBase.h"
#include "DirectX/CDirectX9.h"
#include "DirectX/CDirectX11.h"

#include "DebugText/CDebugText.h"

#include "GameObject/Sprite/CSpriteObject.h"
#include "GameObject/Sprite/2D/CSprite2D.h"
#include "GameObject/Sprite/2D/UI/CUIObject.h"
#include "GameObject/Sprite/3D/CSprite3D.h"
#include "GameObject/Sprite/3D/Explosion/CExplosion.h"

#include "GameObject/Character/CCharacter.h"
#include "GameObject/Character/Player/CPlayer.h"
#include "GameObject/Character/Enemy/CEnemy.h"
#include "GameObject/Character/Zako/CZako.h"

#include "GameObject/Object/Ground/CGround.h"
#include "GameObject/Object/Shot/CShot.h"
#include "GameObject/Mesh/Static/CStaticMesh.h"
#include "GameObject/Mesh/Static/CStaticMeshObject.h"
#include "GameObject/Mesh/Skin/CSkinMesh.h"
#include "Collision/Ray/CRay.h"

/********************************************************************************
*	�Q�[���N���X.
**/
class CGame
	: public CSceneBase
{
public:
	static constexpr int ENEMY_MAX = 3;	//�G�l�~�[�̍ő吔.

	CGame( HWND hWnd );
	~CGame();

	void Create()	override;
	HRESULT LoadData()override;
	void Release()	override;
	void Init()		override;
	void Update()	override;
	void Draw()		override;

private:
	//�J�����֐�.
	void Camera();
	//�v���W�F�N�V�����֐�.
	void Projection();

	//�O�l�̃J����.
	void ThirdPersonCamera(
		CAMERA* pCamera, const D3DXVECTOR3& TargetPos, float TargetRotY );


private:
	CDirectX9*		m_pDx9;
	CDirectX11*		m_pDx11;

	CDebugText*		m_pDbgText;	//�f�o�b�O�e�L�X�g.

	//���C�\���N���X.
	CRay*	m_pRayY;					//Y����(����).
	CRay*	m_pCrossRay[CROSSRAY::max];	//�\��(�O�㍶�E).

	//�E�B���h�E�n���h��.
	HWND			m_hWnd;

	//�J�������.
	CAMERA			m_Camera;
	//���C�g���.
	LIGHT			m_Light;

	//�s��.
	D3DXMATRIX		m_mView;	//�r���[(�J����)�s��.
	D3DXMATRIX		m_mProj;	//�ˉe�i�v���W�F�N�V�����j�s��.

	//�Q�[���ň����X�v���C�g�f�[�^(�g���܂킷����).
	CSprite3D*		m_pSpriteGround;
	CSprite3D*		m_pSpritePlayer;
	CSprite3D*		m_pSpriteExplosion;

	//�X�^�e�B�b�N���b�V��(�g���܂킷����).
	CStaticMesh*	m_pStaticMeshFighter;	//���@.
	CStaticMesh*	m_pStaticMeshGround;	//�n��.
	CStaticMesh*	m_pStaticMeshRoboA;		//���{A.
	CStaticMesh*	m_pStaticMeshRoboB;		//���{B.
	CStaticMesh*	m_pStaticMeshBullet;	//�e.
	CStaticMesh*	m_pStaticMeshBSphere;	//�o�E���f�B���O�X�t�B�A(�����蔻��p).

	//�X�L�����b�V��(�g���܂킷����).
	CSkinMesh*		m_pSkinMeshZako;		//�U�R.
	int				m_ZakoAnimNo;			//�U�R�F�A�j���[�V�����ԍ�.
	double			m_ZakoAnimTime;			//�U�R�F�A�j���[�V�����o�ߎ���.
	D3DXVECTOR3		m_ZakoBonePos;			//�U�R�F�{�[�����W.

	//�X�v���C�g�I�u�W�F�N�g�N���X.
	CSpriteObject*		m_pExplosion;
	
	//�X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X.
	CStaticMeshObject*	m_pStcMeshObj;

	//�L�����N�^�[�N���X.
	CCharacter*			m_pPlayer;
	CCharacter*			m_pEnemy;
	CCharacter*			m_pEnemies[ENEMY_MAX];
	CCharacter**		m_ppEnemies;
	int					m_EnemyMax;

	//�n�ʃN���X.
	CGround*			m_pGround;

	//�e�N���X.
	CShot*				m_pShot;

	//�U�R�N���X.
	CZako*				m_pZako;

	//std::vector<�錾�������^��> �ϐ���.
	std::vector<CZako*>	m_Zako;
};