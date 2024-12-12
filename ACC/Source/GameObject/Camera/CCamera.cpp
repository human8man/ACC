#include "CCamera.h"
#include "Common/DirectInput/CDirectInput.h"

#include "Common/Json/CJson.h"

CCamera::CCamera()
	: m_ViewAngle		( 60 )
	, m_LookDirection	()
	, m_DirectionPos	()
	, m_Playerpos		( ZEROVEC3 )
	, m_NowCurorPos		{ WND_W / 2, WND_H / 2 }
	, m_BeforCursorPos	{ WND_W / 2, WND_H / 2 }
	, m_MoveValue		( 0.1f )
	, m_CameraYaw		( 0.f )
	, m_CameraPitch		( 0.f )
	, m_MouseMoveDis	{ 0.f, 0.f }
	, m_MouseSens		( 0.03f )
	, m_MoveMouse		( false )
	, m_MousePos		()
	, m_OldMousePos		()
	, m_MouseMoveValue	()
	, m_LookMoveValue	()
{
	Init();
}

CCamera::~CCamera()
{
}

void CCamera::Update()
{
	CGamePad* GamePad = CDInput::GetInstance()->CDGamePad();

	// マウス移動量の初期化.
	InitMouseDis();
	// キー入力.
	KeyInput();

	// マウス座標の取得.
	if (!CDInput::GetInstance()->GamePadConnect())
	{
		if (GetCursorPos(&m_NowCurorPos))
		{
			ScreenToClient(GetConsoleWindow(), &m_NowCurorPos);
			// 前回のマウス位置と違う場合新しく位置を入れなおす.
			if (m_BeforCursorPos.x != m_NowCurorPos.x || m_BeforCursorPos.y != m_NowCurorPos.y)
			{
				if (!m_MoveMouse)
				{
					// マウス移動距離計算.
					MouseMath();

					// ベクトル計算をし、注視点に代入.
					MouseMove(m_MouseMoveDis.x, m_MouseMoveDis.y);

					// マウス位置を初期化.
					MousePosReset();
				}
			}
		}
	}
	else {
		m_MoveMouse = true;
	}

	// スティックによるカメラ操作.
	if (GamePad->GetPadpos().lZ  >= STIC_LOW || GamePad->GetPadpos().lZ  <= -STIC_LOW
	||  GamePad->GetPadpos().lRz >= STIC_LOW || GamePad->GetPadpos().lRz <= -STIC_LOW)
	{
		MouseMove((GamePad->GetPadpos().lZ * m_MouseSens),(GamePad->GetPadpos().lRz * m_MouseSens));
	}

	DebugCamera();
}

// カメラ関数(ビュー行列計算).
void CCamera::Camera( D3DXMATRIX& View )
{
	D3DXVECTOR3 cam_pos	 = m_Camera.Position;
	D3DXVECTOR3 cam_look = m_Camera.Look;
	D3DXVECTOR3	vUpVec	 = m_Camera.UpVec;

	// ビュー（カメラ）変換.
	D3DXMatrixLookAtLH(
		&View,	// (out)ビュー計算結果.
		&cam_pos, &cam_look, &vUpVec);
}

// 初期化.
void CCamera::Init()
{
	// 上方ベクトルの初期化.
	m_CameraRot			= ZEROVEC3;
	m_Camera.Look		= ZEROVEC3;
	m_Camera.UpVec		= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_Camera.Position	= D3DXVECTOR3(0.0f, 3.0f, 0.0f);

	m_DefaultFov = m_ViewAngle;

	MouseMove(0.f, 0.f);	// カメラの回転の初期化.
}

void CCamera::DebugCamera()
{
	//if (m_IsDebug)
	{
		// マウスでの一人称視点カメラ移動.
		MouseFirstPersonCamera();
	}
}

