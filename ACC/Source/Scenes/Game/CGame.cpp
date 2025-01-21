#include "CGame.h"

#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"

#include "Effect/CEffect.h"

#include "Camera/CCamera.h"
#include "Object/Ground/CGround.h"
#include "Character/Player/CPlayer.h"
#include "Character/Enemy/CEnemy.h"
#include "Sprite/2D/UI/CGameUI/CGameUI.h"

#include "Common/Random/CRandom.h"

#if _DEBUG
	#include "ImGui/CImGui.h"
#endif

//============================================================================
//		�Q�[���N���X.
//============================================================================
CGame::CGame(HWND hWnd)
	: m_hWnd		( hWnd )
	, m_mView		()
	, m_mProj		()
	, m_Light		()

	, m_pEgg		( nullptr )
	, m_pFloor		( nullptr )
	, m_pCylinders	()
	
	, m_pPlayer		( nullptr )
	, m_pEnemy		( nullptr )
	, m_pGround		( nullptr )

	, m_pGJK		( nullptr )
	, m_pCamRay		( nullptr )

	, m_Angle		(0.f)
	, m_CylinderMax	(9)
{
	// ���C�g���.
	m_Light.vDirection	= D3DXVECTOR3( 1.5f, 1.f, -1.f );
}

CGame::~CGame()
{
	Release();
}


//============================================================================
//		�\�z.
//============================================================================
void CGame::Create()
{
	// �C���X�^���X����.
	m_pEgg		= std::make_unique<CStaticMesh>();
	m_pFloor	= std::make_unique<CStaticMesh>(); 
	for (int i = 0; i < m_CylinderMax; ++i) {
		auto cylinder = std::make_unique<CStaticMesh>();
		cylinder->Init(_T("Data\\Mesh\\Static\\Stage\\Rectangular.x"));
		m_pCylinders.push_back(std::move(cylinder));
	}
	m_pPlayer	= std::make_unique<CPlayer>();
	m_pEnemy	= std::make_unique<CEnemy>();
	m_pGround	= std::make_unique<CGround>();
	m_pCamRay	= std::make_unique<CRay>();
	m_pGameUI	= std::make_unique<CGameUI>();
	m_pGameUI	->Create();

	// �X�|�[���n�_�̐ݒ�.
	m_SpawnPoints = {
		{  50.f, 5.f,  50.f },
		{ -50.f, 5.f,  50.f },
		{  50.f, 5.f, -50.f },
		{ -50.f, 5.f, -50.f }
	};
}


//============================================================================
//		���[�h�f�[�^�֐�.
//============================================================================
HRESULT CGame::LoadData()
{
	// �X�^�e�B�b�N���b�V���̓ǂݍ���.
	m_pEgg		->Init( _T("Data\\Mesh\\Static\\Player\\egg.x"	));
	m_pFloor	->Init( _T("Data\\Mesh\\Static\\Stage\\Floor.x"	));

	// ���b�V�����A�^�b�`����.
	m_pPlayer	->AttachMesh( *m_pEgg );
	m_pEnemy	->AttachMesh( *m_pEgg );
	m_pGround	->AttachMesh( *m_pFloor );

	// �L�����N�^�[�̏������W��ݒ�.
	m_pPlayer	->SetPos( 60.f, 50.f, 60.f );
	m_pEnemy	->SetPos( 10.f, 1.f, 6.f );

	CRandom random;
	InitEPPos(random, m_pPlayer, m_pEnemy);

	// ���̔z�u(��łǂ��ɂ����鉼�ݒu���@).
	for (int i = 0; i < m_pCylinders.size(); ++i)
	{
		// ���O��.
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
		// ���O��.

		// ����.
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
		// ����.
	}

	// �J�����̏�����.
	CCamera::GetInstance()->Init();

	// �J�����̃��C�����擾.
	m_pCamRay->Init(CCamera::GetInstance()->GetRay());

	return S_OK;
}


