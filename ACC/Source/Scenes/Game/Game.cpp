#include "Game.h"

#include "DirectSound/SoundManager.h"
#include "Scenes/SceneManager/SceneManager.h"
#include "DirectInput/DirectInput.h"

#include "Effect/Effect.h"

#include "Collision/Line/MeshLine.h"
#include "Camera/Camera.h"
#include "Character/Player/Player.h"
#include "Character/Enemy/Enemy.h"
#include "Sprite/2D/UI/GameUI/GameUI.h"
#include "Sprite/2D/UI/DefeatUI/DefeatUI.h"
#include "Sprite/2D/UI/VictoryUI/VictoryUI.h"
#include "Random/Random.h"
#include "Easing/Easing.h"

#include "Collision/CollisionManager.h"
#include "ImGui/CImGui.h"

//============================================================================
//		ゲームクラス
//============================================================================
Game::Game(HWND hWnd)
	: m_hWnd				( hWnd )
	, m_mView				()
	, m_mProj				()
	, m_Light				()

	, m_pEgg				( nullptr )
	, m_pFloor				( nullptr )
	, m_pCylinders			()
	
	, m_pPlayer				( nullptr )
	, m_EnemyCount			( 3 )
	, m_pEnemies			( )

	, m_pCamRay				( nullptr )
	, m_pMeshLine			( nullptr )

	, m_pWinUI				( nullptr )
	, m_pDefeatUI			( nullptr )

	, m_HitKind				( 0 )
	, m_CylinderMax			( 9 )
	, m_SlowMode			( false )
	, m_SlowScalingTime		( 0.f )
	, m_SlowScalingTimeMax	( Time::GetDeltaTime() * 60.f )
	, m_SlowScale			( 1.f )
{
	// ライト情報
	m_Light.vDirection	= D3DXVECTOR3( 1.5f, 1.f, -1.f );
}
Game::~Game()
{
	Release();
}


//============================================================================
//		構築
//============================================================================
void Game::Create()
{
	// インスタンス生成
	m_pEgg			= std::make_unique<StaticMesh>();
	m_pFloor		= std::make_unique<StaticMesh>();
	m_pPlayer		= std::make_unique<Player>();
	for (int i = 0; i < m_EnemyCount;++i) { m_pEnemies.push_back(std::move(std::make_unique<Enemy>())); }
	m_pCamRay		= std::make_unique<Ray>();
	m_pMeshLine		= std::make_unique<MeshLine>();
	m_pGameUI		= std::make_unique<GameUI>();
	m_pGameUI		->Create();

	// スポーン地点の設定
	m_SpawnPoints = {
		{  50.f, 5.f,  50.f },
		{ -50.f, 5.f,  50.f },
		{  50.f, 5.f, -50.f },
		{ -50.f, 5.f, -50.f }
	};
	
	// 柱の生成
	for (int i = 0; i < m_CylinderMax; ++i) {
		auto cylinder = std::make_unique<StaticMesh>();
		if (i < 4) {
			cylinder->Init(_T("Data\\Mesh\\Static\\Stage\\Rectangular.x"));
		}
		else {
			cylinder->Init(_T("Data\\Mesh\\Static\\Stage\\Rectangular2.x"));
		}
		m_pCylinders.push_back(std::move(cylinder));
	}
}


