#include "CollisionManager.h"
#include "Camera/Camera.h"
#include "DirectSound/SoundManager.h"
#include "Effect/Effect.h"
#include "ImGui/CImGui.h"

CollisionManager::CollisionManager()
	: m_EnemyLandingFlags	()
	, m_PlayerLanding		(false)
{
}
CollisionManager::~CollisionManager()
{
}


//============================================================================
//		����
//============================================================================
void CollisionManager::ColJudge(
	Player& player, 
	const std::vector<std::unique_ptr<Enemy>>& enemies,
	StaticMesh& floor,
	const std::vector<std::unique_ptr<StaticMesh>>& cylinders)
{
	m_PlayerLanding = false;
	PlayertoFloorCol(player, floor);
	PlayertoCylinderCol(player, cylinders);
	if (m_PlayerLanding) {
		player.CanJump();
		player.SetLanding(m_PlayerLanding);
	}
	else {
		player.JumpMath();
	}
	PlayerBulletsCol(player, enemies, floor, cylinders);

	m_EnemyLandingFlags.assign(enemies.size(), false);
	EnemytoFloorCol(enemies, floor);
	EnemytoCylinderCol(enemies, cylinders);
	for (size_t i = 0; i < m_EnemyLandingFlags.size(); ++i)
	{
		if (m_EnemyLandingFlags[i]) {
			enemies[i]->CanJump();
			enemies[i]->SetLanding(m_EnemyLandingFlags[i]);
		}
		else {
			enemies[i]->JumpMath();
		}
	}
	EnemyBulletsCol(player, enemies, floor, cylinders);

	RaytoObjeCol(enemies, floor, cylinders);
}


