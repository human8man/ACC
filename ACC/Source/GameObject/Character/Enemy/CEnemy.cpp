#include "CEnemy.h"
#include "DirectSound/CSoundManager.h"
#include "Camera/CCamera.h"
#include "Character/Player/CPlayer.h"

//============================================================================
//		�G�l�~�[�N���X.
//============================================================================
CEnemy::CEnemy()
	: m_pGJK		( nullptr )
	, m_TurnSpeed	( 0.1f )
	, m_MoveSpeed	( 0.2f )
	, m_CamRevision	( 2.f )
	, m_SumVec		( ZEROVEC3 )
{
	m_CharaInfo.HP = 8;
}

CEnemy::~CEnemy()
{
}


//============================================================================
//		�X�V����.
//============================================================================
void CEnemy::Update()
{
	// ���t���[�����Z�b�g����.
	m_SumVec = ZEROVEC3;

	// �J�����Ɍ��������킹��.
	m_vRotation.y = CCamera::GetInstance()->GetRot().y;
	
	// ���͏�����ɃJ�������W���Z�b�g.
	if ( !CCamera::GetInstance()->GetMoveCamera() ) {
		// �v���C���[�ʒu + �v���C���[�̍������܂񂾍��W��n��.
		D3DXVECTOR3 campos = m_vPosition;
		campos.y += m_CamRevision;
		 CCamera::GetInstance()->SetPosition(campos);
	}

	CCharacter::Update();
}


//============================================================================
//		�`�揈��.
//============================================================================
void CEnemy::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	// �v���C���[�͕`�悵�Ȃ�.
	CCharacter::Draw( View, Proj, Light );
	
	// �e�̕`��.
	for ( size_t i = 0; i < m_pBullets.size(); ++i ) { m_pBullets[i]->Draw(View, Proj, Light); }

	// �e�̕`��.
	m_pGun->Draw(View, Proj, Light);
}


//============================================================================
//		�����蔻�菈��.
//============================================================================
void CEnemy::Collision(std::unique_ptr<CPlayer>& egg, MeshCollider floor, MeshCollider cylinder)
{
	MeshCollider Bullet, PlayerEgg;
	
	// �v���C���[�f�[�^���擾.
	PlayerEgg.SetVertex(
		egg->GetPos(),
		egg->GetRot(),
		egg->GetScale(),
		egg->GetMesh()->GetVertices());

	// �e�̔���.
	for ( size_t i = 0; i < m_pBullets.size(); ++i ) {

		// �e�f�[�^���擾.
		Bullet.SetVertex(
			m_pBullets[i]->GetPos(),
			m_pBullets[i]->GetRot(),
			m_pBullets[i]->GetScale(),
			m_pMeshBullet->GetVertices());


		// �����蔻����p�̕ϐ���錾.
		CollisionPoints pointsbc, pointsbf, pointsbp;
		pointsbc = m_pGJK->GJK(Bullet, cylinder);
		pointsbf = m_pGJK->GJK(Bullet, floor);
		pointsbp = m_pGJK->GJK(Bullet, PlayerEgg);


		// ���⏰�ɂ��������ꍇ�폜.
		if ( pointsbc.Col || pointsbf.Col ) {
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}

		// �v���C���[�ƒe�����������ꍇ.
		if ( pointsbp.Col ) {
			// �w�b�h�V���b�g����(�C������).
			if (m_pBullets[i]->GetPos().y < egg->GetPos().y + m_EggAirRoomY) { egg->DubleDecreHP(); }
			else  { egg->DecreHP(); }

			// �����������Ƃ͍폜.
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}
	}
}