#include "CGame.h"

#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"

#include "Effect/CEffect.h"

#include "Camera/CCamera.h"
#include "Object/Ground/CGround.h"
#include "Character/Player/CPlayer.h"
#include "Object/Bullet/CBullet.h"

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
	, m_pMeshBullet		( nullptr )
	, m_pMeshGun		( nullptr )

	, m_pPlayer			( nullptr )
	, m_pGround			( nullptr )
	, m_pBullet			( nullptr )
	, m_pBullets		()

	, m_pGJK			( nullptr )

	, m_pCamRay			( nullptr )

	, m_PLayerSize		{ 0.f, 2.f, 0.f, }
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
	m_pMeshBullet	= new CStaticMesh();
	m_pMeshGun		= new CStaticMesh();
	m_pPlayer		= new CPlayer();

	// �n�ʃN���X�̃C���X�^���X�쐬.
	m_pGround = new CGround();
	m_pGround->SetPlayer(*m_pPlayer);
	m_pBullet = new CBullet();

	m_pCamRay = new CRay();
}

//============================================================================
//		���[�h�f�[�^�֐�.
//============================================================================
HRESULT CGame::LoadData()
{
	// �X�^�e�B�b�N���b�V���̓ǂݍ���.
	m_pMeshGun		->Init( _T("Data\\Mesh\\Static\\Gun\\Gun.x" ) );
	m_pMeshFighter	->Init( _T("Data\\Mesh\\Static\\Player\\egg.x" ) );
	m_pMeshGround	->Init( _T("Data\\Mesh\\Static\\Stage\\stage.x" ) );
	m_pMeshBullet	->Init( _T("Data\\Mesh\\Static\\Bullet\\bullet.x" ) );

	m_pPlayer->AttachMesh( *m_pMeshFighter );
	m_pGround->AttachMesh( *m_pMeshGround );
	m_pBullet->AttachMesh( *m_pMeshBullet );

	// �L�����N�^�[�̏������W��ݒ�.
	m_pPlayer->SetPos( 0.f, 0.f, 6.f  );
	m_pBullet->SetPos(0.f, -1000.f, 6.f);

	CCamera::GetInstance()->Init();

	
	m_pCamRay->Init(CCamera::GetInstance()->GetRay());

	return S_OK;
}


//============================================================================
//		����֐�.
//============================================================================
void CGame::Release()
{
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		SAFE_DELETE(m_pBullets[i]);
	}
	
	SAFE_DELETE(m_pCamRay);

	SAFE_DELETE(m_pGround);

	SAFE_DELETE(m_pPlayer);

	SAFE_DELETE(m_pMeshGun);
	SAFE_DELETE(m_pMeshBullet);
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
	D3DXVECTOR3 Bpos = m_pMeshGun->GetPos();
	float radius = 5.f;	// ���a.
	m_Angle += 1.f;		// ��]�p�x�i�t���[�����Ƃɑ����j.
	UpdateSatellitePosition(m_pPlayer->GetPos(), Bpos, radius, m_Angle);	// �q��B�̈ʒu���X�V.
	m_pMeshGun->SetPos(Bpos);

	CKey*	Key	  = CDInput::GetInstance()->CDKeyboard();
	CMouse* Mouse = CDInput::GetInstance()->CDMouse();

	// �J�����̍X�V.
	CCamera::GetInstance()->Update();

	// �J�������̃L�[����𖳌��ɂ���.
	if (Key->IsKeyAction(DIK_F2)) { CCamera::GetInstance()->ChangeCanMove(); }
	if (Key->IsKeyAction(DIK_F3)) { CCamera::GetInstance()->ChangeUseMouse(); }
	
	// ���N���b�N.
	if (Mouse->IsLAction()) {
		m_pBullets.push_back(new CBullet()); // m_pBullets �ɒǉ�

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);

		D3DXVECTOR3 direction = CCamera::GetInstance()->GetCamDir();
		float yaw = atan2(direction.x, direction.z);
		float pitch = atan2(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z));
		D3DXVECTOR3 rot(pitch,yaw,0.f);

		// �����ʒu,�ړ������̒P�ʃx�N�g��,�e�̌���,���x�����邽�ߕۗ�.
		m_pBullets.back()->Init(
			CCamera::GetInstance()->GetPos(),
			CCamera::GetInstance()->GetCamDir(),
			rot,
			0.01f );
	}

	// �v���C���[�̍X�V.
	m_pPlayer->Update();

	// �v���C���[�̍X�V��Ƀv���C���[���W�ɃJ�������Z�b�g.
	if ( !CCamera::GetInstance()->GetMoveCamera())
	{	  CCamera::GetInstance()->SetPosition(m_pPlayer->GetPos() + m_PLayerSize); }

	// �e�̍X�V.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		// �e�e���X�V
		m_pBullets[i]->Update();

		// ��莞�Ԍo�߂�����폜.
		if (m_pBullets[i]->DeleteBullet()) {
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}
	}
}


