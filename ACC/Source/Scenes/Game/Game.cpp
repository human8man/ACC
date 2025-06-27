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
//		�Q�[���N���X
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
	// ���C�g���
	m_Light.vDirection	= D3DXVECTOR3( 1.5f, 1.f, -1.f );
}
Game::~Game()
{
	Release();
}


//============================================================================
//		�\�z
//============================================================================
void Game::Create()
{
	// �C���X�^���X����
	m_pEgg			= std::make_unique<StaticMesh>();
	m_pFloor		= std::make_unique<StaticMesh>();
	m_pPlayer		= std::make_unique<Player>();
	for (int i = 0; i < m_EnemyCount;++i) { m_pEnemies.push_back(std::move(std::make_unique<Enemy>())); }
	m_pCamRay		= std::make_unique<Ray>();
	m_pMeshLine		= std::make_unique<MeshLine>();
	m_pGameUI		= std::make_unique<GameUI>();
	m_pGameUI		->Create();

	// �X�|�[���n�_�̐ݒ�
	m_SpawnPoints = {
		{  50.f, 5.f,  50.f },
		{ -50.f, 5.f,  50.f },
		{  50.f, 5.f, -50.f },
		{ -50.f, 5.f, -50.f }
	};
	
	// ���̐���
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
//		�f�[�^�Ǎ�
//============================================================================
HRESULT Game::LoadData()
{
	// �X�^�e�B�b�N���b�V���̓ǂݍ���
	m_pEgg		->Init( _T("Data\\Mesh\\Static\\Player\\egg.x"	));
	m_pFloor	->Init( _T("Data\\Mesh\\Static\\Stage\\Floor.x"	));

	// ���b�V�����A�^�b�`����
	m_pPlayer	->AttachMesh( *m_pEgg );
	for (int i = 0; i < m_pEnemies.size();++i) { m_pEnemies[i]->AttachMesh(*m_pEgg); }
	;

	// �L�����N�^�[�̏������W��ݒ�
	D3DXVECTOR3 scale = D3DXVECTOR3(2.f, 2.f, 2.f);
	m_pPlayer	->SetScale(scale);
	for (int i = 0; i < m_pEnemies.size();++i) { m_pEnemies[i]->SetScale(scale); }
	;

	// �v���C���[�ƓG�̃X�|�[���ʒu�������_���Ɍ��߂�
	Random random;
	InitEPPos(random, m_pPlayer, m_pEnemies);

	// ���̔z�u(��łǂ��ɂ����鉼�ݒu���@)
	for (int i = 0; i < m_pCylinders.size(); ++i)
	{
		// ���O��
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
		// ���O��

		// ����
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
		// ����
	}

	// �J�����̏�����
	Camera::GetInstance()->Init();

	// �J�����̃��C�����擾
	m_pCamRay->Init(Camera::GetInstance()->GetRay());
	m_pMeshLine->Init();

	return S_OK;
}


//============================================================================
//		����֐�
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

	// �O���ō쐬���Ă���̂ŁA�����Ŕj�����Ȃ�
	m_hWnd = nullptr;
}


//============================================================================
//		�������֐�
//============================================================================
void Game::Init()
{
	Camera::GetInstance()->Init();	// �J�����̏�����
}


//============================================================================
//		�X�V����
//============================================================================
void Game::Update()
{
	// BGM���Đ�
	SoundManager::GetInstance()->PlayLoop(SoundManager::enList::BGM_Game);

	Key* Key = DirectInput::GetInstance()->CDKeyboard();

	// UI�̍X�V����
	UIUpdate();

	// ������s�k��ʂ��o�����Ă��Ȃ���
	if (m_pDefeatUI == nullptr && m_pWinUI == nullptr)
	{
		float minDistSq = FLT_MAX; // ��ԏ����������̏����l
		for (size_t i = 0; i < m_pEnemies.size();++i) {
			m_pEnemies[i]->Update(m_pPlayer); // �G�l�~�[�̍X�V

			const D3DXVECTOR3& enemyPos = m_pEnemies[i]->GetPos();
			D3DXVECTOR3 diff = enemyPos - m_pPlayer->GetPos();
			float distSq = D3DXVec3LengthSq(&diff); // ������2��

			if (distSq < minDistSq) {
				minDistSq = distSq;
				m_NearEnemyIndex = static_cast<int>(i);
			}
		}
		m_pPlayer->Update(m_pEnemies[m_NearEnemyIndex]); // �v���C���[�̍X�V

		Camera::GetInstance()->Update(); // �J�����̍X�V

		// �v���C���[���I�[�g�G�C�����g�p���Ă����ꍇ
		if (m_pPlayer->GetAutoAim()) { AutoAimProcess();}

		// �����蔻�菈��
		m_pCollisionManager->ColJudge(
			*m_pPlayer,
			m_pEnemies,
			*m_pFloor,
			m_pCylinders
		);

		// �G�̎��S�폜����
		for (auto it = m_pEnemies.begin(); it != m_pEnemies.end(); )
		{
			if ((*it)->GetCharaInfo().HP <= 0 
				//|| (*it)->GetPos().y < -1000.f
				)
			{
				// erase�ŗv�f���Ə����iunique_ptr��������delete�j
				it = m_pEnemies.erase(it);
			}
			else {
				++it;
			}
		}

		// UI�̍X�V����
		m_pGameUI->Update(m_pPlayer);
	}

	// �J�������̃L�[����𖳌��ɂ���
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
		// ���j�[�N���x���쐬
		char label[32];
		sprintf_s(label, "Enemypos %zu", i);
		D3DXVECTOR3 pos = m_pEnemies[i]->GetPos();
		ImGui::DragFloat3(label, &pos.x, 0.1f);
	}
	ImGui::End();
}


