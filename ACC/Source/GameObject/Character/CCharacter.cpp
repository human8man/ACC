#include "CCharacter.h"
#include "Camera/CCamera.h"


//============================================================================
//		キャラクタークラス.
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
	// レイの設定.
	m_pRayY			= new RAY();
	m_pRayY->Axis	= D3DXVECTOR3(0.f, -1.f, 0.f);	// 下向きの軸.
	m_pRayY->Length	= 10.f;
	m_pCrossRay		= new CROSSRAY();

	// 弾と銃の設定.
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
//		更新処理.
//============================================================================
void CCharacter::Update()
{
	CStaticMeshObject::Update();
	
	// 銃をキャラの周りで回す処理.
	m_pGun->UpdateGunPos(
		m_vPosition,
		m_GunRadius,
		D3DXToRadian(m_vRotation.y));

	// 銃の角度をプレイヤーの向き + 補正値に設定.
	m_pGun->SetRot( 0.f, -D3DXToRadian(m_vRotation.y) + m_GunRotRevision, 0.f );

	// 弾の更新.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		// 各弾を更新.
		m_pBullets[i]->Update();

		// 一定時間経過したら削除.
		if (m_pBullets[i]->DeleteBullet()) {
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}
	}
}


//============================================================================
//		描画処理
//============================================================================
void CCharacter::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CStaticMeshObject::Draw( View, Proj, Light );
}
