#include "CollisionManager.h"
#include "Camera/Camera.h"


//============================================================================
//		判定
//============================================================================
void CollisionManager::ColJudge(
	Player& player, 
	const std::vector<std::unique_ptr<Enemy>>& enemies,
	StaticMesh& floor,
	const std::vector<std::unique_ptr<StaticMesh>>& cylinders)
{
	PlayertoFloorCol(player, floor);
	PlayertoCylinderCol(player, cylinders);
	EnemytoFloorCol(enemies, floor);
	EnemytoCylinderCol(enemies, cylinders);
	RaytoObjeCol(enemies, floor, cylinders);
}


//============================================================================
//		
//============================================================================
void CollisionManager::PlayertoFloorCol(
	Player& player,
	StaticMesh& floor)
{
	Collider floorCol, playerCol;

	// コライダー設定
	floorCol.SetVertex(floor.GetObjectInfo(), floor.GetVertices());
	playerCol.SetVertex(player.GetObjectInfo(), player.GetMesh()->GetVertices());

	CollisionPoints points = GJK::GJKC(playerCol, floorCol);

	// 当たっていた場合
	if (points.Col)
	{
		player.SetLanding(true);

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
	else
	{
		player.JumpMath();
	}
}


//============================================================================
//		
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
				player.SetLanding(true);
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
//		
//============================================================================
void CollisionManager::EnemytoFloorCol(
	const std::vector<std::unique_ptr<Enemy>>& enemies,
	StaticMesh& floor)
{
	Collider Floorcol,Enemycol;
	CollisionPoints colpoints;

	Floorcol.SetVertex(floor.GetObjectInfo(), floor.GetVertices());

	for (size_t i = 0; i < enemies.size(); ++i) {
		Enemycol.SetVertex(enemies[i]->GetObjectInfo(), enemies[i]->GetMesh()->GetVertices());

		colpoints = GJK::GJKC(Enemycol, Floorcol);

		if (colpoints.Col)
		{
			enemies[i]->SetLanding(true);
			enemies[i]->CanJump();

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
		else {
			enemies[i]->JumpMath();
		}
	}
}


//============================================================================
//		
//============================================================================
void CollisionManager::EnemytoCylinderCol(
	const std::vector<std::unique_ptr<Enemy>>& enemies,
	const std::vector<std::unique_ptr<StaticMesh>>& cylinders)
{
	for (const auto& cyl : cylinders)
	{
		Collider cylCol, enemyCol;
		cylCol.SetVertex(cyl->GetObjectInfo(), cyl->GetVertices());

		for (const auto& enemy : enemies)
		{
			enemyCol.SetVertex(enemy->GetObjectInfo(), enemy->GetMesh()->GetVertices());
			CollisionPoints points = GJK::GJKC(cylCol, enemyCol);

			if (points.Col)
			{
				if (points.Normal.y > 0.f)
				{
					enemy->SetLanding(true);
					enemy->CanJump();
					D3DXVECTOR3 setPos = enemy->GetPos() - points.Normal * points.Depth;
					enemy->SetPos(setPos);
				}

				D3DXVec3Normalize(&points.Normal, &points.Normal);
				D3DXVECTOR3 moveVec = enemy->GetMoveVec();
				float dot = D3DXVec3Dot(&moveVec, &points.Normal);
				moveVec = moveVec - dot * points.Normal;
				enemy->AddVec(moveVec);
				enemy->SetPos(enemy->GetPos() + points.Normal * points.Depth);

				if (points.Depth < 0.05f)
					enemy->AddVec(-enemy->GetMoveVec());
			}
			else
			{
				enemy->JumpMath();
			}
		}
	}
}


//============================================================================
//		
//============================================================================
void CollisionManager::RaytoObjeCol(
	const std::vector<std::unique_ptr<Enemy>>& enemies,
	StaticMesh& floor, 
	const std::vector<std::unique_ptr<StaticMesh>>& cylinders)
{
	D3DXVECTOR3 camlookpos = Camera::GetInstance()->GetPos() + Camera::GetInstance()->GetCamDir() * 100.f;

	RayInfo sendCamera = { false, camlookpos, 5000.f };

	RayInfo groundRay = floor.IsHitForRay(Camera::GetInstance()->GetRay());

	if (groundRay.Hit && sendCamera.Length > groundRay.Length)
		sendCamera = groundRay;

	for (const auto& cyl : cylinders)
	{
		RayInfo cylRay = cyl->IsHitForRay(Camera::GetInstance()->GetRay());
		if (cylRay.Hit && sendCamera.Length > cylRay.Length)
			sendCamera = cylRay;
	}

	for (const auto& enemy : enemies)
	{
		RayInfo enemyRay = enemy->IsHitForRay(Camera::GetInstance()->GetRay());
		if (enemyRay.Hit && sendCamera.Length > enemyRay.Length)
			sendCamera = enemyRay;
	}

	Camera::GetInstance()->SetRayHit(sendCamera.HitPos);
}
