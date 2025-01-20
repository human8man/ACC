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
	std::unique_ptr<CStaticMesh> m_pCylinder;	// 柱.
		
	std::unique_ptr<CPlayer>	m_pPlayer;
	std::unique_ptr<CEnemy>		m_pEnemy;
	std::unique_ptr<CGround>	m_pGround;

	// GJKクラス.
	std::unique_ptr<CGJK> m_pGJK;
	std::unique_ptr<CRay> m_pCamRay;

	float m_Angle;
};