//============================================================================
//		�v���C���[�Ə��̓����蔻����܂Ƃ߂�֐�
//============================================================================
void CollisionManager::PlayertoFloorCol(
	Player& player,
	StaticMesh& floor)
{
	Collider floorCol, playerCol;

	// �R���C�_�[�ݒ�
	floorCol.SetVertex(floor.GetObjectInfo(), floor.GetVertices());
	playerCol.SetVertex(player.GetObjectInfo(), player.GetMesh()->GetVertices());

	CollisionPoints points = GJK::GJKC(playerCol, floorCol);

	// �������Ă����ꍇ
	if (points.Col)
	{
		m_PlayerLanding = true;

		if (points.Normal.y < 0.f)
		{
			D3DXVECTOR3 setPos = player.GetPos() - points.Normal * points.Depth;
			player.SetPos(setPos);
		}
		else {
			D3DXVec3Normalize(&points.Normal, &points.Normal);
			D3DXVECTOR3 moveVec = player.GetMoveVec();
			float dot = D3DXVec3Dot(&moveVec, &points.Normal);
			moveVec = moveVec - dot * points.Normal;
			player.AddVec(moveVec);
		}
	}
}
//============================================================================
//		�v���C���[�ƒ��̓����蔻����܂Ƃ߂�֐�
//============================================================================
void CollisionManager::PlayertoCylinderCol(
	Player& player,
	const std::vector<std::unique_ptr<StaticMesh>>& cylinders)
{
	for (const auto& cyl : cylinders)
	{
		Collider cylCol, playerCol;
		cylCol.SetVertex(cyl->GetObjectInfo(), cyl->GetVertices());
		playerCol.SetVertex(player.GetObjectInfo(), player.GetMesh()->GetVertices());

		CollisionPoints points = GJK::GJKC(cylCol, playerCol);

		if (points.Col)
		{
			if (points.Normal.y > 0.f)
			{
				m_PlayerLanding = true;
				D3DXVECTOR3 setPos = player.GetPos() - points.Normal * points.Depth;
				player.SetPos(setPos);
			}

			D3DXVec3Normalize(&points.Normal, &points.Normal);
			D3DXVECTOR3 moveVec = player.GetMoveVec();
			float dot = D3DXVec3Dot(&moveVec, &points.Normal);
			moveVec = moveVec - dot * points.Normal;
			player.AddVec(moveVec);
			player.SetPos(player.GetPos() + points.Normal * points.Depth);

			if (points.Depth < 0.05f)
				player.AddVec(-player.GetMoveVec());
		}
	}
}
//============================================================================
//		�v���C���[�̒e�̓����蔻��
//============================================================================
void CollisionManager::PlayerBulletsCol(
	Player& player,
	const std::vector<std::unique_ptr<Enemy>>& enemies,
	StaticMesh& floor, 
	const std::vector<std::unique_ptr<StaticMesh>>& cylinders)
{
	// �G�t�F�N�g�p�n���h��
	static ::EsHandle hEffect = -1;

	// �����蔻��p
	float DistanceToFloorY = 5.f;
	float DistanceToCylinder = 100.f;
	float DistanceToEnemy = 30.f;

	Collider bulletCol, floorCol, cylinderCol, enemyCol;
	CollisionPoints point;
	floorCol.SetVertex(floor.GetObjectInfo(), floor.GetVertices());
	D3DXVECTOR3 floorCenter = floorCol.GetCenter();

	// �e���[�v
	for (size_t i = 0; i < player.GetBullets().size(); )
	{
		Bullet* bullet = player.GetBullets()[i].get();
		D3DXVECTOR3 bulletCenter = bullet->GetPos();

		bulletCol.SetVertex(
			bullet->GetObjectInfo(),
			bullet->GetMesh()->GetVertices());

		bool hit = false;

		// ������
		float diffY = fabsf(bulletCenter.y - floorCenter.y);
		if (diffY < DistanceToFloorY && !player.GetThroughWall())
		{
			point = GJK::GJKC(bulletCol, floorCol);
			if (point.Col)
			{
				hEffect = Effect::Play(
					Effect::Dust,
					bullet->GetPos() - bullet->GetMoveVec() * 2);
				hit = true;
			}
		}

		// ������
		if (!hit && !player.GetThroughWall())
		{
			for (const auto& cylinder : cylinders)
			{
				cylinderCol.SetVertex(
					cylinder->GetObjectInfo(),
					cylinder->GetVertices());
				D3DXVECTOR3 cylinderCenter = cylinderCol.GetCenter();
				D3DXVECTOR3 diff = bulletCenter - cylinderCenter;

				if (D3DXVec3Length(&diff) <= DistanceToCylinder)
				{
					if (GJK::GJKC(bulletCol, cylinderCol).Col)
					{
						hEffect = Effect::Play(
							Effect::Dust,
							bullet->GetPos() - bullet->GetMoveVec() * 2);
						hit = true;
						break;
					}
				}
			}
		}

		// �G����
		if (!hit)
		{
			for (const auto& enemy : enemies)
			{
				enemyCol.SetVertex(
					enemy->GetObjectInfo(),
					enemy->GetMesh()->GetVertices());
				D3DXVECTOR3 enemyCenter = enemyCol.GetCenter();
				D3DXVECTOR3 diff = bulletCenter - enemyCenter;
				if (D3DXVec3Length(&diff) <= DistanceToEnemy)
				{
					if (GJK::GJKC(bulletCol, enemyCol).Col)
					{
						if (bullet->GetPos().y < enemy->GetPos().y + player.GetEggAirRoomY())
						{
							enemy->CritDamage();
							hEffect = Effect::Play(Effect::CritHit, enemy->GetPos());
							player.SetHitKind(HitKind::Crit);
							SoundManager::GetInstance()->Play(SoundManager::enList::SE_CritHit);
						}
						else
						{
							D3DXVECTOR3 enemyPos = enemy->GetPos();
							enemyPos.y += 2.f;
							enemy->BodyDamage();
							hEffect = Effect::Play(Effect::Hit, enemyPos);
							player.SetHitKind(HitKind::Hit);
							SoundManager::GetInstance()->Play(SoundManager::enList::SE_Hit);
						}
						player.SetHit(true);
						hit = true;
						break;
					}
				}
			}
		}

		// ����������폜
		if (hit) {
			player.GetBullets().erase(player.GetBullets().begin() + i);
		}
		else {
			++i;
		}
	}
}


