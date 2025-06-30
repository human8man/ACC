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
	* @brief  �����蔻��
	* @param  player	: �v���C���[
	* @param  enemies	: �G�z��
	* @param  floor		: ��
	* @param  cylinders	: ���z��
	**************************************************************************/
	void ColJudge(
		Player& player,
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		StaticMesh& floor,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  �v���C���[�Ə��̓����蔻��
	* @param  player	: �v���C���[
	* @param  floor		: ��
	**************************************************************************/
	void PlayertoFloorCol(
		Player& player,
		StaticMesh& floor
	);


	/**************************************************************************
	* @brief  �v���C���[�ƒ��̓����蔻��
	* @param  player	: �v���C���[
	* @param  cylinder	: ���z��
	**************************************************************************/
	void PlayertoCylinderCol(
		Player& player,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  �v���C���[�̒e�̓����蔻��
	* @param  player	: �v���C���[
	* @param  floor		: ���z��
	**************************************************************************/
	void PlayerBulletsCol(
		Player& player,
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		StaticMesh& floor,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  �G�Ə��̓����蔻��
	* @param  player	: �G�z��
	* @param  floor		: ��
	**************************************************************************/
	void EnemytoFloorCol(
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		StaticMesh& floor
	);


	/**************************************************************************
	* @brief  �G�ƒ��̓����蔻��
	* @param  enemies	: �G�z��
	* @param  cylinder	: ���z��
	**************************************************************************/
	void EnemytoCylinderCol(
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  �G�̒e�̓����蔻��
	* @param  enemies	: �G�z��
	* @param  floor		: ��
	* @param  cylinder	: ���z��
	**************************************************************************/
	void EnemyBulletsCol(
		Player& player,
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		StaticMesh& floor,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  ���C�̓����蔻��
	* @param  enemies	: �G�z��
	* @param  floor		: ��
	* @param  cylinders	: ���z��
	**************************************************************************/
	void RaytoObjeCol(
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		StaticMesh& floor,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders
	);


	/**************************************************************************
	* @brief  �ː����ʂ��Ă���ŒZ�����̓G�̃C���f�b�N�X�̎擾
	* @param  playerPos	: �v���C���[���W
	* @param  enemies	: �G�z��
	* @param  cylinders	: ���z��
	* @return int		: �C���f�b�N�X
	**************************************************************************/
	int FindNearestVisibleEnemy(
		const std::vector<std::unique_ptr<Enemy>>& enemies,
		const std::vector<std::unique_ptr<StaticMesh>>& cylinders);

private:	
	// ��x����������ʂ����̂̂��߂̃t���O.
	std::vector<bool> m_EnemyLandingFlags;
	bool m_PlayerLanding ;
};