//============================================================================
//		データ読込
//============================================================================
HRESULT Game::LoadData()
{
	// スタティックメッシュの読み込み
	m_pEgg		->Init( _T("Data\\Mesh\\Static\\Player\\egg.x"	));
	m_pFloor	->Init( _T("Data\\Mesh\\Static\\Stage\\Floor.x"	));

	// メッシュをアタッチする
	m_pPlayer	->AttachMesh( *m_pEgg );
	for (int i = 0; i < m_pEnemies.size();++i) { m_pEnemies[i]->AttachMesh(*m_pEgg); }
	;

	// キャラクターの初期座標を設定
	D3DXVECTOR3 scale = D3DXVECTOR3(2.f, 2.f, 2.f);
	m_pPlayer	->SetScale(scale);
	for (int i = 0; i < m_pEnemies.size();++i) { m_pEnemies[i]->SetScale(scale); }
	;

	// プレイヤーと敵のスポーン位置をランダムに決める
	Random random;
	InitEPPos(random, m_pPlayer, m_pEnemies);

	// 柱の配置(後でどうにかする仮設置方法)
	for (int i = 0; i < m_pCylinders.size(); ++i)
	{
		// ↓外壁
		if (i == 0) {
			m_pCylinders[i]->SetPos(0.f, -10.f, 103.f);
			m_pCylinders[i]->SetScale(50.f, 0.5f, 1.f);
		}
		if (i == 1) {
			m_pCylinders[i]->SetPos(0.f, -10.f, -103.f);
			m_pCylinders[i]->SetScale(50.f, 0.5f, 1.f);
		}
		if (i == 2) {
			m_pCylinders[i]->SetPos(103.f, -10.f, 0.f);
			m_pCylinders[i]->SetScale(1.f, 0.5f, 50.f);
		}
		if (i == 3) {
			m_pCylinders[i]->SetPos(-103.f, -10.f, 0.f);
			m_pCylinders[i]->SetScale(1.f, 0.5f, 50.f);
		}
		// ↑外壁

		// ↓柱
		if (i == 4) {
			m_pCylinders[i]->SetPos(0.f, -0.3f, 0.f);
			m_pCylinders[i]->SetScale(5.f, 0.5f, 5.f);
		}
		if (i == 5) {
			m_pCylinders[i]->SetPos(51.5f, -0.3f, 0.f);
			m_pCylinders[i]->SetScale(5.f, 0.5f, 5.f);
		}
		if (i == 6) {
			m_pCylinders[i]->SetPos(-51.5f, -0.3f, 0.f);
			m_pCylinders[i]->SetScale(5.f, 0.5f, 5.f);
		}
		if (i == 7) {
			m_pCylinders[i]->SetPos(0.f, -0.3f, 51.5f);
			m_pCylinders[i]->SetScale(5.f, 0.5f, 5.f);
		}
		if (i == 8) {
			m_pCylinders[i]->SetPos(0.f, -0.3f, -51.5f);
			m_pCylinders[i]->SetScale(5.f, 0.5f, 5.f);
		}
		// ↑柱
	}

	// カメラの初期化
	Camera::GetInstance()->Init();

	// カメラのレイ情報を取得
	m_pCamRay->Init(Camera::GetInstance()->GetRay());
	m_pMeshLine->Init();

	return S_OK;
}


//============================================================================
//		解放関数
//============================================================================
void Game::Release()
{
	m_pGameUI.reset();
	m_pCamRay.reset();
	m_pEnemies.clear();
	m_pPlayer.reset();
	for (auto& cylinder : m_pCylinders) {
		cylinder.reset();
	}
	m_pEgg.reset();
	m_pFloor.reset();

	// 外部で作成しているので、ここで破棄しない
	m_hWnd = nullptr;
}


//============================================================================
//		初期化関数
//============================================================================
void Game::Init()
{
	Camera::GetInstance()->Init();	// カメラの初期化
}


