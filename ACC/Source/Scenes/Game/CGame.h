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


//============================================================================
//		�Q�[���N���X.
//============================================================================
class CGame
	: public CSceneBase
{
public:
	static constexpr int ENEMY_MAX = 3;

	CGame( HWND hWnd );
	~CGame();

	void Create()	override;
	HRESULT LoadData()override;
	void Release()	override;
	void Init()		override;
	void Update()	override;
	void Draw()		override;

private:
	// �E�B���h�E�n���h��.
	HWND			m_hWnd;

	// ���C�g���.
	LIGHT			m_Light;

	// �s��.
	D3DXMATRIX		m_mView;	// �r���[(�J����)�s��.
	D3DXMATRIX		m_mProj;	// �ˉe�i�v���W�F�N�V�����j�s��.

	// �X�^�e�B�b�N���b�V��(�g���܂킷����).
	CStaticMesh*	m_pMeshFighter;	// ���@.
	CStaticMesh*	m_pMeshGround;	// �n��.
	CStaticMesh*	m_pMeshBullet;	// �e.
	CStaticMesh*	m_pMeshGun;	// �e.
		
	// �L�����N�^�[�N���X.
	CCharacter*			m_pPlayer;

	// �n�ʃN���X.
	CGround*			m_pGround;

	// �e�N���X.
	CShot*				m_pShot;
};