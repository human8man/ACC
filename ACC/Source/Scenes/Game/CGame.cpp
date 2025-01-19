#include "CGame.h"

#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"

#include "Effect/CEffect.h"

#include "Camera/CCamera.h"
#include "Object/Ground/CGround.h"
#include "Character/Player/CPlayer.h"
#include "Character/Enemy/CEnemy.h"

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
	, m_pCylinder	( nullptr )
	
	, m_pPlayer		( nullptr )
	, m_pEnemy		( nullptr )
	, m_pGround		( nullptr )

	, m_pGJK		( nullptr )
	, m_pCamRay		( nullptr )

	, m_Angle		(0.f)
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
	m_pCylinder = std::make_unique<CStaticMesh>();
	m_pPlayer	= std::make_unique<CPlayer>();
	m_pEnemy	= std::make_unique<CEnemy>();
	m_pGround	= std::make_unique<CGround>();
	m_pCamRay	= std::make_unique<CRay>();
}

//============================================================================
//		���[�h�f�[�^�֐�.
//============================================================================
HRESULT CGame::LoadData()
{
	// �X�^�e�B�b�N���b�V���̓ǂݍ���.
	m_pEgg		->Init( _T("Data\\Mesh\\Static\\Player\\egg.x"			));
	m_pFloor	->Init( _T("Data\\Mesh\\Static\\Stage\\Floor.x"			));
	m_pCylinder	->Init( _T("Data\\Mesh\\Static\\Stage\\Rectangular.x"	));

	// ���b�V�����A�^�b�`����.
	m_pPlayer->AttachMesh( *m_pEgg );
	m_pEnemy->AttachMesh( *m_pEgg );
	m_pGround->AttachMesh( *m_pFloor );

	// �L�����N�^�[�̏������W��ݒ�.
	m_pPlayer->SetPos( 0.f, 1.f, 6.f );
	m_pEnemy->SetPos( 10.f, 1.f, 6.f );
	m_pCylinder->SetPos( 10.f, -0.3f, 10.f );

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
	m_pCamRay.reset();
	m_pGround.reset();
	m_pEnemy.reset();
	m_pPlayer.reset();
	m_pCylinder.reset();
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
	CKey*	Key = CDInput::GetInstance()->CDKeyboard();

	// �v���C���[�̍X�V.
	m_pPlayer->Update();

	// �J�����̍X�V.
	CCamera::GetInstance()->Update();

	

	// ����ɂ͐�΂ɉ��o������i����,�s�k���o).
	// �v���C���[��HP���O�ɂȂ����Ƃ�.
	if(m_pPlayer->GetCharaInfo().HP == 0)
	{
		CSceneManager::GetInstance()->LoadScene(SceneList::Title);
	}

	// �G��HP���O�ɂȂ����Ƃ�.
	if(m_pEnemy->GetCharaInfo().HP == 0)
	{
		CSceneManager::GetInstance()->LoadScene(SceneList::Title);
	}

	CollisionJudge();


#if _DEBUG
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
	m_pCylinder->Render( m_mView, m_mProj, m_Light );

	// �G�t�F�N�g�̕`��.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
}


