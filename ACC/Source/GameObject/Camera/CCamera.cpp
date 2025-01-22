#include "CCamera.h"
#include "Common/DirectInput/CDirectInput.h"


//===================================================================
//		カメラクラス.
//===================================================================
CCamera::CCamera()
	: m_Camera			()
	
	, m_NowCurorPos		( RESETPOS )
	, m_BeforCursorPos	( RESETPOS )

	, m_MouseMoveDis	( ZEROVEC2 )
	, m_CameraRot		( ZEROVEC3 )
	, m_DirectionPos	( ZEROVEC3 )
	, m_RayHitPoint		( ZEROVEC3 )

	, m_Fov				( 60.0f )
	, m_DefaultFov		( 60.0f )
	, m_MoveValue		(  0.1f )
	, m_CameraYaw		(  0.0f )
	, m_CameraPitch		(  0.0f )
	, m_MouseSens		( 0.03f )

	, m_CanMoveMouse	( false )
	, m_CanMoveCamera	( false )
{
	Init();
}

CCamera::~CCamera()
{
}


//===================================================================
//		初期化.
//===================================================================
void CCamera::Init()
{
	// 上方ベクトルの初期化.
	m_CameraRot		= ZEROVEC3;
	m_Camera.Look	= ZEROVEC3;
	m_Camera.UpVec	= D3DXVECTOR3(0.f, 1.f, 0.f);
	m_Camera.Pos	= D3DXVECTOR3(0.f, 3.f, 0.f);

	 m_Fov = m_DefaultFov;

	MouseMove(ZEROVEC2);	// カメラの回転の初期化.
}


//===================================================================
//		更新処理.
//===================================================================
void CCamera::Update()
{
	// マウス移動量の初期化.
	m_MouseMoveDis = ZEROVEC2;


	// カメラの操作が可能な場合、キー入力処理を通る.
	if( m_CanMoveCamera ) { KeyInput(); }


	// マウスが操作可能か.
	if ( !m_CanMoveMouse )
	{
		// マウス座標の取得.
		GetCursorPos(&m_NowCurorPos);
		ScreenToClient(GetConsoleWindow(), &m_NowCurorPos);

		// 前回のマウス位置と違う場合.
		if (m_BeforCursorPos.x != m_NowCurorPos.x
		||  m_BeforCursorPos.y != m_NowCurorPos.y)
		{
			// マウス移動距離計算.
			m_MouseMoveDis.x = static_cast<float>(m_NowCurorPos.x - m_BeforCursorPos.x);
			m_MouseMoveDis.y = static_cast<float>(m_NowCurorPos.y - m_BeforCursorPos.y);

			// マウス位置を画面中央に初期化.
			SetCursorPos(WND_WM, WND_HM);	// 現在のマウス位置リセット.
			m_BeforCursorPos = RESETPOS;	// 過去のマウス位置リセット.
		}
	}

	// ベクトル計算をし、注視点に代入.
	MouseMove(m_MouseMoveDis);
	ShowCursor(m_CanMoveMouse);

	// レイに値を入れる.
	m_pRay.Position = m_Camera.Pos;
	m_CamDirection = m_Camera.Look - m_Camera.Pos;
	D3DXVec3Normalize(&m_CamDirection, &m_CamDirection);
	m_pRay.Axis = m_CamDirection;
	m_pRay.Length = 1.f;
}

//===================================================================
//		カメラ関数(ビュー行列計算).
//===================================================================
void CCamera::Camera( D3DXMATRIX& View ) const
{
	D3DXVECTOR3 pos	 = m_Camera.Pos;
	D3DXVECTOR3 look = m_Camera.Look;
	D3DXVECTOR3	vec	 = m_Camera.UpVec;

	// ビュー（カメラ）変換.
	D3DXMatrixLookAtLH( &View, &pos, &look, &vec );
}


