#include "CGame.h"

#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"

#include "Effect/CEffect.h"

#include "Camera/CCamera.h"
#include "Character/Player/CPlayer.h"
#include "Character/Enemy/CEnemy.h"
#include "Sprite/2D/UI/CGameUI/CGameUI.h"
#include "Sprite/2D/UI/CLoseUI/CLoseUI.h"
#include "Sprite/2D/UI/CWinUI/CWinUI.h"
#include "Random/CRandom.h"


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

	, m_pGJK		( nullptr )
	, m_pCamRay		( nullptr )

	, m_pWinUI		( nullptr )
	, m_pLoseUI		( nullptr )

	, m_HitKind		( 0 )
	, m_CylinderMax	( 9 )
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
	m_pEgg			= std::make_unique<CStaticMesh>();
	m_pFloor		= std::make_unique<CStaticMesh>();
	m_pPlayer		= std::make_unique<CPlayer>();
	m_pEnemy		= std::make_unique<CEnemy>();
	m_pCamRay		= std::make_unique<CRay>();
	m_pGameUI		= std::make_unique<CGameUI>();
	m_pGameUI		->Create();

	// �X�|�[���n�_�̐ݒ�.
	m_SpawnPoints = {
		{  50.f, 5.f,  50.f },
		{ -50.f, 5.f,  50.f },
		{  50.f, 5.f, -50.f },
		{ -50.f, 5.f, -50.f }
	};
	
	// ���̐���.
	for (int i = 0; i < m_CylinderMax; ++i) {
		auto cylinder = std::make_unique<CStaticMesh>();
		cylinder->Init(_T("Data\\Mesh\\Static\\Stage\\Rectangular.x"));
		m_pCylinders.push_back(std::move(cylinder));
	}
}


//============================================================================
//		�f�[�^�Ǎ�.
//============================================================================
HRESULT CGame::LoadData()
{
	// �X�^�e�B�b�N���b�V���̓ǂݍ���.
	m_pEgg		->Init( _T("Data\\Mesh\\Static\\Player\\egg.x"	));
	m_pFloor	->Init( _T("Data\\Mesh\\Static\\Stage\\Floor.x"	));

	// ���b�V�����A�^�b�`����.
	m_pPlayer	->AttachMesh( *m_pEgg );
	m_pEnemy	->AttachMesh( *m_pEgg );

	// �L�����N�^�[�̏������W��ݒ�.
	m_pPlayer	->SetScale( 2.f, 2.f, 2.f );
	m_pEnemy	->SetScale( 2.f, 2.f, 2.f );

	// �v���C���[�ƓG�̃X�|�[���ʒu�������_���Ɍ��߂�.
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
	// BGM���Đ�.
	CSoundManager::GetInstance()->PlayLoop(CSoundManager::enList::BGM_Game);

	CKey* Key = CInput::GetInstance()->CDKeyboard();

	// UI�̍X�V����.
	UIUpdate();

	// ������s�k��ʂ��o�����Ă��Ȃ���.
	if (m_pLoseUI == nullptr && m_pWinUI == nullptr)
	{
		// ���̑��ƃJ�������C�̔���(�e�̓����n�_�Ɏg�p����).
		RaytoObjeCol();

		m_pPlayer->Update(m_pEnemy);		// �v���C���[�̍X�V.
		m_pEnemy->Update(m_pPlayer);		// �G�l�~�[�̍X�V.
		CCamera::GetInstance()->Update();	// �J�����̍X�V.

		// �v���C���[���I�[�g�G�C�����g�p���Ă����ꍇ.
		if (m_pPlayer->GetAutoAim()) {
			CCamera::GetInstance()->SetLook(m_pEnemy->GetPos());
		}

		// �����蔻�菈��.
		CollisionJudge();

		// UI�̍X�V����.
		m_pGameUI->Update(m_pPlayer);
	}

	// �J�������̃L�[����𖳌��ɂ���.
	if (Key->IsKeyAction(DIK_F3)) { CCamera::GetInstance()->ChangeUseMouse(); }
}


