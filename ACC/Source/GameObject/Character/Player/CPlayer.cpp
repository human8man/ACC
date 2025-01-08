#include "CPlayer.h"
#include "DirectSound/CSoundManager.h"
#include "Common/DirectInput/CDirectInput.h"
#include "Camera/CCamera.h"

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

	KeyInput();

	// 前回のフレームで弾を飛ばしているかも知れないのでfalseにする.
	m_Shot = false;
	
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


//-----------------------------------------------------------------------------
//		キー入力処理.
//-----------------------------------------------------------------------------
void CPlayer::KeyInput()
{
	CKey* Key = CDInput::GetInstance()->CDKeyboard();
	
	// 弾を飛ばす.
	if( Key->IsKeyAction( DIK_Z )){ m_Shot = true; }


	//----------------------------------------
	//		移動処理.
	//----------------------------------------
	
	// カメラの向きベクトルを取得.
	D3DXVECTOR3 camDir = CCamera::GetInstance()->GetCamDir();
	D3DXVec3Normalize(&camDir, &camDir); // 正規化.
	
	float moveSpeed = 1.f;	// プレイヤーの移動速度.
	
	// 移動する方向ベクトル.
	D3DXVECTOR3 forward(ZEROVEC3);
	D3DXVECTOR3 right(ZEROVEC3);
	
	D3DXVec3Cross(&right, &camDir, &D3DXVECTOR3(0, 1, 0));
	D3DXVec3Normalize(&right, &right);

	// WASDで移動.
	if (Key->IsKeyDown( DIK_W )) { forward += camDir; }
	if (Key->IsKeyDown( DIK_S )) { forward -= camDir; }
	if (Key->IsKeyDown( DIK_A )) { forward -= right; }
	if (Key->IsKeyDown( DIK_D )) { forward += right; }

	// 最終的な移動方向を速度ベクトルに変換.
	D3DXVECTOR3 velocity = forward * moveSpeed;

	// 位置を更新.
	m_vPosition += velocity;
}