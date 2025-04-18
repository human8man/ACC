#pragma once


#include "Scenes/CSceneBase.h"
#include "DirectX/CDirectX9.h"
#include "DirectX/CDirectX11.h"

#include "Sprite/CSpriteObject.h"
#include "Collision/GJK/CGJK.h"


class CStaticMesh;
class CGJK;
class CRay;
class CMeshLine;
class CEnemy;
class CPlayer;
class CCharacter;
class CGameUI;
class CRandom;
class CWinUI;
class CLoseUI;
class CEnemyFrame;

//============================================================================
//		ゲームクラス.
//============================================================================
class CGame
	: public CSceneBase
{
public:
	CGame( HWND hWnd );
	~CGame();

	void Create()	override;	// 作成処理.
	HRESULT LoadData()override;	// データ読込.
	void Release()	override;	// 解放処理.
	void Init()		override;	// 初期化処理.
	void Update()	override;	// 更新処理.
	void Draw()		override;	// 描画処理.


private:
	// 敵とプレイヤーをンダムでスポーン.
	void InitEPPos(CRandom& random, std::unique_ptr<CPlayer>& player, std::unique_ptr<CEnemy>& enemy);

	// 当たり判定関数.
	void CollisionJudge();
	
	// プレイヤーの床と柱の当たり判定をまとめる関数.
	void PlayertoFloorCol	(CollisionPoints points);
	void PlayertoCylinderCol(CollisionPoints points);
	
	// 敵の床と柱の当たり判定をまとめる関数.
	void EnemytoFloorCol	(CollisionPoints points);
	void EnemytoCylinderCol	(CollisionPoints points);

	// レイの当たり判定をまとめる関数.
	void RaytoObjeCol();

	// UI処理をまとめる関数.
	void UIUpdate();

private:
	HWND	m_hWnd;	 // ウィンドウハンドル.
	LIGHT	m_Light; // ライト情報.

	D3DXMATRIX	m_mView; // ビュー(カメラ)行列.
	D3DXMATRIX	m_mProj; // 射影（プロジェクション）行列.

	std::unique_ptr<CStaticMesh> m_pEgg;					// たまご.
	std::unique_ptr<CStaticMesh> m_pFloor;					// 地面.
	std::vector<std::unique_ptr<CStaticMesh>> m_pCylinders;	// 柱配列.
		
	std::unique_ptr<CPlayer>	m_pPlayer;	// プレイヤークラス.
	std::unique_ptr<CEnemy>		m_pEnemy;	// 敵クラス.

	// 当たり判定.
	std::unique_ptr<CGJK>		m_pGJK;			// GJKクラス.
	std::unique_ptr<CRay>		m_pCamRay;		// レイクラス.
	std::unique_ptr<CMeshLine>	m_pMeshLine;	// メッシュ線クラス.

	// UI.
	std::unique_ptr<CGameUI>	m_pGameUI;	// UIクラス.
	std::unique_ptr<CWinUI>		m_pWinUI;	// 勝利クラス.
	std::unique_ptr<CLoseUI>	m_pLoseUI;	// 敗北クラス.

	std::vector<D3DXVECTOR3> m_SpawnPoints;	// スポーン地点配列.

	int	m_HitKind;		// Hitの種類.
	int	m_CylinderMax;	// 柱の最大数.
};