//============================================================================
//		�`�揈��.
//============================================================================
void CGame::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);

	m_pFloor	->Render( m_mView, m_mProj, m_Light );	// �n�ʂ̕`��.
	m_pPlayer	->Draw( m_mView, m_mProj, m_Light );	// �v���C���[�̕`��.
	m_pEnemy	->Draw( m_mView, m_mProj, m_Light );	// �G�̕`��.

	// ���̕`��.
	for (auto& cylinder : m_pCylinders) {
		cylinder->Render(m_mView, m_mProj, m_Light);
	}

	// �G�t�F�N�g�̕`��.
	CEffect::GetInstance()->Draw(m_mView, m_mProj, m_Light);

	// UI�̕`��.
	m_pGameUI->Draw();


	// �����Ɣs�k��ʂ̕`��.
	if (m_pLoseUI != nullptr) { m_pLoseUI->Draw(); }
	if (m_pWinUI != nullptr) { m_pWinUI->Draw(); }
}


//-----------------------------------------------------------------------------
//		�����蔻��֐�.
//-----------------------------------------------------------------------------
void CGame::CollisionJudge()
{
	Collider PlayerEgg, EnemyEgg, Floor, Cylinder;
	std::vector<Collider> Cylinders;

	// ���f�[�^�擾.
	for (int i = 0; i < m_CylinderMax; ++i) {
		Cylinder.SetVertex( m_pCylinders[i]->GetObjeInfo(), m_pCylinders[i]->GetVertices());
		Cylinders.push_back(Cylinder);
	}

	// �n��,�v���C���[,�G�̃f�[�^�擾.
	Floor		.SetVertex( m_pFloor->GetObjeInfo(),	m_pFloor->GetVertices());
	PlayerEgg	.SetVertex( m_pPlayer->GetObjeInfo(),	m_pEgg->GetVertices());
	EnemyEgg	.SetVertex( m_pEnemy->GetObjeInfo(),	m_pEgg->GetVertices());

	// �v���C���[�ƓG�̒�������擾�p�̕ϐ���p��.
	CollisionPoints pointsPC, pointsEC;

	// �v���C���[�Ɖ~���̔����Ԃ�.
	std::vector<CollisionPoints> pointspecs;
	for (int i = 0; i < m_CylinderMax; ++i) {
		pointsPC = m_pGJK->GJK(Cylinders[i], PlayerEgg);
		pointspecs.push_back(pointsPC);
	}

	// �G�Ɖ~���̔����Ԃ�.
	std::vector<CollisionPoints> pointseecs;
	for (int i = 0; i < m_CylinderMax; ++i) {
		pointsEC = m_pGJK->GJK(Cylinders[i], EnemyEgg);
		pointseecs.push_back(pointsEC);
	}

	// �v���C���[�ƓG�̏��̔�����擾.
	CollisionPoints pointsPF = m_pGJK->GJK(PlayerEgg, Floor), pointsEF = m_pGJK->GJK(EnemyEgg, Floor);

	// �v���C���[�ƓG�̏��̏Փ˔��菈��.
	PlayertoFloorCol(pointsPF);
	EnemytoFloorCol(pointsEF);

	// �v���C���[�ƓG�ɏd�͂�������.
	m_pPlayer->UseGravity();
	m_pEnemy->UseGravity();

	// �v���C���[�ƓG�̒��̏Փ˔��菈��.
	for (int i = 0; i < m_CylinderMax; ++i) { PlayertoCylinderCol(pointspecs[i]); }
	for (int i = 0; i < m_CylinderMax; ++i) { EnemytoCylinderCol(pointseecs[i]); }

	// �v���C���[�ƓG�̓����蔻�菈��������.
	for (int i = 0; i < m_CylinderMax; ++i) {
		m_pPlayer->Collision(m_pEnemy, Floor, Cylinders[i]);
		m_pEnemy->Collision(m_pPlayer, Floor, Cylinders[i]);
	}
}