//============================================================================
//		����֐�.
//============================================================================
void CGame::Release()
{
	m_pGameUI.reset();
	m_pCamRay.reset();
	m_pGround.reset();
	m_pEnemy.reset();
	m_pPlayer.reset();
	for (auto& cylinder : m_pCylinders) {
		cylinder.reset();
	}
	m_pEgg.reset();
	m_pFloor.reset();

	// �O���ō쐬���Ă���̂ŁA�����Ŕj�����Ȃ�.
	m_hWnd = nullptr;
}


//============================================================================
//		�������֐�.
//============================================================================
void CGame::Init()
{
	CCamera::GetInstance()->Init();	// �J�����̏�����.
}


//============================================================================
//		�X�V����.
//============================================================================
void CGame::Update()
{
	CKey* Key = CDInput::GetInstance()->CDKeyboard();

	// ���̑��ƃJ�������C�̔���(�e�̓����n�_�Ɏg�p����).
	RaytoObjeCol();

	m_pPlayer->Update(); // �v���C���[�̍X�V.
	m_pEnemy->Update();	 // �G�l�~�[�̍X�V.
	CCamera::GetInstance()->Update(); // �J�����̍X�V.


	// ����ɂ͐�΂ɉ��o������i����,�s�k���o).
	// �v���C���[��HP���O�ɂȂ����Ƃ�(�o�O������).
	if( m_pPlayer->GetCharaInfo().HP <= 0 
	||  m_pPlayer->GetPos().y < -100.f) {
		CSceneManager::GetInstance()->LoadScene(SceneList::Title);
	}

	// �G��HP���O�ɂȂ����Ƃ�(�o�O������).
	if( m_pEnemy->GetCharaInfo().HP <= 0
	||  m_pEnemy->GetPos().y < -100.f) {
		CSceneManager::GetInstance()->LoadScene(SceneList::Title);
	}

	CollisionJudge();

	m_pGameUI->Update();

#if _DEBUG
	//-----------------------------------------------------
	//	�L�����N�^�[�E�B���h�E.
	//-----------------------------------------------------
	ImGui::Begin("CharaWindow");
	ImGui::Text("%d", m_pPlayer->GetCharaInfo().HP);
	ImGui::Text("%d", m_pEnemy->GetCharaInfo().HP);
	ImGui::End();


	//-----------------------------------------------------
	//	�v���C���[�E�B���h�E.
	//-----------------------------------------------------
	ImGui::Begin("PlayerWindow");
	D3DXVECTOR3 playerpos = m_pPlayer->GetPos();
	ImGui::Text("%f,%f,%f", playerpos.x, playerpos.y, playerpos.z);
	D3DXVECTOR3 playersumvec = m_pPlayer->GetMoveVec();
	ImGui::Text("%f,%f,%f", playersumvec.x, playersumvec.y, playersumvec.z);
	ImGui::End();


	//-----------------------------------------------------
	//	�e�E�B���h�E.
	//-----------------------------------------------------
	ImGui::Begin("CameraWindow");
	D3DXVECTOR3 camrot = CCamera::GetInstance()->GetRot();
	ImGui::Text("%f,%f,%f", camrot.x, camrot.y, camrot.z);
	ImGui::End();


	// �J�������̃L�[����𖳌��ɂ���.
	if (Key->IsKeyAction(DIK_F2)) { CCamera::GetInstance()->ChangeCanMove(); }
	if (Key->IsKeyAction(DIK_F3)) { CCamera::GetInstance()->ChangeUseMouse(); }
#endif
}


//============================================================================
//		�`�揈��.
//============================================================================
void CGame::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);

	m_pGround->Draw( m_mView, m_mProj, m_Light );
	m_pPlayer->Draw( m_mView, m_mProj, m_Light );
	m_pEnemy->Draw( m_mView, m_mProj, m_Light );

	for (auto& cylinder : m_pCylinders) {
		cylinder->Render(m_mView, m_mProj, m_Light);
	}
	m_pGameUI->Draw();

	// �G�t�F�N�g�̕`��.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
}