//============================================================================
//		�`�揈��.
//============================================================================
void CGame::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);

	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		m_pBullets[i]->Draw(m_mView, m_mProj, m_Light);
	}

	m_pGround->Draw( m_mView, m_mProj, m_Light );
	m_pPlayer->Draw( m_mView, m_mProj, m_Light );
	m_pMeshGun->Render(m_mView, m_mProj, m_Light);

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
	// �e�ƃv���C���[�̔���.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {

		m_MeshB.SetVertex(
			m_pBullets[i]->GetPos(),
			m_pBullets[i]->GetRot(),
			m_pBullets[i]->GetScale(),
			m_pMeshBullet->GetVertices());

		m_MeshA.SetVertex(
			m_pPlayer->GetPos(),
			m_pPlayer->GetRot(),
			m_pPlayer->GetScale(),
			m_pMeshFighter->GetVertices());

		CollisionPoints points = m_pGJK->GJK(m_MeshA, m_MeshB);

		// ���������ꍇ�폜.
		if (points.HasCollision)
		{
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}
	}

	auto [hit, hitpos, length] = m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	
#if _DEBUG
	ImGui::Begin("BulletWindow");

	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		D3DXVECTOR3 Bullet = m_pBullets[i]->GetPos();
		ImGui::Text("%f,%f,%f", Bullet.x, Bullet.y, Bullet.z);
		ImGui::DragFloat3("##Position", Bullet, 0.1f);
		m_pBullets[i]->SetPos(Bullet);
	}
	ImGui::End();

	ImGui::Begin("ColWindow");

	if(hit) { ImGui::Text("true");	}
	else	{ ImGui::Text("false");	}
	ImGui::Text("%f,%f,%f",hitpos.x,hitpos.y,hitpos.z );
	ImGui::Text("%f",length );

	ImGui::End();
#endif
}


//-----------------------------------------------------------------------------
//		�N�H�^�[�j�I��������.
//-----------------------------------------------------------------------------
void CGame::UpdateSatellitePosition(
	const D3DXVECTOR3& center, 
	D3DXVECTOR3& pos, 
	float radius, 
	float angle)
{
	// ��]���i�����Y�����j.
	D3DXVECTOR3 rotationAxis(0.f, 1.f, 0.f);

	// �����ʒu�iB��A����radius���ꂽ�ʒu�j.
	D3DXVECTOR3 initialPosition(radius, 0.f, 0.f);

	// �N�H�[�^�j�I�����g���ĉ�]���v�Z.
	D3DXQUATERNION rotationQuat;
	D3DXQuaternionRotationAxis(&rotationQuat, &rotationAxis, D3DXToRadian(angle));

	D3DXVECTOR3 rotatedPosition; // ��]��̍��W���v�Z.
	D3DXMATRIX rotationMatrix;	 // ��]�s������O�ɐ錾.
	D3DXMatrixRotationQuaternion(&rotationMatrix, &rotationQuat);
	D3DXVec3TransformCoord(&rotatedPosition, &initialPosition, &rotationMatrix);

	// A����ɍ��W�����Z.
	pos = center + rotatedPosition;
}