//-----------------------------------------------------------------------------
//		�G�ƃv���C���[�����_���ŃX�|�[��.
//-----------------------------------------------------------------------------
void CGame::InitEPPos(CRandom& random, std::unique_ptr<CPlayer>& player, std::unique_ptr<CEnemy>& enemy)
{
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
		D3DXVec3Normalize(&points.Normal, &points.Normal);	// �@���𐳋K��.
		D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();	// �v���C���[�̈ړ��x�N�g�����擾.

		// �v���C���[�̈ړ��x�N�g���Ɩ@���x�N�g���̓��ς��v�Z.
		float Dot = D3DXVec3Dot(&PlayerMove, &points.Normal);

		// �@�������̈ړ��������������āA�ǂɉ������ړ������݂̂ɂ���.
		PlayerMove = PlayerMove - Dot * points.Normal;

		// �C�������ړ��x�N�g�����v���C���[�ɓK�p.
		m_pPlayer->AddVec(PlayerMove);
		m_pPlayer->SetPos(m_pPlayer->GetPos() + points.Normal * points.Depth);

		// �[�x���ݒ�l�ȉ��̏ꍇ.
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
		m_pEnemy->ResetGravity();	// �G�ɂ�����d�͂̃��Z�b�g.
		m_pEnemy->CanJump();		// �W�����v���\��.

		// �n�ʂɏՓ˂��Ă���ꍇ.
		if (points.Normal.y < 0.f)
		{
			// �G�������߂����Ƃ��̍��W���Z�o.
			D3DXVECTOR3 SetPos = m_pEnemy->GetPos() - points.Normal * points.Depth;

			// �G�̈ʒu�̍X�V.
			m_pEnemy->SetPos(SetPos);
		}
		// �ǂ�ΖʂɏՓ˂��Ă���ꍇ.
		else {
			D3DXVec3Normalize(&points.Normal, &points.Normal);	// ���K��.
			D3DXVECTOR3 EnemyMove = m_pEnemy->GetMoveVec();		// �G�̈ړ��x�N�g�����擾.
			
			// �G�̈ړ��x�N�g���Ɩ@���x�N�g���̓��ς��v�Z.
			float Dot = D3DXVec3Dot(&EnemyMove, &points.Normal);

			// �@�������̈ړ��������������āA�ǂɉ������ړ������݂̂ɂ���.
			EnemyMove = EnemyMove - Dot * points.Normal;

			// �C�������ړ��x�N�g����G�ɉ��Z.
			m_pEnemy->AddVec(EnemyMove);
		}
	}
	else {
		m_pEnemy->AddGravity();	 // �G�ɂ�����d�͂𑝂₷.
		m_pEnemy->JumpPowerDec();// �W�����v�ŉ��Z�����l�����炷.
	}
}


