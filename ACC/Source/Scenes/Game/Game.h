#pragma once


#include "Scenes/SceneBase.h"
#include "DirectX/DirectX9.h"
#include "DirectX/DirectX11.h"
#include "Collision/GJK/GJK.h"
#include "Sprite/SpriteObject.h"


class StaticMesh;
class Ray;
class MeshLine;
class Enemy;
class Player;
class Character;
class GameUI;
class Random;
class VictoryUI;
class DefeatUI;
class EnemyFrame;
class CollisionManager;


class Game
	: public SceneBase
{
public:
	Game( HWND hWnd );
	~Game();

	void Create()	override;	// 作成処理
	HRESULT LoadData()override;	// データ読込
	void Release()	override;	// 解放処理
	void Init()		override;	// 初期化処理
	void Update()	override;	// 更新処理
	void Draw()		override;	// 描画処理


private:
	// 敵とプレイヤーをンダムでスポーン
	void InitEPPos(Random& random,
		std::unique_ptr<Player>& player,
		std::vector<std::unique_ptr<Enemy>>& enemy);

	// UI処理をまとめる関数
	void UIUpdate();

	// オートエイム時のカメラ処理
	void AutoAimProcess();

private:
	HWND	m_hWnd;	 // ウィンドウハンドル
	LIGHT	m_Light; // ライト情報

	D3DXMATRIX	m_mView; // ビュー(カメラ)行列
	D3DXMATRIX	m_mProj; // 射影（プロジェクション）行列

	std::unique_ptr<StaticMesh> m_pEgg;					// たまご
	std::unique_ptr<StaticMesh> m_pFloor;					// 地面
	std::vector<std::unique_ptr<StaticMesh>> m_pCylinders;	// 柱配列
		
	std::unique_ptr<Player>	m_pPlayer;	// プレイヤークラス

	int	m_EnemyCount;		// 敵の数
	int	m_NearEnemyIndex;	// 近くの敵のインデックス
	std::vector<std::unique_ptr<Enemy>> m_pEnemies;	// 敵配列

	// 当たり判定
	std::unique_ptr<CollisionManager> m_pCollisionManager; // 当たり判定管理クラス
	std::unique_ptr<Ray>		m_pCamRay;		// レイクラス
	std::unique_ptr<MeshLine>	m_pMeshLine;	// メッシュ線クラス

	// UI
	std::unique_ptr<GameUI>	m_pGameUI;	// UIクラス
	std::unique_ptr<VictoryUI>		m_pWinUI;	// 勝利クラス
	std::unique_ptr<DefeatUI>	m_pDefeatUI;	// 敗北クラス

	std::vector<D3DXVECTOR3> m_SpawnPoints;	// スポーン地点配列

	int	m_HitKind;		// Hitの種類
	int	m_CylinderMax;	// 柱の最大数

	bool m_SlowMode;			// スローモーション
	float m_SlowScalingTime;	// スローモーション経過時間
	float m_SlowScalingTimeMax;	// スローモーション最大時間
	float m_SlowScale;			// スローモーション比率
};