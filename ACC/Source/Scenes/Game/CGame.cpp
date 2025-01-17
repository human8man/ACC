#include "CGame.h"

#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"

#include "Effect/CEffect.h"

#include "Camera/CCamera.h"
#include "Object/Ground/CGround.h"
#include "Character/Player/CPlayer.h"

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
	m_pEgg		= new CStaticMesh();
	m_pFloor	= new CStaticMesh();
	m_pCylinder = new CStaticMesh();

	m_pPlayer	= new CPlayer();
	m_pGround	= new CGround();
	m_pCamRay	= new CRay();
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
	m_pPlayer->AttachMesh( *m_pEgg);
	m_pGround->AttachMesh( *m_pFloor);

	// �L�����N�^�[�̏������W��ݒ�.
	m_pPlayer->SetPos( 0.f, 14.f, 6.f );
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
	SAFE_DELETE(m_pCamRay);
	SAFE_DELETE(m_pGround);
	SAFE_DELETE(m_pPlayer);
	SAFE_DELETE(m_pCylinder);
	SAFE_DELETE(m_pEgg);
	SAFE_DELETE(m_pFloor);

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
	//if(m_pEnemy->GetCharaInfo().HP == 0)
	//{
	//	CSceneManager::GetInstance()->LoadScene(SceneList::Title);
	//}

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
	m_pCylinder->Render( m_mView, m_mProj, m_Light );

	// �G�t�F�N�g�̕`��.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
}


//-----------------------------------------------------------------------------
//		�����蔻��֐�.
//-----------------------------------------------------------------------------
void CGame::CollisionJudge()
{
	//		�v���C���[�Ƃ��G�N���X�ɂ�������������e�ł͂��邪
	//		���͂��̎��ł͂Ȃ��̂� "���e" ��Y�� "�Ȃ��悤" �ɃR�����g��.
	
	//// �e�ƃv���C���[�̔���.
	//for (size_t i = 0; i < m_pBullets.size(); ++i) {

	//	m_MeshB.SetVertex(
	//		m_pBullets[i]->GetPos(),
	//		m_pBullets[i]->GetRot(),
	//		m_pBullets[i]->GetScale(),
	//		m_pMeshBullet->GetVertices());

	//	m_MeshA.SetVertex(
	//		m_pPlayer->GetPos(),
	//		m_pPlayer->GetRot(),
	//		m_pPlayer->GetScale(),
	//		m_pMeshEgg->GetVertices());

	//	CollisionPoints points = m_pGJK->GJK(m_MeshA, m_MeshB);

	//	// ���������ꍇ�폜.
	//	if (points.HasCollision)
	//	{
	//		SAFE_DELETE(m_pBullets[i]);
	//		m_pBullets.erase(m_pBullets.begin() + i);
	//		--i;
	//	}
	//}
	MeshCollider Egg,Floor,Cylinder;

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
	Egg.SetVertex(
		m_pPlayer->GetPos(),
		m_pPlayer->GetRot(),
		m_pPlayer->GetScale(),
		m_pEgg->GetVertices());

	// �v���C���[�ɏd�͂�������.
	m_pPlayer->UseGravity();

	// ���Ə��̔����Ԃ�.
	CollisionPoints pointsef = m_pGJK->GJK(Egg,Floor);
	// ���Ɖ~���̔����Ԃ�.
	CollisionPoints pointsec = m_pGJK->GJK(Cylinder, Egg);

	if (pointsef.Col)
	{
		if (pointsef.Normal.y < 0.f) // �@�����������������Ă���ꍇ�i�n�ʂɏՓ˂��Ă���j.
		{
			// �Փː[�x�Ɋ�Â��ăv���C���[�������߂�.
			D3DXVECTOR3 SetPos = m_pPlayer->GetPos() - pointsef.Normal * pointsef.Depth;

			// �v���C���[�̈ʒu���C����̍��W�ɍX�V.
			m_pPlayer->SetPos(SetPos);
		}
		else // �@�����������ȊO�i�ǂ�Ζʁj�������Ă���ꍇ.
		{
			// �@���𐳋K�����ĒP�ʃx�N�g���ɂ���i�v�Z���x�����߂邽�߁j.
			D3DXVec3Normalize(&pointsef.Normal, &pointsef.Normal);

			// �v���C���[�̌��݂̈ړ��x�N�g�����擾.
			D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();

			// �v���C���[�̈ړ��x�N�g���Ɩ@���x�N�g���̓��ς��v�Z�i�i�s�����Ɩ@���̏d�Ȃ����擾�j.
			float Dot = D3DXVec3Dot(&PlayerMove, &pointsef.Normal);

			// �@�������̈ړ��������������āA�ǂɉ������ړ������݂̂��c��.
			PlayerMove = PlayerMove - Dot * pointsef.Normal;

			// �C�������ړ��x�N�g�����v���C���[�ɓK�p.
			m_pPlayer->AddVec(PlayerMove);
		}
	}


	if (pointsec.Col) {
		// �@���𐳋K�����ĒP�ʃx�N�g���ɂ���i�v�Z���x�����߂邽�߁j.
		D3DXVec3Normalize(&pointsec.Normal, &pointsec.Normal);

		// �v���C���[�̌��݂̈ړ��x�N�g�����擾.
		D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();

		// �v���C���[�̈ړ��x�N�g���Ɩ@���x�N�g���̓��ς��v�Z�i�i�s�����Ɩ@���̏d�Ȃ����擾�j.
		float Dot = D3DXVec3Dot(&PlayerMove, &pointsec.Normal);

		// �@�������̈ړ��������������āA�ǂɉ������ړ������݂̂��c��.
		PlayerMove = PlayerMove - Dot * pointsec.Normal;

		// �C�������ړ��x�N�g�����v���C���[�ɓK�p.
		m_pPlayer->AddVec(PlayerMove);
		m_pPlayer->SetPos(m_pPlayer->GetPos() + pointsec.Normal * pointsec.Depth);

		if (pointsec.Depth < 0.05f){
			m_pPlayer->AddVec(-m_pPlayer->GetMoveVec());
		}
	}
 
	// �n�ʂƃJ�������C�̔���(�e�̓����n�_�Ɏg�p����).
	auto [hit, hitpos, length] = m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	
#if _DEBUG
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

	//-----------------------------------------------------
	// �����蔻��̃E�B���h�E.
	//-----------------------------------------------------
	ImGui::Begin("ColWindow");

	//if(hit) { ImGui::Text("true");	}
	//else	{ ImGui::Text("false");	}
	//ImGui::Text("%f,%f,%f",hitpos.x,hitpos.y,hitpos.z );
	//ImGui::Text("%f",length );

	if(pointsec.Col) { ImGui::Text("true");	}
	else			 { ImGui::Text("false");}
	ImGui::Text("%f,%f,%f", pointsec.Normal.x, pointsec.Normal.y, pointsec.Normal.z);
	ImGui::Text("%f", pointsec.Depth);

	ImGui::End();
#endif
}