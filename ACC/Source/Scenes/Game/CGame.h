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
//		ゲームクラス.
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
	// ウィンドウハンドル.
	HWND			m_hWnd;

	// ライト情報.
	LIGHT			m_Light;

	// 行列.
	D3DXMATRIX		m_mView;	// ビュー(カメラ)行列.
	D3DXMATRIX		m_mProj;	// 射影（プロジェクション）行列.

	// スタティックメッシュ(使いまわす資源).
	CStaticMesh*	m_pMeshFighter;	// 自機.
	CStaticMesh*	m_pMeshGround;	// 地面.
	CStaticMesh*	m_pMeshBullet;	// 弾.
	CStaticMesh*	m_pMeshGun;	// 弾.
		
	// キャラクタークラス.
	CCharacter*			m_pPlayer;

	// 地面クラス.
	CGround*			m_pGround;

	// 弾クラス.
	CShot*				m_pShot;
};