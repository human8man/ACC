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
	

	//----------------------------------------
	//		移動処理.
	//----------------------------------------
	
	// カメラの向きベクトルを取得.
	D3DXVECTOR3 camDir = CCamera::GetInstance()->GetCamDir();
	camDir.y = 0.f;	//Y情報があると飛び始めるのでYの要素を抜く.
	D3DXVec3Normalize(&camDir, &camDir); // 正規化.

	float moveSpeed = 0.1f;	// プレイヤーの移動速度.
	
	// 移動する方向ベクトル.
	D3DXVECTOR3 forward(ZEROVEC3);
	D3DXVECTOR3 left(ZEROVEC3);
	D3DXVECTOR3 upvec(0, 1, 0);

	// 左ベクトルを求める.
	D3DXVec3Cross(&left, &camDir, &upvec);
	D3DXVec3Normalize(&left, &left);

	// WASDで移動.
	if (Key->IsKeyDown( DIK_W )) { forward += camDir; }
	if (Key->IsKeyDown( DIK_S )) { forward -= camDir; }
	if (Key->IsKeyDown( DIK_A )) { forward += left; }
	if (Key->IsKeyDown( DIK_D )) { forward -= left; }

	// 最終的な移動方向を速度ベクトルに変換.
	D3DXVECTOR3 velocity = forward * moveSpeed;

	// 位置を更新.
	m_vPosition += velocity;
}