#include "CEnemy.h"
#include "DirectSound/CSoundManager.h"
#include "Camera/CCamera.h"
#include "Character/Player/CPlayer.h"

//============================================================================
//		エネミークラス.
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
//		更新処理.
//============================================================================
void CEnemy::Update()
{
	// 毎フレームリセットする.
	m_SumVec = ZEROVEC3;

	// カメラに向きを合わせる.
	m_vRotation.y = CCamera::GetInstance()->GetRot().y;
	
	// 入力処理後にカメラ座標をセット.
	if ( !CCamera::GetInstance()->GetMoveCamera() ) {
		// プレイヤー位置 + プレイヤーの高さを含んだ座標を渡す.
		D3DXVECTOR3 campos = m_vPosition;
		campos.y += m_CamRevision;
		 CCamera::GetInstance()->SetPosition(campos);
	}

	CCharacter::Update();
}


//============================================================================
//		描画処理.
//============================================================================
void CEnemy::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	// プレイヤーは描画しない.
	CCharacter::Draw( View, Proj, Light );
	
	// 弾の描画.
	for ( size_t i = 0; i < m_pBullets.size(); ++i ) { m_pBullets[i]->Draw(View, Proj, Light); }

	// 銃の描画.
	m_pGun->Draw(View, Proj, Light);
}


//============================================================================
//		当たり判定処理.
//============================================================================
void CEnemy::Collision(std::unique_ptr<CPlayer>& egg, MeshCollider floor, MeshCollider cylinder)
{
	MeshCollider Bullet, PlayerEgg;
	
	// プレイヤーデータを取得.
	PlayerEgg.SetVertex(
		egg->GetPos(),
		egg->GetRot(),
		egg->GetScale(),
		egg->GetMesh()->GetVertices());

	// 弾の判定.
	for ( size_t i = 0; i < m_pBullets.size(); ++i ) {

		// 弾データを取得.
		Bullet.SetVertex(
			m_pBullets[i]->GetPos(),
			m_pBullets[i]->GetRot(),
			m_pBullets[i]->GetScale(),
			m_pMeshBullet->GetVertices());


		// 当たり判定情報用の変数を宣言.
		CollisionPoints pointsbc, pointsbf, pointsbp;
		pointsbc = m_pGJK->GJK(Bullet, cylinder);
		pointsbf = m_pGJK->GJK(Bullet, floor);
		pointsbp = m_pGJK->GJK(Bullet, PlayerEgg);


		// 柱や床にあたった場合削除.
		if ( pointsbc.Col || pointsbf.Col ) {
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}

		// プレイヤーと弾が当たった場合.
		if ( pointsbp.Col ) {
			// ヘッドショット判定(気室判定).
			if (m_pBullets[i]->GetPos().y < egg->GetPos().y + m_EggAirRoomY) { egg->DubleDecreHP(); }
			else  { egg->DecreHP(); }

			// 当たったあとは削除.
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}
	}
}