//-----------------------------------------------------------------------------
//		�����蔻��֐�.
//-----------------------------------------------------------------------------
void CGame::CollisionJudge()
{
	MeshCollider PlayerEgg, Floor, Cylinder;

	Cylinder.SetVertex(
		m_pCylinder->GetPos(),
		m_pCylinder->GetRot(),
		m_pCylinder->GetScale(),
		m_pCylinder->GetVertices());

	// �n�ʃf�[�^�ݒ�.
	Floor.SetVertex(
		m_pGround->GetPos(),
		m_pGround->GetRot(),
		m_pGround->GetScale(),
		m_pFloor->GetVertices());

	// �v���C���[�f�[�^�ݒ�.
	PlayerEgg.SetVertex(
		m_pPlayer->GetPos(),
		m_pPlayer->GetRot(),
		m_pPlayer->GetScale(),
		m_pEgg->GetVertices());


	// ���Ə��̔����Ԃ�.
	CollisionPoints pointspef = m_pGJK->GJK(PlayerEgg, Floor);
	// ���Ɖ~���̔����Ԃ�.
	CollisionPoints pointspec = m_pGJK->GJK(Cylinder, PlayerEgg);


	//------------------------------------------------------------------------------
	//		���Ə��̔��菈��.
	//------------------------------------------------------------------------------
	if (pointspef.Col)
	{
		// �v���C���[�ɂ�����d�͂����Z�b�g����.
		m_pPlayer->ResetGravity();
		// �W�����v���\�ɂ���.
		m_pPlayer->CanJump();

		// �@�����������������Ă���ꍇ�i�n�ʂɏՓ˂��Ă���j.
		if (pointspef.Normal.y < 0.f)
		{
			// �Փː[�x�Ɋ�Â��ăv���C���[�������߂�.
			D3DXVECTOR3 SetPos = m_pPlayer->GetPos() - pointspef.Normal * pointspef.Depth;

			// �v���C���[�̈ʒu���C����̍��W�ɍX�V.
			m_pPlayer->SetPos(SetPos);
		}
		// �@�����������ȊO�i�ǂ�Ζʁj�������Ă���ꍇ.
		else {
			D3DXVec3Normalize(&pointspef.Normal, &pointspef.Normal);	// ���K��.

			// �v���C���[�̌��݂̈ړ��x�N�g�����擾.
			D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();
			// �v���C���[�̈ړ��x�N�g���Ɩ@���x�N�g���̓��ς��v�Z�i�i�s�����Ɩ@���̏d�Ȃ����擾�j.
			float Dot = D3DXVec3Dot(&PlayerMove, &pointspef.Normal);
			// �@�������̈ړ��������������āA�ǂɉ������ړ������݂̂��c��.
			PlayerMove = PlayerMove - Dot * pointspef.Normal;
			// �C�������ړ��x�N�g�����v���C���[�ɉ��Z.
			m_pPlayer->AddVec(PlayerMove);
		}
	}
	else {
		// �v���C���[�ɂ�����d�͂𑝂₷.
		m_pPlayer->AddGravity();
		// �W�����v�ŉ��Z�����l�����炷.
		m_pPlayer->JumpPowerDec();
	}

	// �v���C���[�ɏd�͂�������.
	m_pPlayer->UseGravity();


	//------------------------------------------------------------------------------
	//		���ƒ��̔��菈��.
	//------------------------------------------------------------------------------
	if (pointspec.Col) {
		// �@���𐳋K�����ĒP�ʃx�N�g���ɂ���i�v�Z���x�����߂邽�߁j.
		D3DXVec3Normalize(&pointspec.Normal, &pointspec.Normal);

		// �v���C���[�̌��݂̈ړ��x�N�g�����擾.
		D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();
		// �v���C���[�̈ړ��x�N�g���Ɩ@���x�N�g���̓��ς��v�Z�i�i�s�����Ɩ@���̏d�Ȃ����擾�j.
		float Dot = D3DXVec3Dot(&PlayerMove, &pointspec.Normal);
		// �@�������̈ړ��������������āA�ǂɉ������ړ������݂̂��c��.
		PlayerMove = PlayerMove - Dot * pointspec.Normal;


		// �C�������ړ��x�N�g�����v���C���[�ɓK�p.
		m_pPlayer->AddVec(PlayerMove);
		m_pPlayer->SetPos(m_pPlayer->GetPos() + pointspec.Normal * pointspec.Depth);

		if (pointspec.Depth < 0.05f) {
			m_pPlayer->AddVec(-m_pPlayer->GetMoveVec());
		}
	}

	//------------------------------------------------------------------------------
	//		�n�ʂƃJ�������C�̔���(�e�̓����n�_�Ɏg�p����).
	//------------------------------------------------------------------------------
	auto [hit, hitpos, length] = m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	if( hit ) { 
		CCamera::GetInstance()->SetRayHit(hitpos); 
	}
	else {
		//auto [hit, hitpos, length] = m_pCylinder->IsHitForRay(CCamera::GetInstance()->GetRay());
		CCamera::GetInstance()->SetRayHit(hitpos);
	}

	//------------------------------------------------------------------------------
	//		�v���C���[�̓����蔻�菈��������.
	//------------------------------------------------------------------------------
	m_pPlayer->Collision(m_pEnemy, Floor, Cylinder);




#if _DEBUG
	//-----------------------------------------------------
	// �L�����N�^�[�E�B���h�E.
	//-----------------------------------------------------

	ImGui::Begin("CharaWindow");

	ImGui::Text("%d", m_pPlayer->GetCharaInfo().HP);
	ImGui::Text("%d", m_pEnemy->GetCharaInfo().HP);

	ImGui::End();



	//-----------------------------------------------------
	// �v���C���[�E�B���h�E.
	//-----------------------------------------------------
	ImGui::Begin("PlayerWindow");

	D3DXVECTOR3 playerpos = m_pPlayer->GetPos();
	ImGui::Text("%f,%f,%f", playerpos.x, playerpos.y, playerpos.z);
	D3DXVECTOR3 playersumvec = m_pPlayer->GetMoveVec();
	ImGui::Text("%f,%f,%f", playersumvec.x, playersumvec.y, playersumvec.z);

	ImGui::End();

	//-----------------------------------------------------
	// �e�E�B���h�E.
	//-----------------------------------------------------
	ImGui::Begin("CameraWindow");

	D3DXVECTOR3 camrot = CCamera::GetInstance()->GetRot();
	ImGui::Text("%f,%f,%f", camrot.x, camrot.y, camrot.z);
	ImGui::End();
#endif
}