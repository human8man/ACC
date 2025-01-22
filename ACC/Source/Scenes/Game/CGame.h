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
class CGameUI;
class CRandom;


//============================================================================
//		ゲームクラス.
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
	// 敵とプレイヤーの初期化.
	void InitEPPos(CRandom& random, std::unique_ptr<CPlayer>& player, std::unique_ptr<CEnemy>& enemy);

	// 当たり判定関数.
	void CollisionJudge();
	
	// プレイヤーと床の当たり判定をまとめる関数.
	void PlayertoFloorCol	(CollisionPoints points);
	
	// プレイヤーと柱の当たり判定をまとめる関数.
	void PlayertoCylinderCol(CollisionPoints points);
	
	// 敵と床の当たり判定をまとめる関数.
	void EnemytoFloorCol	(CollisionPoints points);

	// 敵と柱の当たり判定をまとめる関数.
	void EnemytoCylinderCol	(CollisionPoints points);

	// レイの当たり判定をまとめる関数.
	void RaytoObjeCol();

private:
	HWND	m_hWnd;	 // ウィンドウハンドル.
	LIGHT	m_Light; // ライト情報.

	D3DXMATRIX	m_mView; // ビュー(カメラ)行列.
	D3DXMATRIX	m_mProj; // 射影（プロジェクション）行列.

	std::unique_ptr<CStaticMesh> m_pEgg;		// たまご.
	std::unique_ptr<CStaticMesh> m_pFloor;		// 地面.
	std::vector<std::unique_ptr<CStaticMesh>> m_pCylinders;
		
	std::unique_ptr<CPlayer>	m_pPlayer;
	std::unique_ptr<CEnemy>		m_pEnemy;
	std::unique_ptr<CGround>	m_pGround;

	// GJKクラス.
	std::unique_ptr<CGJK> m_pGJK;
	std::unique_ptr<CRay> m_pCamRay;

	// UIクラス.
	std::unique_ptr<CGameUI> m_pGameUI;	
	std::vector<D3DXVECTOR3> m_SpawnPoints;

	int	m_HitKind;
	int	m_CylinderMax;
};