//-------------------------------------------------------------------
//		キー入力.
//-------------------------------------------------------------------
void CCamera::KeyInput()
{
	CKey* Key = CDInput::GetInstance()->CDKeyboard();

	// マウスの操作ができない場合(マウスが画面中央に固定されている場合).
	if (!m_CanMoveMouse)
	{
		if ( Key->IsKeyDown(DIK_W) ){ CameraMove(Straight);	}
		if ( Key->IsKeyDown(DIK_S) ){ CameraMove(Back);		}
		if ( Key->IsKeyDown(DIK_A) ){ CameraMove(Left);		}
		if ( Key->IsKeyDown(DIK_D) ){ CameraMove(Right);	}

		float Yvalue = 0.f;
		if ( Key->IsKeyDown(DIK_SPACE) )  { Yvalue =  m_MoveValue; }
		if ( Key->IsKeyDown(DIK_LSHIFT) ) { Yvalue = -m_MoveValue; }

		m_Camera.Pos.y  += Yvalue;
		m_Camera.Look.y += Yvalue;
	}

	// マウスカーソルの表示.
	ShowCursor(m_CanMoveMouse);
}


//-------------------------------------------------------------------
//		マウスの移動量に応じてカメラの方向を変更する.
//-------------------------------------------------------------------
void CCamera::MouseMove(D3DXVECTOR2 value)
{
	D3DXVECTOR3	LookDirection = ZEROVEC3; // カメラ注視点の移動量.

	// マウスの移動量にマウス感度をかけてYawとPitchを出す.
	m_CameraRot.y -= value.x * m_MouseSens;
	m_CameraRot.x -= value.y * m_MouseSens;

	// カメラピッチ角を制限.
	if ( m_CameraRot.x >=  89.f  ) { m_CameraRot.x =  89.f; }
	if ( m_CameraRot.x <= -89.f  ) { m_CameraRot.x = -89.f; }
	if ( m_CameraRot.y >=  360.f ) { m_CameraRot.y =   0.f; }
	if ( m_CameraRot.y <= -360.f ) { m_CameraRot.y =   0.f; }

	// 方向ベクトルを更新.
	LookDirection.x = cos(D3DXToRadian(m_CameraRot.y)) * cos(D3DXToRadian(m_CameraRot.x));
	LookDirection.y = sin(D3DXToRadian(m_CameraRot.x));
	LookDirection.z = sin(D3DXToRadian(m_CameraRot.y)) * cos(D3DXToRadian(m_CameraRot.x));

	// 正規化.
	D3DXVec3Normalize(&LookDirection, &LookDirection);

	// カメラの位置を更新.
	m_Camera.Look = m_Camera.Pos + LookDirection;
}


//-------------------------------------------------------------------
//		カメラの移動.
//-------------------------------------------------------------------
void CCamera::CameraMove(int vec)
{
	m_DirectionPos			= ZEROVEC3; // カメラ位置～注視点のベクトルの値を初期化.
	D3DXVECTOR3 rightVector = ZEROVEC3; // 左右ベクトル用.

	// カメラ位置から注視点へのベクトルを計算する.
	m_DirectionPos = m_Camera.Look - m_Camera.Pos;

	// Y軸の値をなくし、水平方向ベクトルにする.
	m_DirectionPos.y = 0.f;

	// ノーマライズ.
	D3DXVec3Normalize(&m_DirectionPos, &m_DirectionPos);

	// 左右移動の場合.
	if(vec == Right || vec == Left)
	{
		// カメラの右方向ベクトルを計算する.
		D3DXVec3Cross(&rightVector, &m_Camera.UpVec, &m_DirectionPos);
		D3DXVec3Normalize(&rightVector, &rightVector);
	}

	// 移動方向に応じた処理をする.
	switch (vec)
	{
	case CCamera::Right:
		m_Camera.Pos  += rightVector * m_MoveValue;
		m_Camera.Look += rightVector * m_MoveValue;
		break;
	case CCamera::Left:
		m_Camera.Pos  -= rightVector * m_MoveValue;
		m_Camera.Look -= rightVector * m_MoveValue;
		break;
	case CCamera::Straight:
		m_Camera.Pos  += m_DirectionPos * m_MoveValue;
		m_Camera.Look += m_DirectionPos * m_MoveValue;
		break;
	case CCamera::Back:
		m_Camera.Pos  -= m_DirectionPos * m_MoveValue;
		m_Camera.Look -= m_DirectionPos * m_MoveValue;
		break;
	default: break;
	}
}