//============================================================================
//		�`�揈��
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

	// �E�H�[���n�b�N�N�����͓G�̃t���[����`��
	if (m_pPlayer->GetWallHack()) {
		DirectX11::GetInstance()->SetDepth(false);
		for (size_t i = 0; i < m_pEnemies.size();++i) {
			m_pMeshLine->DrawMeshWireframeFromVertices(*m_pEnemies[i]->GetMesh(), *m_pEnemies[i], m_mView, m_mProj, color, 1.f);
		}
		m_pMeshLine->Render(m_mView, m_mProj);
		DirectX11::GetInstance()->SetDepth(true);
	}
	m_pPlayer->Draw(m_mView, m_mProj, m_Light);	// �v���C���[�̕`��

	// �G�t�F�N�g�̕`��
	Effect::GetInstance()->Draw(m_mView, m_mProj, m_Light);
	
	// UI�̕`��
	m_pGameUI->Draw();

	// �����Ɣs�k��ʂ̕`��
	if (m_pDefeatUI != nullptr) { m_pDefeatUI->Draw(); }
	if (m_pWinUI != nullptr) { m_pWinUI->Draw(); }
}


//-----------------------------------------------------------------------------
//		�G�ƃv���C���[�������_���ŃX�|�[��
//-----------------------------------------------------------------------------
void Game::InitEPPos(Random& random,
	std::unique_ptr<Player>& player,
	std::vector<std::unique_ptr<Enemy>>& enemy)
{
	// �X�|�[���|�C���g�̃C���f�b�N�X�������_���Ɏ擾
	int PIndex = random.GetRandomInt(0, static_cast<int>(m_SpawnPoints.size()) - 1);
	// �v���C���[�ʒu��ݒ�
	player->SetPos(m_SpawnPoints[PIndex]);

	// �G�̃X�|�[���|�C���g���v���C���[�ƈقȂ�ꏊ�ɂ���
	int EIndex, countIndex;
	for (int i = 0;i < m_pEnemies.size(); ++i) {
		do {
			EIndex = random.GetRandomInt(0, static_cast<int> (m_SpawnPoints.size()) - 1);
		} while (EIndex == PIndex);

		enemy[i]->SetPos(m_SpawnPoints[EIndex]);
	}
}


//-----------------------------------------------------------------------------
//		UI�������܂Ƃ߂�֐�
//-----------------------------------------------------------------------------
void Game::UIUpdate()
{
	// �v���C���[��HP�O�̏ꍇ�A�������͒n�ʔ������Ă����ꍇ�A����UI���쐬
	if (m_pDefeatUI == nullptr
	&& (m_pPlayer->GetCharaInfo().HP <= 0|| m_pPlayer->GetPos().y < -100.f)) 
	{
		// �s�kUI�̍쐬
		m_pDefeatUI = std::make_unique<DefeatUI>();
		m_pDefeatUI->Create();

		// �s�k���̌��ʉ���炷
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_Lose);
	}

	// �G��HP�O�̏ꍇ�A�������͒n�ʔ������Ă����ꍇ�A����UI���쐬
	if (m_pWinUI == nullptr 
	&& m_pEnemies.size() == 0) 
	{
		// ����UI�̍쐬
		m_pWinUI = std::make_unique<VictoryUI>();
		m_pWinUI->Create();

		// �������̌��ʉ���炷
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_Win);
	}
	
	// ������s�k��ʂ̍X�V����
	if ( m_pDefeatUI != nullptr ) { m_pDefeatUI->Update(); }
	if ( m_pWinUI  != nullptr ) { m_pWinUI->Update(); }
}


//-----------------------------------------------------------------------------
//		�I�[�g�G�C�����̃J��������
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

	// �J���������X�V
	D3DXVECTOR3 lookDir;
	lookDir.x = cosf(D3DXToRadian(camRot.y)) * cosf(D3DXToRadian(camRot.x));
	lookDir.y = sinf(D3DXToRadian(camRot.x));
	lookDir.z = sinf(D3DXToRadian(camRot.y)) * cosf(D3DXToRadian(camRot.x));
	D3DXVec3Normalize(&lookDir, &lookDir);

	// �ŏI�I�Ȓ����_
	Camera::GetInstance()->SetLook(camPos + lookDir);
}
