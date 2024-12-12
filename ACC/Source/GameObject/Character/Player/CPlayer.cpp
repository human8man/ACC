#include "CPlayer.h"
#include "DirectSound/CSoundManager.h"

CPlayer::CPlayer()
	: m_TurnSpeed		( 0.1f )	//きっちりやりたい場合はラジアン値を設定すること.
	, m_MoveSpeed		( 0.1f )
	, m_MoveState		( Stop )
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::Update()
{
#if 1
	//前進.
	if( GetAsyncKeyState( VK_UP ) & 0x8000 ){
		m_MoveState = enMoveState::Forward;
	}
	//後退.
	if( GetAsyncKeyState( VK_DOWN ) & 0x8000 ){
		m_MoveState = enMoveState::Backward;
	}
	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000 ){
		m_vRotation.y += m_TurnSpeed;
	}
	if( GetAsyncKeyState( VK_LEFT ) & 0x8000 ){
		m_vRotation.y -= m_TurnSpeed;
	}

	RadioControl();

#else
	float add_value = 0.1f;
	if( GetAsyncKeyState( VK_UP ) & 0x8000 ){
		m_vPosition.y += add_value;
	}
	if( GetAsyncKeyState( VK_DOWN ) & 0x8000 ){
		m_vPosition.y -= add_value;
	}
	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000 ){
		m_vPosition.x += add_value;
	}
	if( GetAsyncKeyState( VK_LEFT ) & 0x8000 ){
		m_vPosition.x -= add_value;
	}
#endif
	//前回のフレームで弾を飛ばしているかも知れないのでfalseにする.
	m_Shot = false;

	//弾を飛ばしたい!!.
	if( GetAsyncKeyState( 'Z' ) & 0x8000 ){
		m_Shot = true;

		//SEの再生.
		CSoundManager::PlaySE( CSoundManager::SE_Jump );
	}

	//レイの位置をプレイヤーの座標にそろえる.
	m_pRayY->Position = m_vPosition;
	//地面にめり込み回避のため、プレイヤーの位置よりも少し上にしておく.
	m_pRayY->Position.y += 0.2f;
	m_pRayY->RotationY = m_vRotation.y;

	//十字（前後左右に伸ばした）レイの設定.
	for (int dir = 0; dir < CROSSRAY::max; dir++)
	{
		m_pCrossRay->Ray[dir].Position = m_vPosition;
		m_pCrossRay->Ray[dir].Position.y += 0.2f;
		m_pCrossRay->Ray[dir].RotationY = m_vRotation.y;
	}

	CCharacter::Update();
}

void CPlayer::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj,
	LIGHT& Light )
{
	CCharacter::Draw( View, Proj, Light );
}

//ラジコン操作.
void CPlayer::RadioControl()
{
	//Z軸ベクトル（Z+方向への単位ベクトル）.
	//※長さ（大きさ）が１のベクトルを単位ベクトルという.
	D3DXVECTOR3 vecAxisZ( 0.0f, 0.0f, 1.0f );

	//Y方向の回転行列.
	D3DXMATRIX mRotationY;
	//Y軸回転行列を作成.
	D3DXMatrixRotationY(
		&mRotationY,		//(out)行列.
		m_vRotation.y );	//プレイヤーのY方向の回転値.

	//Y軸回転行列を使ってZ軸ベクトルを座標変換する.
	D3DXVec3TransformCoord(
		&vecAxisZ,		//(out)Z軸ベクトル.
		&vecAxisZ,		//(in)Z軸ベクトル.
		&mRotationY );	//Y軸回転行列.

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
	//上記の移動処理が終われば停止状態にしておく.
	m_MoveState = enMoveState::Stop;
}