//============================================================================
//		更新処理
//============================================================================
void Game::Update()
{
	// BGMを再生
	SoundManager::GetInstance()->PlayLoop(SoundManager::enList::BGM_Game);

	Key* Key = DirectInput::GetInstance()->CDKeyboard();

	// UIの更新処理
	UIUpdate();

	// 勝利や敗北画面が出現していない間
	if (m_pDefeatUI == nullptr && m_pWinUI == nullptr)
	{
		float minDistSq = FLT_MAX; // 一番小さい距離の初期値
		for (size_t i = 0; i < m_pEnemies.size();++i) {
			m_pEnemies[i]->Update(m_pPlayer); // エネミーの更新

			const D3DXVECTOR3& enemyPos = m_pEnemies[i]->GetPos();
			D3DXVECTOR3 diff = enemyPos - m_pPlayer->GetPos();
			float distSq = D3DXVec3LengthSq(&diff); // 距離の2乗

			if (distSq < minDistSq) {
				minDistSq = distSq;
				m_NearEnemyIndex = static_cast<int>(i);
			}
		}
		m_pPlayer->Update(m_pEnemies[m_NearEnemyIndex]); // プレイヤーの更新

		Camera::GetInstance()->Update(); // カメラの更新

		// プレイヤーがオートエイムを使用していた場合
		if (m_pPlayer->GetAutoAim()) { AutoAimProcess();}

		// 当たり判定処理
		m_pCollisionManager->ColJudge(
			*m_pPlayer,
			m_pEnemies,
			*m_pFloor,
			m_pCylinders
		);

		// 敵の死亡削除処理
		for (auto it = m_pEnemies.begin(); it != m_pEnemies.end(); )
		{
			if ((*it)->GetCharaInfo().HP <= 0 
				//|| (*it)->GetPos().y < -1000.f
				)
			{
				// eraseで要素ごと消す（unique_ptrが自動でdelete）
				it = m_pEnemies.erase(it);
			}
			else {
				++it;
			}
		}

		// UIの更新処理
		m_pGameUI->Update(m_pPlayer);
	}

	// カメラ側のキー操作を無効にする
	if (Key->IsKeyAction(DIK_F3)) { Camera::GetInstance()->ChangeUseMouse(); }
	if (Key->IsKeyAction(DIK_5)) { 
		m_SlowMode = !m_SlowMode;
		m_SlowScalingTime = 0;
	}
	m_pGameUI->SetSlowUI(m_SlowMode);
	if (m_SlowScalingTime < m_SlowScalingTimeMax) { m_SlowScalingTime += Time::GetDeltaTime(); }
	if (m_SlowMode) {
		m_SlowScale = MyEasing::OutCirc(m_SlowScalingTime, m_SlowScalingTimeMax, m_SlowScale, 0.1f);
	}
	else {
		m_SlowScale = MyEasing::OutCirc(m_SlowScalingTime, m_SlowScalingTimeMax, m_SlowScale, 1.f);
	}

	Time::GetInstance()->SetTimeScale(m_SlowScale);
	if (m_pEnemies.empty()) { return; }
	ImGui::Begin("enemies window");
	for (size_t i = 0; i < m_pEnemies.size(); ++i) {
		// ユニークラベル作成
		char label[32];
		sprintf_s(label, "Enemypos %zu", i);
		D3DXVECTOR3 pos = m_pEnemies[i]->GetPos();
		ImGui::DragFloat3(label, &pos.x, 0.1f);
	}
	ImGui::End();
}


//============================================================================
//		描画処理
//============================================================================
void Game::Draw()
{
	Camera::GetInstance()->CameraView(m_mView);
	SceneBase::Projection(m_mProj);

	D3DXVECTOR4 color = { 0.f,1.f,0.f,1.f };

	for (auto& cylinder : m_pCylinders) { cylinder->Draw(m_mView, m_mProj, m_Light); }
	m_pFloor->Draw(m_mView, m_mProj, m_Light);
	for (size_t i = 0; i < m_pEnemies.size();++i) {
		m_pEnemies[i]->Draw(m_mView, m_mProj, m_Light);
	}

	// ウォールハック起動中は敵のフレームを描画
	if (m_pPlayer->GetWallHack()) {
		DirectX11::GetInstance()->SetDepth(false);
		for (size_t i = 0; i < m_pEnemies.size();++i) {
			m_pMeshLine->DrawMeshWireframeFromVertices(*m_pEnemies[i]->GetMesh(), *m_pEnemies[i], m_mView, m_mProj, color, 1.f);
		}
		m_pMeshLine->Render(m_mView, m_mProj);
		DirectX11::GetInstance()->SetDepth(true);
	}
	m_pPlayer->Draw(m_mView, m_mProj, m_Light);	// プレイヤーの描画

	// エフェクトの描画
	Effect::GetInstance()->Draw(m_mView, m_mProj, m_Light);
	
	// UIの描画
	m_pGameUI->Draw();

	// 勝利と敗北画面の描画
	if (m_pDefeatUI != nullptr) { m_pDefeatUI->Draw(); }
	if (m_pWinUI != nullptr) { m_pWinUI->Draw(); }
}


