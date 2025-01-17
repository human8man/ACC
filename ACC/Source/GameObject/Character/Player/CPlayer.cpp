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
	, m_CamRevision	( 2.f )
	, m_JumpPower	( 117.6f )
	, m_SumVec		(ZEROVEC3)
{
	m_CharaInfo.HP = 8;
}

CPlayer::~CPlayer()
{
}


//============================================================================
//		更新処理.
//============================================================================
void CPlayer::Update()
{
	// 毎フレームリセットする.
	m_SumVec = ZEROVEC3;

	// カメラに向きを合わせる.
	m_vRotation.y = CCamera::GetInstance()->GetRot().y;
	
	// 入力処理.
	KeyInput();

	// 入力処理後にカメラ座標をセット.
	if (!CCamera::GetInstance()->GetMoveCamera()) {
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
void CPlayer::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	// プレイヤーは描画しない.
	// CCharacter::Draw( View, Proj, Light );
	
	// 弾の描画.
	for (size_t i = 0; i < m_pBullets.size(); ++i) { m_pBullets[i]->Draw(View, Proj, Light); }

	// 銃の描画.
	m_pGun->Draw(View, Proj, Light);
}


//-----------------------------------------------------------------------------
//		キー入力処理.
//-----------------------------------------------------------------------------
void CPlayer::KeyInput()
{
	CKey* Key = CDInput::GetInstance()->CDKeyboard();
	CMouse* Mouse = CDInput::GetInstance()->CDMouse();
	

	//----------------------------------------------------------------------
	//		移動処理.
	//----------------------------------------------------------------------
	
	// カメラの向きベクトルを取得.
	D3DXVECTOR3 camDir = CCamera::GetInstance()->GetCamDir();
	camDir.y = 0.f;	// Y情報があると飛び始めるのでYの要素を抜く.
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

	// 最終的な移動方向を速度ベクトルに変換し合計の移動量に渡す.
	m_SumVec += forward * moveSpeed;


	//----------------------------------------------------------------------
	//		左クリックで射撃.
	//----------------------------------------------------------------------
	if (Mouse->IsLAction()) {
		// カメラの向きベクトルを取得.
		D3DXVECTOR3 direction = CCamera::GetInstance()->GetCamDir();
		m_pBullets.push_back(new CBullet());	// m_pBullets に追加.

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// メッシュを設定.
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	//初期位置を仮設定.

		// ベクトルのノーマライズ（方向のみを抽出）.
		D3DXVec3Normalize(&direction, &direction);

		// 初期位置,移動方向の単位ベクトル,弾の向き,速度がいるため保留.
		m_pBullets.back()->Init(
			m_pGun->GetShootPos(),
			CCamera::GetInstance()->GetCamDir(),
			direction,
			0.01f );
	}

	// 合計のベクトル量分位置を更新.
	m_vPosition += m_SumVec;
}