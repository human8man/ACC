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
*	ゲームクラス.
**/
class CGame
	: public CSceneBase
{
public:
	static constexpr int ENEMY_MAX = 3;	//エネミーの最大数.

	CGame( HWND hWnd );
	~CGame();

	void Create()	override;
	HRESULT LoadData()override;
	void Release()	override;
	void Init()		override;
	void Update()	override;
	void Draw()		override;

private:
	//カメラ関数.
	void Camera();
	//プロジェクション関数.
	void Projection();

	//三人称カメラ.
	void ThirdPersonCamera(
		CAMERA* pCamera, const D3DXVECTOR3& TargetPos, float TargetRotY );


private:
	CDirectX9*		m_pDx9;
	CDirectX11*		m_pDx11;

	CDebugText*		m_pDbgText;	//デバッグテキスト.

	//レイ表示クラス.
	CRay*	m_pRayY;					//Y方向(垂直).
	CRay*	m_pCrossRay[CROSSRAY::max];	//十字(前後左右).

	//ウィンドウハンドル.
	HWND			m_hWnd;

	//カメラ情報.
	CAMERA			m_Camera;
	//ライト情報.
	LIGHT			m_Light;

	//行列.
	D3DXMATRIX		m_mView;	//ビュー(カメラ)行列.
	D3DXMATRIX		m_mProj;	//射影（プロジェクション）行列.

	//ゲームで扱うスプライトデータ(使いまわす資源).
	CSprite3D*		m_pSpriteGround;
	CSprite3D*		m_pSpritePlayer;
	CSprite3D*		m_pSpriteExplosion;

	//スタティックメッシュ(使いまわす資源).
	CStaticMesh*	m_pStaticMeshFighter;	//自機.
	CStaticMesh*	m_pStaticMeshGround;	//地面.
	CStaticMesh*	m_pStaticMeshRoboA;		//ロボA.
	CStaticMesh*	m_pStaticMeshRoboB;		//ロボB.
	CStaticMesh*	m_pStaticMeshBullet;	//弾.
	CStaticMesh*	m_pStaticMeshBSphere;	//バウンディングスフィア(当たり判定用).

	//スキンメッシュ(使いまわす資源).
	CSkinMesh*		m_pSkinMeshZako;		//ザコ.
	int				m_ZakoAnimNo;			//ザコ：アニメーション番号.
	double			m_ZakoAnimTime;			//ザコ：アニメーション経過時間.
	D3DXVECTOR3		m_ZakoBonePos;			//ザコ：ボーン座標.

	//スプライトオブジェクトクラス.
	CSpriteObject*		m_pExplosion;
	
	//スタティックメッシュオブジェクトクラス.
	CStaticMeshObject*	m_pStcMeshObj;

	//キャラクタークラス.
	CCharacter*			m_pPlayer;
	CCharacter*			m_pEnemy;
	CCharacter*			m_pEnemies[ENEMY_MAX];
	CCharacter**		m_ppEnemies;
	int					m_EnemyMax;

	//地面クラス.
	CGround*			m_pGround;

	//弾クラス.
	CShot*				m_pShot;

	//ザコクラス.
	CZako*				m_pZako;

	//std::vector<宣言したい型名> 変数名.
	std::vector<CZako*>	m_Zako;
};