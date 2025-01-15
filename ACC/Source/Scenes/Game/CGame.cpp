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

	, m_pMeshEgg	( nullptr )
	, m_pMeshGround	( nullptr )
	
	, m_pPlayer		( nullptr )
	, m_pGround		( nullptr )

	, m_pGJK		( nullptr )
	, m_pCamRay		( nullptr )

	, m_MeshA		()
	, m_MeshB		()

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
	m_pMeshEgg		= new CStaticMesh();
	m_pMeshGround	= new CStaticMesh();
	m_pPlayer		= new CPlayer();
	m_pGround		= new CGround();
	m_pCamRay		= new CRay();
}

//============================================================================
//		���[�h�f�[�^�֐�.
//============================================================================
HRESULT CGame::LoadData()
{
	// �X�^�e�B�b�N���b�V���̓ǂݍ���.
	m_pMeshEgg	 ->Init( _T("Data\\Mesh\\Static\\Player\\egg.x" ) );
	m_pMeshGround->Init( _T("Data\\Mesh\\Static\\Stage\\stage.x" ) );

	// ���b�V�����A�^�b�`����.
	m_pPlayer->AttachMesh( *m_pMeshEgg );
	m_pGround->AttachMesh( *m_pMeshGround );

	// �L�����N�^�[�̏������W��ݒ�.
	m_pPlayer->SetPos( 0.f, 0.f, 6.f  );

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
	SAFE_DELETE(m_pMeshGround);
	SAFE_DELETE(m_pMeshEgg);

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

	// �G�t�F�N�g�̕`��.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
	CollisionJudge();
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
	
	// �n�ʃf�[�^�ݒ�.
	m_MeshB.SetVertex(
		m_pGround->GetPos(),
		m_pGround->GetRot(),
		m_pGround->GetScale(),
		m_pMeshGround->GetVertices());

	// �v���C���[�f�[�^�ݒ�.
	m_MeshA.SetVertex(
		m_pPlayer->GetPos(),
		m_pPlayer->GetRot(),
		m_pPlayer->GetScale(),
		m_pMeshEgg->GetVertices());
	
	CollisionPoints points = m_pGJK->GJK(m_MeshA, m_MeshB);

	// �Փ˂��Ă���ꍇ.
	if (points.HasCollision) {
		// �v���C���[�̌����W����[�x�������߂�.
		m_pPlayer->SetPos(points.Normal * points.Depth);
	}
	
 
	// �n�ʂƃJ�������C�̔���(�e�̓����n�_�Ɏg�p����).
	auto [hit, hitpos, length] = m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	
#if _DEBUG
	// �e�E�B���h�E.
	ImGui::Begin("CameraWindow");

	D3DXVECTOR3 camrot = CCamera::GetInstance()->GetRot();
	ImGui::Text("%f,%f,%f", camrot.x, camrot.y, camrot.z);
	ImGui::DragFloat3("##Position", camrot, 0.1f);

	ImGui::End();

	// ���C�̓����蔻��̃E�B���h�E.
	ImGui::Begin("ColWindow");

	if(hit) { ImGui::Text("true");	}
	else	{ ImGui::Text("false");	}
	ImGui::Text("%f,%f,%f",hitpos.x,hitpos.y,hitpos.z );
	ImGui::Text("%f",length );

	ImGui::End();
#endif
}