//-----------------------------------------------------------------------------
//		�����蔻��֐�.
//-----------------------------------------------------------------------------
void CGame::CollisionJudge()
{
	MeshCollider PlayerEgg, EnemyEgg, Floor;
	std::vector<MeshCollider> Cylinders;

	// ���f�[�^�擾.
	for (int i = 0; i < m_CylinderMax; ++i) {
		MeshCollider cylinder;
		cylinder.SetVertex(
			m_pCylinders[i]->GetPos(),
			m_pCylinders[i]->GetRot(),
			m_pCylinders[i]->GetScale(),
			m_pCylinders[i]->GetVertices()
		);
		Cylinders.push_back(cylinder);
	}

	// �n�ʃf�[�^�擾.
	Floor.SetVertex(
		m_pGround->GetPos(),
		m_pGround->GetRot(),
		m_pGround->GetScale(),
		m_pFloor->GetVertices());

	// �v���C���[�f�[�^�擾.
	PlayerEgg.SetVertex(
		m_pPlayer->GetPos(),
		m_pPlayer->GetRot(),
		m_pPlayer->GetScale(),
		m_pEgg->GetVertices());
	
	// �v���C���[�f�[�^�擾.
	EnemyEgg.SetVertex(
		m_pEnemy->GetPos(),
		m_pEnemy->GetRot(),
		m_pEnemy->GetScale(),
		m_pEgg->GetVertices());


	// �v���C���[�Ə��̔����Ԃ�.
	CollisionPoints pointspef = m_pGJK->GJK(PlayerEgg, Floor);
	// �v���C���[�Ɖ~���̔����Ԃ�.
	std::vector<CollisionPoints> pointspecs;
	for (int i = 0; i < m_CylinderMax; ++i) {
		CollisionPoints pointspec;
		pointspec = m_pGJK->GJK(Cylinders[i], PlayerEgg);
		pointspecs.push_back(pointspec);
	}

	// �G�Ə��̔����Ԃ�.
	CollisionPoints pointseef = m_pGJK->GJK(EnemyEgg, Floor);
	// �G�Ɖ~���̔����Ԃ�.
	std::vector<CollisionPoints> pointseecs;
	for (int i = 0; i < m_CylinderMax; ++i) {
		CollisionPoints pointseec;
		pointseec = m_pGJK->GJK(Cylinders[i], EnemyEgg);
		pointseecs.push_back(pointseec);
	}


	// �v���C���[�Ə��̔��菈��.
	PlayertoFloorCol(pointspef);

	// �v���C���[�ɏd�͂�������.
	m_pPlayer->UseGravity();

	// �v���C���[�ƒ��̔��菈��.
	for (int i = 0; i < m_CylinderMax; ++i) {
		PlayertoCylinderCol(pointspecs[i]);
	}

	// �G�l�~�[�Ə��̔��菈��.
	EnemytoFloorCol(pointseef);

	// �G�l�~�[�ɏd�͂�������.
	m_pEnemy->UseGravity();

	// �G�l�~�[�ƒ��̔��菈��.
	for (int i = 0; i < m_CylinderMax; ++i) {
		EnemytoCylinderCol(pointseecs[i]);
	}

	// �v���C���[�ƓG�̓����蔻�菈��������.
	for (int i = 0; i < m_CylinderMax; ++i) {
		m_pPlayer->Collision(m_pEnemy, Floor, Cylinders[i]);
		m_pEnemy->Collision(m_pPlayer, Floor, Cylinders[i]);
	}
}


//-----------------------------------------------------------------------------
//	�����_���ŃX�|�[��
//-----------------------------------------------------------------------------
void CGame::InitEPPos(CRandom& random, std::unique_ptr<CPlayer>& player, std::unique_ptr<CEnemy>& enemy) {
	// �X�|�[���|�C���g�̃C���f�b�N�X�������_���Ɏ擾.
	int PIndex = random.GetRandomInt(0, m_SpawnPoints.size() - 1);

	// �G�̃X�|�[���|�C���g���v���C���[�ƈقȂ�ꏊ�ɂ���.
	int EIndex;
	do {
		EIndex = random.GetRandomInt(0, m_SpawnPoints.size() - 1);
	} while (EIndex == PIndex);

	// �v���C���[�ƓG�̈ʒu��ݒ�.
	player->SetPos(m_SpawnPoints[PIndex]);
	enemy->SetPos(m_SpawnPoints[EIndex]);
}


