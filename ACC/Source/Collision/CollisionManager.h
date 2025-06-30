#pragma once

#include "Character/Enemy/Enemy.h"
#include "Character/Player/Player.h"
#include "Mesh/Static/Object/Gun/Gun.h"
#include "Mesh/Static/Object/Bullet/Bullet.h"
#include "Collision/GJK/GJK.h"


class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	/**************************************************************************
	* @brief  当たり判定
	* @param  player	: プレイヤー
	* @param  enemies	: 敵配列
	* @param  floor		: 床
	* @param  cylinders	: 柱配列
	**************************************************************************/
	void ColJudge(
		Player& player,
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		StaticMesh& floor,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  プレイヤーと床の当たり判定
	* @param  player	: プレイヤー
	* @param  floor		: 床
	**************************************************************************/
	void PlayertoFloorCol(
		Player& player,
		StaticMesh& floor
	);


	/**************************************************************************
	* @brief  プレイヤーと柱の当たり判定
	* @param  player	: プレイヤー
	* @param  cylinder	: 柱配列
	**************************************************************************/
	void PlayertoCylinderCol(
		Player& player,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  プレイヤーの弾の当たり判定
	* @param  player	: プレイヤー
	* @param  floor		: 柱配列
	**************************************************************************/
	void PlayerBulletsCol(
		Player& player,
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		StaticMesh& floor,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  敵と床の当たり判定
	* @param  player	: 敵配列
	* @param  floor		: 床
	**************************************************************************/
	void EnemytoFloorCol(
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		StaticMesh& floor
	);


	/**************************************************************************
	* @brief  敵と柱の当たり判定
	* @param  enemies	: 敵配列
	* @param  cylinder	: 柱配列
	**************************************************************************/
	void EnemytoCylinderCol(
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  敵の弾の当たり判定
	* @param  enemies	: 敵配列
	* @param  floor		: 床
	* @param  cylinder	: 柱配列
	**************************************************************************/
	void EnemyBulletsCol(
		Player& player,
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		StaticMesh& floor,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  レイの当たり判定
	* @param  enemies	: 敵配列
	* @param  floor		: 床
	* @param  cylinders	: 柱配列
	**************************************************************************/
	void RaytoObjeCol(
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		StaticMesh& floor,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  射線が通っている最短距離の敵のインデックスの取得
	* @param  playerPos	: プレイヤー座標
	* @param  enemies	: 敵配列
	* @param  cylinders	: 柱配列
	* @return int		: インデックス
	**************************************************************************/
	int FindNearestVisibleEnemy(
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders);

private:	
	// 一度だけ処理を通すもののためのフラグ.
	std::vector<bool> m_EnemyLandingFlags;
	bool m_PlayerLanding ;
};