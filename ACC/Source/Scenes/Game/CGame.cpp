#include "CGame.h"

#include "DirectSound/CSoundManager.h"

#include "Effect/CEffect.h"	//Effekseer���g�����߂̃N���X.
#include "../GameObject/Camera/CCamera.h"

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
	, m_pShot			( nullptr )
{
	// ���C�g���.
	m_Light.vDirection	= D3DXVECTOR3( 1.5f, 1.f, -1.f );	//���C�g����.
}

CGame::~CGame()
{
	SAFE_DELETE( m_pShot );
	SAFE_DELETE( m_pGround );
	
	SAFE_DELETE( m_pPlayer );

	SAFE_DELETE( m_pMeshGun );
	SAFE_DELETE( m_pMeshBullet );
	SAFE_DELETE( m_pMeshGround );
	SAFE_DELETE( m_pMeshFighter );

	//�O���ō쐬���Ă���̂ŁA�����Ŕj�����Ȃ�.
	m_hWnd = nullptr;
}

// �\�z.
void CGame::Create()
{
	m_pMeshFighter	= new CStaticMesh();
	m_pMeshGround	= new CStaticMesh();
	m_pMeshBullet	= new CStaticMesh();
	m_pMeshGun		= new CStaticMesh();
	m_pPlayer		= new CPlayer();

	//�n�ʃN���X�̃C���X�^���X�쐬.
	m_pGround = new CGround();
	m_pGround->SetPlayer(*m_pPlayer);
	m_pShot = new CShot();
}

// ���[�h�f�[�^�֐�.
HRESULT CGame::LoadData()
{

	// �n�ʃX�v���C�g�̍\����.
	CSprite3D::SPRITE_STATE SSGround{ 1.f, 1.f, 256.f, 256.f, 256.f, 256.f };

	// �v���C���[�X�v���C�g�̍\����.
	CSprite3D::SPRITE_STATE SSPlayer = { 1.f, 1.f, 64.f, 64.f, 64.f, 64.f };

	// �����v���C�g�̍\����.
	CSprite3D::SPRITE_STATE SSExplosion = { 1.f, 1.f, 256.f, 256.f, 32.f, 32.f };

	// �X�^�e�B�b�N���b�V���̓ǂݍ���.
	m_pMeshGun		->Init( _T("Data\\Mesh\\Static\\Gun\\Gun.x" ) );
	m_pMeshFighter	->Init( _T("Data\\Mesh\\Static\\Player\\egg.x" ) );
	m_pMeshGround	->Init( _T("Data\\Mesh\\Static\\Stage\\stage.x" ) );
	m_pMeshBullet	->Init( _T("Data\\Mesh\\Static\\Bullet\\bullet.x" ) );

	m_pPlayer->AttachMesh( *m_pMeshFighter );
	m_pGround->AttachMesh( *m_pMeshGround );
	m_pShot->AttachMesh( *m_pMeshBullet );

	m_pShot->CreateBSphereForMesh( *m_pMeshBullet );

	// �L�����N�^�[�̏������W��ݒ�.
	m_pPlayer->SetPosition( 0.f, 1.f, 6.f  );

	CCamera::GetInstance()->Init();
	CCamera::SetPlayerPos(m_pPlayer->GetPosition());

	return S_OK;
}

// ����֐�.
void CGame::Release()
{
}

void CGame::Init()
{
	CCamera::GetInstance()->Init();	// �J�����̏�����.
}


// �X�V����.
void CGame::Update()
{
	CCamera::GetInstance()->Update();

	m_pPlayer->Update();

	m_pGround->Update();
	
	//�e���΂������I.
	if( m_pPlayer->IsShot() == true ){
		m_pShot->Reload(
			m_pPlayer->GetPosition(),
			m_pPlayer->GetRotation().y );
	}

	m_pShot->Update();
}

//�`�揈��.
void CGame::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);

	m_pGround->Draw( m_mView, m_mProj, m_Light );

	m_pPlayer->Draw( m_mView, m_mProj, m_Light );

	m_pShot->Draw( m_mView, m_mProj, m_Light );
	m_pMeshGun->Render(m_mView, m_mProj, m_Light);
	//�����蔻��̒��S���W���X�V����.
	m_pPlayer->UpdateBSpherePos();
	m_pShot->UpdateBSpherePos();

	//�e�ƃG�l�~�[�̓����蔻��.
	if( m_pShot->GetBSphere()->IsHit( *m_pGround->GetBSphere() ) )
	{
		//�e.
		m_pShot->SetDisplay( false );				//��\��.
		m_pShot->SetPosition( 0.f, -10.f, 0.f );	//�n�ʂɖ��߂�.
	}

	//Effect�N���X.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
}