//-----------------------------------------------------------------------------
//		�v���C���[�Ə��̓����蔻����܂Ƃ߂�֐�.
//-----------------------------------------------------------------------------
void CGame::PlayertoFloorCol(CollisionPoints points)
{
	// �������Ă����ꍇ.
	if (points.Col)
	{
		m_pPlayer->ResetGravity();	// �v���C���[�ɂ�����d�͂̃��Z�b�g.
		m_pPlayer->CanJump();		// �W�����v���\��.

		// �n�ʂɏՓ˂��Ă���ꍇ.
		if (points.Normal.y < 0.f)
		{
			// �v���C���[�������߂����Ƃ��̍��W���Z�o.
			D3DXVECTOR3 SetPos = m_pPlayer->GetPos() - points.Normal * points.Depth;

			// �v���C���[�̈ʒu�̍X�V.
			m_pPlayer->SetPos(SetPos);
		}
		// �ǂ�ΖʂɏՓ˂��Ă���ꍇ.
		else {
			D3DXVec3Normalize(&points.Normal, &points.Normal); // ���K��.
			D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();  // �v���C���[�̈ړ��x�N�g�����擾.
			
			// �v���C���[�̈ړ��x�N�g���Ɩ@���x�N�g���̓��ς��v�Z.
			float Dot = D3DXVec3Dot(&PlayerMove, &points.Normal);

			// �@�������̈ړ��������������āA�ǂɉ������ړ������݂̂ɂ���.
			PlayerMove = PlayerMove - Dot * points.Normal;

			// �C�������ړ��x�N�g�����v���C���[�ɉ��Z.
			m_pPlayer->AddVec(PlayerMove);
		}
	}
	else {
		m_pPlayer->AddGravity();	// �v���C���[�ɂ�����d�͂𑝂₷.
		m_pPlayer->JumpPowerDec();	// �W�����v�ŉ��Z�����l�����炷.
	}
}


//-----------------------------------------------------------------------------
//		�v���C���[�ƒ��̓����蔻����܂Ƃ߂�֐�.
//-----------------------------------------------------------------------------
void CGame::PlayertoCylinderCol(CollisionPoints points)
{
	if (points.Col) {
		D3DXVec3Normalize(&points.Normal, &points.Normal); // �@���𐳋K��.
		D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();  // �v���C���[�̈ړ��x�N�g�����擾.

		// �v���C���[�̈ړ��x�N�g���Ɩ@���x�N�g���̓��ς��v�Z.
		float Dot = D3DXVec3Dot(&PlayerMove, &points.Normal);
		// �@�������̈ړ��������������āA�ǂɉ������ړ������݂̂ɂ���.
		PlayerMove = PlayerMove - Dot * points.Normal;

		// �C�������ړ��x�N�g�����v���C���[�ɓK�p.
		m_pPlayer->AddVec(PlayerMove);
		m_pPlayer->SetPos(m_pPlayer->GetPos() + points.Normal * points.Depth);

		if (points.Depth < 0.05f) {
			m_pPlayer->AddVec(-m_pPlayer->GetMoveVec());
		}
	}
}


