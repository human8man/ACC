#include "CCharacter.h"
#include "Camera/CCamera.h"


//============================================================================
//		�L�����N�^�[�N���X.
//============================================================================
CCharacter::CCharacter()
	: m_pRayY			( nullptr )
	, m_pCrossRay		( nullptr )
	, m_pBullets		()
	, m_pGun			( nullptr )
	, m_pMeshBullet		( nullptr )
	, m_pMeshGun		( nullptr )
	, m_GunRadius		( 1.f )
	, m_GunRotRevision	( -1.5f )
{
	// ���C�̐ݒ�.
	m_pRayY			= new RAY();
	m_pRayY->Axis	= D3DXVECTOR3(0.f, -1.f, 0.f);	// �������̎�.
	m_pRayY->Length	= 10.f;
	m_pCrossRay		= new CROSSRAY();

	// �e�Əe�̐ݒ�.
	m_pMeshBullet	= new CStaticMesh();
	m_pMeshGun		= new CStaticMesh();
	m_pMeshGun		->Init( _T("Data\\Mesh\\Static\\Gun\\Gun.x" ) );
	m_pMeshBullet	->Init( _T("Data\\Mesh\\Static\\Bullet\\bullet.x" ) );

	m_pGun	= new CGun();
	m_pGun	->AttachMesh( *m_pMeshGun );

}

CCharacter::~CCharacter()
{
	SAFE_DELETE( m_pCrossRay );
	SAFE_DELETE( m_pRayY );
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		SAFE_DELETE(m_pBullets[i]);
	}
	SAFE_DELETE(m_pGun);
	SAFE_DELETE(m_pMeshGun);
	SAFE_DELETE(m_pMeshBullet);
}


//============================================================================
//		�X�V����.
//============================================================================
void CCharacter::Update()
{
	CStaticMeshObject::Update();
	
	// �e���L�����̎���ŉ񂷏���.
	m_pGun->UpdateGunPos(
		m_vPosition,
		m_GunRadius,
		D3DXToRadian(m_vRotation.y));

	// �e�̊p�x���v���C���[�̌��� + �␳�l�ɐݒ�.
	m_pGun->SetRot( 0.f, -D3DXToRadian(m_vRotation.y) + m_GunRotRevision, 0.f );

	// �e�̍X�V.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		// �e�e���X�V.
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
//		�`�揈��
//============================================================================
void CCharacter::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CStaticMeshObject::Draw( View, Proj, Light );
}
