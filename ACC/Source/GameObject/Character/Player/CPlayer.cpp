#include "CPlayer.h"
#include "DirectSound/CSoundManager.h"
#include "Common/DirectInput/CDirectInput.h"

//============================================================================
//		プレイヤークラス.
//============================================================================
CPlayer::CPlayer()
	: m_TurnSpeed	( 0.1f )
	, m_MoveSpeed	( 0.1f )
	, m_MoveState	( Stop )
{
}

CPlayer::~CPlayer()
{
}


//============================================================================
//		更新処理.
//============================================================================
void CPlayer::Update()
{
	CKey* Key = CDInput::GetInstance()->CDKeyboard();

	RadioControl();

	// 前回のフレームで弾を飛ばしているかも知れないのでfalseにする.
	m_Shot = false;

	// 弾を飛ばす.
	if( Key->IsKeyAction( DIK_Z )){ m_Shot = true; }

	// レイの位置をプレイヤーの座標にそろえる.
	m_pRayY->Position = m_vPosition;
	// 地面にめり込み回避のため、プレイヤーの位置よりも少し上にしておく.
	m_pRayY->Position.y += 0.2f;
	m_pRayY->RotationY = m_vRotation.y;

	// 十字（前後左右に伸ばした）レイの設定.
	for (int dir = 0; dir < CROSSRAY::max; dir++)
	{
		m_pCrossRay->Ray[dir].Position = m_vPosition;
		m_pCrossRay->Ray[dir].Position.y += 0.2f;
		m_pCrossRay->Ray[dir].RotationY = m_vRotation.y;
	}

	CCharacter::Update();
}


//============================================================================
//		描画処理.
//============================================================================
void CPlayer::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CCharacter::Draw( View, Proj, Light );
}


//============================================================================
//		ラジコン操作.
//============================================================================
void CPlayer::RadioControl()
{
	// Z軸ベクトル（Z+方向への単位ベクトル）.
	D3DXVECTOR3 vecAxisZ( 0.f, 0.f, 1.f );

	// Y方向の回転行列.
	D3DXMATRIX mRotationY;
	// Y軸回転行列を作成.
	D3DXMatrixRotationY( &mRotationY, m_vRotation.y );

	// Y軸回転行列を使ってZ軸ベクトルを座標変換する.
	D3DXVec3TransformCoord( &vecAxisZ, &vecAxisZ, &mRotationY );

	switch( m_MoveState ){
	case enMoveState::Forward:	//前進.
		m_vPosition += vecAxisZ * m_MoveSpeed;
		break;
	case enMoveState::Backward:	//後退.
		m_vPosition -= vecAxisZ * m_MoveSpeed;
		break;
	default:
		break;
	}

	// 上記の移動処理が終われば停止状態にしておく.
	m_MoveState = enMoveState::Stop;
}