//============================================================================
//		�G�Ə��̓����蔻����܂Ƃ߂�֐�
//============================================================================
void CollisionManager::EnemytoFloorCol(
	const std::vector<std::unique_ptr<Enemy>>& enemies,
	StaticMesh& floor)
{
	Collider Floorcol,Enemycol;
	CollisionPoints colpoints;


	for (size_t i = 0; i < enemies.size(); ++i) {
		Enemycol.SetVertex(enemies[i]->GetObjectInfo(), enemies[i]->GetMesh()->GetVertices());
		Floorcol.SetVertex(floor.GetObjectInfo(), floor.GetVertices());

		colpoints = GJK::GJKC(Enemycol, Floorcol);

		if (colpoints.Col)
		{
			m_EnemyLandingFlags[i] = true;

			if (colpoints.Normal.y < 0.f)
			{
				D3DXVECTOR3 SetPos = enemies[i]->GetPos() - colpoints.Normal * colpoints.Depth;
				enemies[i]->SetPos(SetPos);
			}
			else {
				D3DXVec3Normalize(&colpoints.Normal, &colpoints.Normal);
				D3DXVECTOR3 EnemyMove = enemies[i]->GetMoveVec();
				float Dot = D3DXVec3Dot(&EnemyMove, &colpoints.Normal);
				EnemyMove = EnemyMove - Dot * colpoints.Normal;
				enemies[i]->AddVec(EnemyMove);
			}
		}
	}
}
//============================================================================
//		�G�ƒ��̓����蔻����܂Ƃ߂�֐�
//============================================================================
void CollisionManager::EnemytoCylinderCol(
	const std::vector<std::unique_ptr<Enemy>>& enemies,
	const std::vector<std::unique_ptr<StaticMesh>>& cylinders)
{
	for (const auto& cyl : cylinders)
	{
		Collider cylCol, enemyCol;
		cylCol.SetVertex(cyl->GetObjectInfo(), cyl->GetVertices());

		for (size_t i = 0; i < enemies.size(); ++i) {
			enemyCol.SetVertex(enemies[i]->GetObjectInfo(), enemies[i]->GetMesh()->GetVertices());
			CollisionPoints points = GJK::GJKC(cylCol, enemyCol);

			if (points.Col)
			{
				if (points.Normal.y > 0.f)
				{
					m_EnemyLandingFlags[i] = true;
					D3DXVECTOR3 setPos = enemies[i]->GetPos() - points.Normal * points.Depth;
					enemies[i]->SetPos(setPos);
				}

				D3DXVec3Normalize(&points.Normal, &points.Normal);
				D3DXVECTOR3 moveVec = enemies[i]->GetMoveVec();
				float dot = D3DXVec3Dot(&moveVec, &points.Normal);
				moveVec = moveVec - dot * points.Normal;
				enemies[i]->AddVec(moveVec);
				enemies[i]->SetPos(enemies[i]->GetPos() + points.Normal * points.Depth);

				if (points.Depth < 0.05f)
					enemies[i]->AddVec(-enemies[i]->GetMoveVec());
			}
		}
	}
}
//============================================================================
//		�G�̒e�̓����蔻��
//============================================================================
void CollisionManager::EnemyBulletsCol(
	Player& player,
	const std::vector<std::unique_ptr<Enemy>>& enemies,
	StaticMesh& floor, 
	const std::vector<std::unique_ptr<StaticMesh>>& cylinders)
{
	// �G�t�F�N�g�p�n���h��
	static ::EsHandle hEffect = -1;

	// �����蔻��p
	const float DistanceToFloorY = 5.f;
	const float DistanceToCylinder = 100.f;
	const float DistanceToPlayer = 30.f;

	Collider bulletCol, floorCol, cylinderCol, playerCol;
	CollisionPoints point;
	floorCol.SetVertex(floor.GetObjectInfo(), floor.GetVertices());
	D3DXVECTOR3 floorCenter = floorCol.GetCenter();

	// �v���C���[�R���C�_�[
	playerCol.SetVertex(player.GetObjectInfo(), player.GetMesh()->GetVertices());
	D3DXVECTOR3 playerCenter = playerCol.GetCenter();

	// �S�G���[�v
	for (auto& enemy : enemies)
	{
		auto& bullets = enemy->GetBullets();

		for (size_t i = 0; i < bullets.size(); )
		{
			Bullet* bullet = bullets[i].get();
			D3DXVECTOR3 bulletCenter = bullet->GetPos();

			bulletCol.SetVertex(
				bullet->GetObjectInfo(),
				bullet->GetMesh()->GetVertices());

			bool hit = false;

			// ������
			float diffY = fabsf(bulletCenter.y - floorCenter.y);
			if (diffY < DistanceToFloorY)
			{
				point = GJK::GJKC(bulletCol, floorCol);
				if (point.Col)
				{
					hEffect = Effect::Play(
						Effect::Dust,
						bullet->GetPos() - bullet->GetMoveVec() * 2);
					hit = true;
				}
			}

			// ������
			if (!hit)
			{
				for (const auto& cylinder : cylinders)
				{
					cylinderCol.SetVertex(
						cylinder->GetObjectInfo(),
						cylinder->GetVertices());
					D3DXVECTOR3 cylinderCenter = cylinderCol.GetCenter();
					D3DXVECTOR3 diff = bulletCenter - cylinderCenter;

					if (D3DXVec3Length(&diff) <= DistanceToCylinder)
					{
						if (GJK::GJKC(bulletCol, cylinderCol).Col)
						{
							hEffect = Effect::Play(
								Effect::Dust,
								bullet->GetPos() - bullet->GetMoveVec() * 2);
							hit = true;
							break;
						}
					}
				}
			}

			// �v���C���[����
			if (!hit && !player.GetInvincible())
			{
				D3DXVECTOR3 diff = bulletCenter - playerCenter;
				if (D3DXVec3Length(&diff) <= DistanceToPlayer)
				{
					if (GJK::GJKC(bulletCol, playerCol).Col)
					{
						if (bullet->GetPos().y < enemy->GetPos().y + player.GetEggAirRoomY()) {
							SoundManager::GetInstance()->Play(SoundManager::enList::SE_DamageCrit);
							player.CritDamage();
						}
						else {
							SoundManager::GetInstance()->Play(SoundManager::enList::SE_Damage);
							player.BodyDamage();
						}
						player.SetHit(true);
						hit = true;
						break;
					}
				}
			}

			// ����������폜
			if (hit) {
				bullets.erase(bullets.begin() + i);
			}
			else {
				++i;
			}
		}
	}
}