void CCamera::MouseFirstPersonCamera()
{
	m_MouseMoveValue = ZEROVEC3;

	if (GetCursorPos(&m_MousePos))
	{
		ScreenToClient(GetConsoleWindow(), &m_MousePos);

		// マウス位置が前回のマウス位置と違うなら.
		if (m_MousePos.x != m_OldMousePos.x
		||  m_MousePos.y != m_OldMousePos.y)
		{
			// マウスの移動量計算.
			m_MouseMoveValue.x = static_cast<float>(m_MousePos.x - m_OldMousePos.x);
			m_MouseMoveValue.y = static_cast<float>(m_MousePos.y - m_OldMousePos.y);

			m_CameraRot.x -= m_MouseMoveValue.y * 0.03f;
			m_CameraRot.y -= m_MouseMoveValue.x * 0.03f;

			m_LookMoveValue = ZEROVEC3;

			m_LookMoveValue.x = cosf(D3DXToRadian(m_CameraRot.y)) * cosf(D3DXToRadian(m_CameraRot.x));
			m_LookMoveValue.y = sinf(D3DXToRadian(m_CameraRot.x));
			m_LookMoveValue.z = sinf(D3DXToRadian(m_CameraRot.y)) * cosf(D3DXToRadian(m_CameraRot.x));

			D3DXVec3Normalize(&m_LookMoveValue, &m_LookMoveValue);

			m_Camera.Look = m_Camera.Position + m_LookMoveValue;

			// マウス位置を固定.
			if (!CDInput::GetInstance()->GamePadConnect())
			{
				SetCursorPos(WND_W / 2, WND_H / 2);
				// マウス位置を固定.
				m_OldMousePos = { WND_W / 2, WND_H / 2 };
			}

		}
	}

	// カメラの移動(注視点も同時に動かす).
	DebugCameraMove();
}

void CCamera::DebugCameraMove()
{
	// カメラの注視点から位置の方向を計算.
	D3DXVECTOR3 Direction = m_Camera.Look - m_Camera.Position;
	// 正規化.
	D3DXVec3Normalize(&Direction, &Direction);

	//--------------------キーボードのカメラ移動--------------------//
	//右方向ベクトル
	D3DXVECTOR3 RightVec;
	D3DXVec3Cross(&RightVec, &m_Camera.UpVec, &Direction);
	D3DXVec3Normalize(&RightVec, &RightVec);

	// 仮数値.
	float AddValue = 0.1f;

	// 速度増減.
	if (GetAsyncKeyState('Q') & 0x8000) { AddValue = 0.01f; }
	if (GetAsyncKeyState('E') & 0x8000) { AddValue = 1.f; }

	// 前.
	if (GetAsyncKeyState('W') & 0x8000) {
		m_Camera.Position	+= Direction * AddValue;
		m_Camera.Look		+= Direction * AddValue;
	}
	// 下.
	if (GetAsyncKeyState('S') & 0x8000)	{
		m_Camera.Position	-= Direction * AddValue;
		m_Camera.Look		-= Direction * AddValue;
	}
	// 左.
	if (GetAsyncKeyState('A') & 0x8000)	{
		m_Camera.Position	-= RightVec * AddValue;
		m_Camera.Look		-= RightVec * AddValue;
	}
	// 右.
	if (GetAsyncKeyState('D') & 0x8000)	{
		m_Camera.Position	+= RightVec * AddValue;
		m_Camera.Look		+= RightVec * AddValue;
	}
}

void CCamera::InitMouseDis()
{
	//マウスの移動量の初期化.
	m_MouseMoveDis = { 0.f, 0.f };
}

// キー入力.
void CCamera::KeyInput()
{
	CKey*	  Key	  = CDInput::GetInstance()->CDKeyboard();
	CGamePad* GamePad = CDInput::GetInstance()->CDGamePad();

	// カメラ位置の移動量の初期化.
	m_DirectionPos = ZEROVEC3;

	//if (m_IsDebug == true)
	{
		if (Key->IsKeyDown(DIK_UP))		{ CameraMove(Straight); }
		if (Key->IsKeyDown(DIK_DOWN))	{ CameraMove(Back);		}
		if (Key->IsKeyDown(DIK_RIGHT))	{ CameraMove(Left);		}
		if (Key->IsKeyDown(DIK_LEFT))	{ CameraMove(Right);	}

		// Y座標の調整.
		if (Key->IsKeyDown(DIK_SPACE))
		{
			m_Camera.Position.y += m_MoveValue;
			m_Camera.Look.y += m_MoveValue;
		}
		if (Key->IsKeyDown(DIK_LSHIFT))
		{
			m_Camera.Position.y -= m_MoveValue;
			m_Camera.Look.y -= m_MoveValue;
		}
	}

	if (Key->ISKeyAction(DIK_F2)) { m_MoveMouse = !m_MoveMouse; }

	// マウスカーソルの表示.
	ShowCursor(m_MoveMouse);
}