//-----------------------------------------------------------------------------
//		�G�ƒ��̓����蔻����܂Ƃ߂�֐�.
//-----------------------------------------------------------------------------
void CGame::EnemytoCylinderCol(CollisionPoints points)
{
	// �������Ă���ꍇ.
	if (points.Col) {
		D3DXVec3Normalize(&points.Normal, &points.Normal);	// �@���𐳋K��.
		D3DXVECTOR3 EnemyMove = m_pEnemy->GetMoveVec();		// �v���C���[�̈ړ��x�N�g�����擾.

		// �v���C���[�̈ړ��x�N�g���Ɩ@���x�N�g���̓��ς��v�Z.
		float Dot = D3DXVec3Dot(&EnemyMove, &points.Normal);

		// �@�������̈ړ��������������āA�ǂɉ������ړ������݂̂ɂ���.
		EnemyMove = EnemyMove - Dot * points.Normal;

		// �C�������ړ��x�N�g����G�ɓK�p.
		m_pEnemy->AddVec(EnemyMove);
		m_pEnemy->SetPos(m_pEnemy->GetPos() + points.Normal * points.Depth);

		// �[�x���ݒ�l�ȉ��̏ꍇ.
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
	// �J�����x�N�g��.
	D3DXVECTOR3 camlookpos = CCamera::GetInstance()->GetPos() + CCamera::GetInstance()->GetCamDir() * 100.f;
	
	// ���C���p�̕ϐ�.
	RayInfo SendCamera = { false, camlookpos, 5000.f }, GroundRay, EnemyRay, CylinderRay;
	std::vector<RayInfo> CylinderRays;	// ���p�̃��C�z��.

	//-------------------------------------------------------------------------
	//	�J�������C�Ɗe�I�u�W�F���Ƃ̔�������擾.
	//-------------------------------------------------------------------------
	
	// �n�ʂƃJ�������C�̔�����擾.
	GroundRay = m_pFloor->IsHitForRay(CCamera::GetInstance()->GetRay());
	
	// ���ƃJ�������C�̔�����擾.
	for (int i = 0; i < m_CylinderMax; ++i) {
		CylinderRay = m_pCylinders[i]->IsHitForRay(CCamera::GetInstance()->GetRay());
		CylinderRays.push_back(CylinderRay);
	}
	
	// �G�ƃJ�������C�̔�����擾.
	EnemyRay = m_pEnemy->IsHitForRay(CCamera::GetInstance()->GetRay());


	//-------------------------------------------------------------------------
	//		�ǂ̃I�u�W�F�N�g���ł��J��������߂�����T��.
	//-------------------------------------------------------------------------
	
	// ���C���n�ʂɂ������Ă���ꍇ.
	if (GroundRay.Hit) {
		// �n�ʂ���J�����܂ł̃��C�������r�Atrue�̏ꍇ�l���㏑������.
		if (SendCamera.Length > GroundRay.Length) { SendCamera = GroundRay; }
	}
	
	// ���C���G�ɂ������Ă���ꍇ.
	if (EnemyRay.Hit) {
		// �G����J�����܂ł̃��C�������r���Atrue�̏ꍇ�l���㏑������.
		if (SendCamera.Length > EnemyRay.Length) { SendCamera = EnemyRay; }
	}

	// ���̐���for���[�v����.
	for (int i = 0; i < m_CylinderMax; ++i) {
		// ���C�����ɂ������Ă���ꍇ.
		if (CylinderRays[i].Hit) {
			// ������J�����܂ł̃��C�������r�Atrue�̏ꍇ�l���㏑������.
			if (SendCamera.Length > CylinderRays[i].Length) { SendCamera = CylinderRays[i]; }
		}
	}

	// �ŏI�I�ɍŒZ�����ɂ���I�u�W�F�N�g��Hit���W��n��.
	CCamera::GetInstance()->SetRayHit(SendCamera.HitPos);
}


//-----------------------------------------------------------------------------
//		UI�������܂Ƃ߂�֐�.
//-----------------------------------------------------------------------------
void CGame::UIUpdate()
{
	// �v���C���[��HP�O�̏ꍇ�A�������͒n�ʔ������Ă����ꍇ�A����UI���쐬.
	if (m_pLoseUI == nullptr
	&& (m_pPlayer->GetCharaInfo().HP <= 0|| m_pPlayer->GetPos().y < -100.f)) 
	{
		// �s�kUI�̍쐬
		m_pLoseUI = std::make_unique<CLoseUI>();
		m_pLoseUI->Create();

		// �s�k���̌��ʉ���炷.
		CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_Lose);
	}

	// �G��HP�O�̏ꍇ�A�������͒n�ʔ������Ă����ꍇ�A����UI���쐬.
	if (m_pWinUI == nullptr 
	&& (m_pEnemy->GetCharaInfo().HP <= 0|| m_pEnemy->GetPos().y < -100.f)) 
	{
		// ����UI�̍쐬
		m_pWinUI = std::make_unique<CWinUI>();
		m_pWinUI->Create();

		// �������̌��ʉ���炷.
		CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_Win);
	}
	
	// ������s�k��ʂ̍X�V����.
	if ( m_pLoseUI != nullptr )	{ m_pLoseUI->Update();	}
	if ( m_pWinUI  != nullptr )	{ m_pWinUI->Update();	}
}