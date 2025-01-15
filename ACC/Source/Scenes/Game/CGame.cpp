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

	, m_pMeshFighter	( nullptr )
	, m_pMeshGround		( nullptr )

	, m_pPlayer			( nullptr )
	, m_pGround			( nullptr )

	, m_pGJK			( nullptr )

	, m_pCamRay			( nullptr )

	, m_MeshA	()
	, m_MeshB	()

	, m_Angle	(0.f)
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
	m_pMeshFighter	= new CStaticMesh();
	m_pMeshGround	= new CStaticMesh();
	m_pPlayer		= new CPlayer();

	// �n�ʃN���X�̃C���X�^���X�쐬.
	m_pGround = new CGround();

	m_pCamRay = new CRay();
}

//============================================================================
//		���[�h�f�[�^�֐�.
//============================================================================
HRESULT CGame::LoadData()
{
	// �X�^�e�B�b�N���b�V���̓ǂݍ���.
	m_pMeshFighter	->Init( _T("Data\\Mesh\\Static\\Player\\egg.x" ) );
	m_pMeshGround	->Init( _T("Data\\Mesh\\Static\\Stage\\stage.x" ) );

	m_pPlayer->AttachMesh( *m_pMeshFighter );
	m_pGround->AttachMesh( *m_pMeshGround );

	// �L�����N�^�[�̏������W��ݒ�.
	m_pPlayer->SetPos( 0.f, 0.f, 6.f  );

	CCamera::GetInstance()->Init();

	
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

	SAFE_DELETE(m_pMeshGround);
	SAFE_DELETE(m_pMeshFighter);

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

	// �J�������̃L�[����𖳌��ɂ���.
	if (Key->IsKeyAction(DIK_F2)) { CCamera::GetInstance()->ChangeCanMove(); }
	if (Key->IsKeyAction(DIK_F3)) { CCamera::GetInstance()->ChangeUseMouse(); }
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

	m_pCamRay->Render(m_mView, m_mProj, CCamera::GetInstance()->GetRay());

	// �G�t�F�N�g�̕`��.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
	CollisionJudge();
}


//-----------------------------------------------------------------------------
//		�����蔻��֐�.
//-----------------------------------------------------------------------------
void CGame::CollisionJudge()
{
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
	//		m_pMeshFighter->GetVertices());

	//	CollisionPoints points = m_pGJK->GJK(m_MeshA, m_MeshB);

	//	// ���������ꍇ�폜.
	//	if (points.HasCollision)
	//	{
	//		SAFE_DELETE(m_pBullets[i]);
	//		m_pBullets.erase(m_pBullets.begin() + i);
	//		--i;
	//	}
	//}

	//auto [hit, hitpos, length] = m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	
//#if _DEBUG
//	// �e�E�B���h�E.
//	ImGui::Begin("CameraWindow");
//
//	D3DXVECTOR3 camrot = CCamera::GetInstance()->GetRot();
//	ImGui::Text("%f,%f,%f", camrot.x, camrot.y, camrot.z);
//	ImGui::DragFloat3("##Position", camrot, 0.1f);
//
//	ImGui::End();
//
//
//	// �e�E�B���h�E.
//	ImGui::Begin("BulletWindow");
//
//	D3DXVECTOR3 shootpos = m_pGun->GetPos();
//	ImGui::Text("%f,%f,%f", shootpos.x, shootpos.y, shootpos.z);
//	ImGui::DragFloat3("##Position", shootpos, 0.1f);
//	m_pGun->SetPos(shootpos);
//
//	ImGui::End();
//
//
//	// �e�̃E�B���h�E.
//	ImGui::Begin("BulletWindow");
//
//	for (size_t i = 0; i < m_pBullets.size(); ++i) {
//		D3DXVECTOR3 rot = m_pBullets[i]->GetRot();
//		D3DXVECTOR3 pos = m_pBullets[i]->GetPos();
//		ImGui::Text("------------------------------------");
//		ImGui::DragFloat3("rot", rot, 0.01f);
//		ImGui::DragFloat3("pos", pos, 0.01f);
//	}
//	ImGui::End();
//
//
//	// ���C�̓����蔻��̃E�B���h�E.
//	ImGui::Begin("ColWindow");
//
//	if(hit) { ImGui::Text("true");	}
//	else	{ ImGui::Text("false");	}
//	ImGui::Text("%f,%f,%f",hitpos.x,hitpos.y,hitpos.z );
//	ImGui::Text("%f",length );
//
//	ImGui::End();
//#endif
}