//-----------------------------------------------------------------------------
//		敵とプレイヤーをランダムでスポーン
//-----------------------------------------------------------------------------
void Game::InitEPPos(Random& random,
	std::unique_ptr<Player>& player,
	std::vector<std::unique_ptr<Enemy>>& enemy)
{
	// スポーンポイントのインデックスをランダムに取得
	int PIndex = random.GetRandomInt(0, static_cast<int>(m_SpawnPoints.size()) - 1);
	// プレイヤー位置を設定
	player->SetPos(m_SpawnPoints[PIndex]);

	// 敵のスポーンポイントをプレイヤーと異なる場所にする
	int EIndex, countIndex;
	for (int i = 0;i < m_pEnemies.size(); ++i) {
		do {
			EIndex = random.GetRandomInt(0, static_cast<int> (m_SpawnPoints.size()) - 1);
		} while (EIndex == PIndex);

		enemy[i]->SetPos(m_SpawnPoints[EIndex]);
	}
}


//-----------------------------------------------------------------------------
//		UI処理をまとめる関数
//-----------------------------------------------------------------------------
void Game::UIUpdate()
{
	// プレイヤーがHP０の場合、もしくは地面抜けしていた場合、勝利UIを作成
	if (m_pDefeatUI == nullptr
	&& (m_pPlayer->GetCharaInfo().HP <= 0|| m_pPlayer->GetPos().y < -100.f)) 
	{
		// 敗北UIの作成
		m_pDefeatUI = std::make_unique<DefeatUI>();
		m_pDefeatUI->Create();

		// 敗北時の効果音を鳴らす
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_Lose);
	}

	// 敵がHP０の場合、もしくは地面抜けしていた場合、勝利UIを作成
	if (m_pWinUI == nullptr 
	&& m_pEnemies.size() == 0) 
	{
		// 勝利UIの作成
		m_pWinUI = std::make_unique<VictoryUI>();
		m_pWinUI->Create();

		// 勝利時の効果音を鳴らす
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_Win);
	}
	
	// 勝利や敗北画面の更新処理
	if ( m_pDefeatUI != nullptr ) { m_pDefeatUI->Update(); }
	if ( m_pWinUI  != nullptr ) { m_pWinUI->Update(); }
}


//-----------------------------------------------------------------------------
//		オートエイム時のカメラ処理
//-----------------------------------------------------------------------------
void Game::AutoAimProcess()
{
	Camera::GetInstance()->CursorInit();
	D3DXVECTOR3 camPos = Camera::GetInstance()->GetPos();
	D3DXVECTOR3 enemyPos = m_pEnemies[m_NearEnemyIndex]->GetPos();
	enemyPos.y += 1.f;
	D3DXVECTOR3 toEnemy = enemyPos - camPos;
	D3DXVECTOR3 camRot = ZEROVEC3;
	D3DXVec3Normalize(&toEnemy, &toEnemy);

	float yaw = atan2f(toEnemy.z, toEnemy.x);
	float pitch = asinf(toEnemy.y);

	camRot.x = D3DXToDegree(pitch);
	camRot.y = D3DXToDegree(yaw);
	Camera::GetInstance()->SetRot(camRot);

	// カメラ向き更新
	D3DXVECTOR3 lookDir;
	lookDir.x = cosf(D3DXToRadian(camRot.y)) * cosf(D3DXToRadian(camRot.x));
	lookDir.y = sinf(D3DXToRadian(camRot.x));
	lookDir.z = sinf(D3DXToRadian(camRot.y)) * cosf(D3DXToRadian(camRot.x));
	D3DXVec3Normalize(&lookDir, &lookDir);

	// 最終的な注視点
	Camera::GetInstance()->SetLook(camPos + lookDir);
}