//============================================================================
//		���C�̓����蔻��
//============================================================================
void CollisionManager::RaytoObjeCol(
	const std::vector<std::unique_ptr<Enemy>>& enemies,
	StaticMesh& floor, 
	const std::vector<std::unique_ptr<StaticMesh>>& cylinders)
{
	D3DXVECTOR3 camPos = Camera::GetInstance()->GetPos();
	D3DXVECTOR3 camDir = Camera::GetInstance()->GetCamDir();

	D3DXVECTOR3 defaultTarget = camPos + camDir * 100000.0f;

	RayInfo nearestHit;
	nearestHit.Hit = false;
	nearestHit.HitPos = defaultTarget;
	nearestHit.Length = FLT_MAX;

	const RAY& ray = Camera::GetInstance()->GetRay();

	RayInfo groundRay = floor.IsHitForRay(ray);
	if (groundRay.Hit && groundRay.Length < nearestHit.Length)
		nearestHit = groundRay;

	for (const auto& cyl : cylinders)
	{
		RayInfo cylRay = cyl->IsHitForRay(ray);
		if (cylRay.Hit && cylRay.Length < nearestHit.Length)
			nearestHit = cylRay;
	}

	for (const auto& enemy : enemies)
	{
		RayInfo enemyRay = enemy->IsHitForRay(ray);
		if (enemyRay.Hit && enemyRay.Length < nearestHit.Length)
			nearestHit = enemyRay;
	}

	Camera::GetInstance()->SetRayHit(nearestHit.HitPos);
}


//============================================================================
//		�ŒZ�������ː����ʂ��Ă���G�̃C���f�b�N�X�̎擾
//============================================================================
int CollisionManager::FindNearestVisibleEnemy(
	const std::vector<std::unique_ptr<Enemy>>& enemies,
	const std::vector<std::unique_ptr<StaticMesh>>& cylinders)
{
	D3DXVECTOR3 campos = Camera::GetInstance()->GetPos();

	int nearestIndex = 0;
	float nearestDist = FLT_MAX;

	for (size_t i = 0; i < enemies.size(); ++i)
	{
		D3DXVECTOR3 enemyPos = enemies[i]->GetPos();
		D3DXVECTOR3 vecToEnemy = enemyPos - campos;
		float lengthToEnemy = D3DXVec3Length(&vecToEnemy);

		const float MaxRayLength = 10000.0f;

		D3DXVECTOR3 axis;
		D3DXVec3Normalize(&axis, &vecToEnemy);

		RAY camtoenemy;
		camtoenemy.Position = campos;
		camtoenemy.Axis = axis;
		camtoenemy.Length = MaxRayLength;
		camtoenemy.RotationY = atan2f(axis.x, axis.z);

		bool visible = true;

		for (const auto& cyl : cylinders)
		{
			RayInfo cylHit = cyl->IsHitForRay(camtoenemy);

			if (cylHit.Hit && cylHit.Length < lengthToEnemy - 0.01f)
			{
				visible = false;
				break;
			}
		}

		if (visible && lengthToEnemy < nearestDist)
		{
			nearestDist = lengthToEnemy;
			nearestIndex = static_cast<int>(i);
		}
	}
	return nearestIndex;
}