//-----------------------------------------------------------------------------
//		�G�Ə��̓����蔻����܂Ƃ߂�֐�.
//-----------------------------------------------------------------------------
void CGame::EnemytoFloorCol(CollisionPoints points)
{
	// �������Ă����ꍇ.
	if (points.Col)
	{
		m_pEnemy->ResetGravity();	// �v���C���[�ɂ�����d�͂̃��Z�b�g.
		m_pEnemy->CanJump();		// �W�����v���\��.

		// �n�ʂɏՓ˂��Ă���ꍇ.
		if (points.Normal.y < 0.f)
		{
			// �v���C���[�������߂����Ƃ��̍��W���Z�o.
			D3DXVECTOR3 SetPos = m_pEnemy->GetPos() - points.Normal * points.Depth;

			// �v���C���[�̈ʒu�̍X�V.
			m_pEnemy->SetPos(SetPos);
		}
		// �ǂ�ΖʂɏՓ˂��Ă���ꍇ.
		else {
			D3DXVec3Normalize(&points.Normal, &points.Normal); // ���K��.
			D3DXVECTOR3 PlayerMove = m_pEnemy->GetMoveVec();   // �v���C���[�̈ړ��x�N�g�����擾.
			
			// �v���C���[�̈ړ��x�N�g���Ɩ@���x�N�g���̓��ς��v�Z.
			float Dot = D3DXVec3Dot(&PlayerMove, &points.Normal);

			// �@�������̈ړ��������������āA�ǂɉ������ړ������݂̂ɂ���.
			PlayerMove = PlayerMove - Dot * points.Normal;

			// �C�������ړ��x�N�g�����v���C���[�ɉ��Z.
			m_pEnemy->AddVec(PlayerMove);
		}
	}
	else {
		m_pEnemy->AddGravity();	 // �v���C���[�ɂ�����d�͂𑝂₷.
		m_pEnemy->JumpPowerDec();// �W�����v�ŉ��Z�����l�����炷.
	}
}


//-----------------------------------------------------------------------------
//		�G�ƒ��̓����蔻����܂Ƃ߂�֐�.
//-----------------------------------------------------------------------------
void CGame::EnemytoCylinderCol(CollisionPoints points)
{
	if (points.Col) {
		D3DXVec3Normalize(&points.Normal, &points.Normal); // �@���𐳋K��.
		D3DXVECTOR3 PlayerMove = m_pEnemy->GetMoveVec();  // �v���C���[�̈ړ��x�N�g�����擾.

		// �v���C���[�̈ړ��x�N�g���Ɩ@���x�N�g���̓��ς��v�Z.
		float Dot = D3DXVec3Dot(&PlayerMove, &points.Normal);
		// �@�������̈ړ��������������āA�ǂɉ������ړ������݂̂ɂ���.
		PlayerMove = PlayerMove - Dot * points.Normal;

		// �C�������ړ��x�N�g�����v���C���[�ɓK�p.
		m_pEnemy->AddVec(PlayerMove);
		m_pEnemy->SetPos(m_pPlayer->GetPos() + points.Normal * points.Depth);

		if (points.Depth < 0.05f) {
			m_pEnemy->AddVec(-m_pEnemy->GetMoveVec());
		}
	}
}


//-----------------------------------------------------------------------------
//		���C�̓����蔻����܂Ƃ߂�֐�.
//-----------------------------------------------------------------------------
void CGame::RaytoObjeCol()
{
	// ���C���p�̕ϐ�.
	RayInfo SendCamera, GroundRay, EnemyRay;
	std::vector<RayInfo> CylinderRays;

	D3DXVECTOR3 camlookpos = CCamera::GetInstance()->GetPos() + CCamera::GetInstance()->GetCamDir() * 100.f;
	SendCamera = { false, camlookpos, 5000.f };

	// �J�������C�Ɗe�I�u�W�F���Ƃ̔�������擾.
	GroundRay	= m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	for (int i = 0; i < m_CylinderMax; ++i) {
		RayInfo CylinderRay;
		CylinderRay = m_pCylinders[i]->IsHitForRay(CCamera::GetInstance()->GetRay());
		CylinderRays.push_back(CylinderRay);
	}
	EnemyRay = m_pEnemy->IsHitForRay(CCamera::GetInstance()->GetRay());


	// �ǂ̃I�u�W�F���ł��߂�����T��.
	if (GroundRay.Hit) {
		if (SendCamera.Length > GroundRay.Length) {
			SendCamera = GroundRay;
		}
	}
	if (EnemyRay.Hit) {
		if (SendCamera.Length > EnemyRay.Length) {
			SendCamera = EnemyRay;
		}
	}
	for (int i = 0; i < m_CylinderMax; ++i) {
		if (CylinderRays[i].Hit) {
			if (SendCamera.Length > CylinderRays[i].Length) {
				SendCamera = CylinderRays[i];
			}
		}
	}

	// �ŏI�I�ɍŒZ�����ɂ��镨�̂����������W��n��.
	CCamera::GetInstance()->SetRayHit(SendCamera.HitPos);
}