// マウス移動量の計算.
void CCamera::MouseMath()
{
	// マウスの移動量.
	m_MouseMoveDis.x = static_cast<float>(m_NowCurorPos.x - m_BeforCursorPos.x);
	m_MouseMoveDis.y = static_cast<float>(m_NowCurorPos.y - m_BeforCursorPos.y);
}

// マウスの移動量に応じてカメラの方向を変更.
void CCamera::MouseMove(float deltaX, float deltaY)
{
	// マウスの移動量にマウス感度をかけてYawとPitchを出す.
	m_CameraRot.y -= deltaX * m_MouseSens;
	m_CameraRot.x -= deltaY * m_MouseSens;

	// カメラピッチ角を制限.
	if ( m_CameraRot.x >=  89.f  ) { m_CameraRot.x =  89.f; }
	if ( m_CameraRot.x <= -89.f  ) { m_CameraRot.x = -89.f; }
	if ( m_CameraRot.y >=  360.f ) { m_CameraRot.y =   0.f; }
	if ( m_CameraRot.y <= -360.f ) { m_CameraRot.y =   0.f; }

	// カメラ注視点の移動量の初期化.
	m_LookDirection = ZEROVEC3;

	// 方向ベクトルを更新.
	m_LookDirection.x = cos(D3DXToRadian(m_CameraRot.y)) * cos(D3DXToRadian(m_CameraRot.x));
	m_LookDirection.y = sin(D3DXToRadian(m_CameraRot.x));
	m_LookDirection.z = sin(D3DXToRadian(m_CameraRot.y)) * cos(D3DXToRadian(m_CameraRot.x));

	// ノーマライズ(正常化).
	//	 ベクトルの大きさを1に固定すること.
	D3DXVec3Normalize(&m_LookDirection, &m_LookDirection);

	// カメラの位置を更新.
	m_Camera.Look = m_Camera.Position + m_LookDirection;
}

// カメラの移動.
void CCamera::CameraMove(int vec)
{
	// カメラ注視点の移動量を初期化.
	m_DirectionPos = ZEROVEC3;

	// カメラの真横への移動.
	// カメラの注視点方向へベクトルを計算する.
	m_DirectionPos = m_Camera.Look - m_Camera.Position;
	// ノーマライズ.
	D3DXVec3Normalize(&m_DirectionPos, &m_DirectionPos);

	// 移動方向によって違う処理にする.
	switch (vec)
	{
	// カメラ位置を左右させる.
	case CCamera::Right:
	case CCamera::Left:
	{
		// カメラの右方向ベクトルを計算する.
		D3DXVECTOR3 rightVector;
		D3DXVec3Cross(&rightVector, &m_Camera.UpVec, &m_DirectionPos);
		D3DXVec3Normalize(&rightVector, &rightVector);

		// カメラ位置を右方向に移動する.
		if (vec == Right) {
			m_Camera.Position -= rightVector * m_MoveValue;
			m_Camera.Look	  -= rightVector * m_MoveValue;
		}
		// カメラ位置を左方向に移動する.
		else {
			m_Camera.Position += rightVector * m_MoveValue;
			m_Camera.Look	  += rightVector * m_MoveValue;
		}
	}
	break;
	case CCamera::Straight:
		m_Camera.Position += m_DirectionPos * m_MoveValue;
		m_Camera.Look	  += m_DirectionPos * m_MoveValue;
		break;
	case CCamera::Back:
		m_Camera.Position -= m_DirectionPos * m_MoveValue;
		m_Camera.Look	  -= m_DirectionPos * m_MoveValue;
		break;
	default:
		break;
	}
}

// マウス位置の初期化.
void CCamera::MousePosReset()
{
	// マウスカーソルを中央に固定.
	SetCursorPos(WND_W / 2, WND_H / 2);

	// 現在のマウス位置を退避.
	m_BeforCursorPos = { WND_W / 2, WND_H / 2 };
}

// カーソル位置.
void CCamera::MousePos(HDC hdc, POINT pos)
{
	int r = 10;
	::MoveToEx(hdc, pos.x - r, pos.y, NULL);
	::LineTo(hdc, pos.x + r, pos.y);

	::MoveToEx(hdc, pos.x, pos.y - r, NULL);
	::LineTo(hdc, pos.x, pos.y + r);
}
