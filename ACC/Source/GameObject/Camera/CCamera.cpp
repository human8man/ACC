#include "CCamera.h"
#include "Common/DirectInput/CDirectInput.h"

#include "Common/Json/CJson.h"

#ifdef _DEBUG
#include "Common/ImGui/CImGui.h"
#endif


CCamera::CCamera()
	: m_ViewAngle(60)
	, m_LookDirection()
	, m_DirectionPos()
	, m_Playerpos(0.0f, 0.0f, 0.0f)

	, m_NowCurorPos{ WND_W / 2, WND_H / 2 }
	, m_BeforCursorPos{ WND_W / 2, WND_H / 2 }

	, m_MoveValue(0.1f)

	, m_CameraYaw(0.0f)
	, m_CameraPitch(0.0f)

	, m_MouseMoveDis{ 0.0f, 0.0f }
	, m_MouseSens(0.03f)

	, m_MoveMouse(false)

	, m_IsDebug(false)

	, m_MousePos()
	, m_OldMousePos()

	, m_MouseMoveVolume()
	, m_LookMoveVolume()
{
	Init();
}

CCamera::~CCamera()
{
}

void CCamera::Update()
{
	CGamePad* GamePad = CDInput::GetInstance()->CDGamePad();

	//マウス移動量の初期化.
	InitMouseDis();
	//キー入力.
	KeyInput();

	if (m_IsDebug == false)
	{
		m_Camera.Position = m_Playerpos;		//スライディング以外の時はカメラの位置をプレイヤーの位置にセット.
		if (m_IsSliding == false)
		{
			m_Camera.Position.y = m_Playerpos.y + 0.5f;
		}
		else
		{
			m_Camera.Position.y = m_Playerpos.y;
		}

		//注視点を固定.
		m_Camera.Look = m_Camera.Position + m_LookDirection;
	}

	//マウス座標の取得.
	if (!CDInput::GetInstance()->GamePadConnect())
	{
		if (GetCursorPos(&m_NowCurorPos))
		{
			ScreenToClient(GetConsoleWindow(), &m_NowCurorPos);
			//前回のマウス位置と違う場合新しく位置を入れなおす.
			if (m_BeforCursorPos.x != m_NowCurorPos.x || m_BeforCursorPos.y != m_NowCurorPos.y)
			{
				if (!m_MoveMouse)
				{
					//マウス移動距離計算.
					MouseMath();

					//ベクトル計算をし、注視点に代入.
					MouseMove(m_MouseMoveDis.x, m_MouseMoveDis.y);

					//マウス位置を初期化.
					MousePosReset();
				}
			}
		}
	}
	else
	{
		m_MoveMouse = true;
	}

	//スティックによるカメラ操作.
	if (GamePad->GetPadpos().lZ >= STIC_LOW || GamePad->GetPadpos().lZ <= -STIC_LOW
		|| GamePad->GetPadpos().lRz >= STIC_LOW || GamePad->GetPadpos().lRz <= -STIC_LOW)
	{
		MouseMove
		(
			(GamePad->GetPadpos().lZ * m_MouseSens),
			(GamePad->GetPadpos().lRz * m_MouseSens)
		);
	}

#ifdef _DEBUG
	ImGui::Begin("PlayerWindow");

	ImGui::Text("Look:%f", &m_Camera.Look);
	ImGui::Text("Pos:%f", &m_Camera.Position);
	ImGui::Text("Vec:%f", &m_Camera.UpVec);
	ImGui::Text("direction:%f", &m_DirectionPos);
	ImGui::Text("Rot:%f", &m_CameraRot);

	ImGui::End();

#endif

}

//カメラ関数(ビュー行列計算)
void CCamera::Camera(
	D3DXMATRIX& View)
{
	//自身のインスタンス
	auto* This = CCamera::GetInstance();

	D3DXVECTOR3 cam_pos = This->m_Camera.Position;
	D3DXVECTOR3 cam_look = This->m_Camera.Look;
	D3DXVECTOR3	vUpVec = This->m_Camera.UpVec;	//上方（ベクトル）.

	//ビュー（カメラ）変換.
	D3DXMatrixLookAtLH(
		&View,	//(out)ビュー計算結果.
		&cam_pos, &cam_look, &vUpVec);
}

//壁走り時のカメラ
void CCamera::WallRunCamera()
{
	m_Camera.UpVec = D3DXVECTOR3(D3DXToRadian(25.0f), 1.0f, 0.0f);
	//m_Camera.Position 
}

//カメラの向きが一定の向きになったら
bool CCamera::CameraDirConstancy()
{
	if (static_cast<int>(m_CameraRot.y) % 90 == 0)
	{
		return true;
	}
	else if (m_CameraRot.x >= 75.0f)
	{
		return true;
	}
	return false;
}

void CCamera::GetPlayerInfo(D3DXVECTOR3 Pos)
{
	m_Playerpos = Pos;
}

void CCamera::SetPosX(D3DXVECTOR3 Playerpos, bool flag)
{
	if (flag == true)
	{
		m_Camera.Position.x = Playerpos.x + 0.5f;
	}
	else if (flag == false)
	{
		m_Camera.Position.x = Playerpos.x;
	}
}

void CCamera::SetPosZ(D3DXVECTOR3 Playerpos, bool RLflag, bool flag)
{
	if (flag == true)
	{
		if (RLflag == true)
		{
			m_Camera.Position.z = Playerpos.z + 0.5f;
		}
		else if (RLflag == false)
		{
			m_Camera.Position.z = Playerpos.z - 0.5f;
		}
	}
	else if (flag == false)
	{
		m_Camera.Position.z = Playerpos.z;
	}
}

//初期化.
void CCamera::Init()
{
	//上方ベクトルの初期化
	m_Camera.Position = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
	m_Camera.Look = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Camera.UpVec = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_CameraRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_DefaultFov = m_ViewAngle;

	MouseMove(0.0f, 0.0f);		//カメラの回転の初期化.
}

void CCamera::DebugCamera()
{
	if (m_IsDebug)
	{
		//マウスでの一人称視点カメラ移動
		MouseFirstPersonCamera();
	}
}

void CCamera::MouseFirstPersonCamera()
{
	m_MouseMoveVolume = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (GetCursorPos(&m_MousePos))
	{
		ScreenToClient(GetConsoleWindow(), &m_MousePos);

		//マウス位置が前回のマウス位置と違うなら
		if (m_MousePos.x != m_OldMousePos.x
			|| m_MousePos.y != m_OldMousePos.y)
		{
			//マウスの移動量計算
			m_MouseMoveVolume.x = static_cast<float>(m_MousePos.x - m_OldMousePos.x);
			m_MouseMoveVolume.y = static_cast<float>(m_MousePos.y - m_OldMousePos.y);

			m_CameraRot.x -= m_MouseMoveVolume.y * 0.03f;
			m_CameraRot.y -= m_MouseMoveVolume.x * 0.03f;

			m_LookMoveVolume = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			m_LookMoveVolume.x = cosf(D3DXToRadian(m_CameraRot.y)) * cosf(D3DXToRadian(m_CameraRot.x));
			m_LookMoveVolume.y = sinf(D3DXToRadian(m_CameraRot.x));
			m_LookMoveVolume.z = sinf(D3DXToRadian(m_CameraRot.y)) * cosf(D3DXToRadian(m_CameraRot.x));

			D3DXVec3Normalize(&m_LookMoveVolume, &m_LookMoveVolume);

			m_Camera.Look = m_Camera.Position + m_LookMoveVolume;

			//マウス位置を固定
			if (!CDInput::GetInstance()->GamePadConnect())
			{
				SetCursorPos(WND_W / 2, WND_H / 2);
				//マウス位置を固定
				m_OldMousePos = { WND_W / 2, WND_H / 2 };
			}

		}
	}

	//カメラの移動(注視点も同時に動かす)
	DebugCameraMove();
}

void CCamera::DebugCameraMove()
{
	//カメラの注視点から位置の方向を計算
	D3DXVECTOR3 Direction = m_Camera.Look - m_Camera.Position;
	//正規化
	D3DXVec3Normalize(&Direction, &Direction);

	//--------------------キーボードのカメラ移動---------------//
	//右方向ベクトル
	D3DXVECTOR3 RightVec;
	D3DXVec3Cross(&RightVec, &m_Camera.UpVec, &Direction);
	D3DXVec3Normalize(&RightVec, &RightVec);

	//仮数値
	float AddValue = 0.1f;

	//速度増減.
	if (GetAsyncKeyState('Q') & 0x8000) { AddValue = 0.01f; }
	if (GetAsyncKeyState('E') & 0x8000) { AddValue = 1.f; }

	//前方
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_Camera.Position += Direction * AddValue;
		m_Camera.Look += Direction * AddValue;
	}
	//下方
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_Camera.Position -= Direction * AddValue;
		m_Camera.Look -= Direction * AddValue;
	}
	//左方
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_Camera.Position -= RightVec * AddValue;
		m_Camera.Look -= RightVec * AddValue;
	}
	//右方
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_Camera.Position += RightVec * AddValue;
		m_Camera.Look += RightVec * AddValue;
	}
}

void CCamera::InitMouseDis()
{
	//マウスの移動量の初期化.
	m_MouseMoveDis = { 0.0f, 0.0f };
}

//キー入力.
void CCamera::KeyInput()
{
	CKey* Key = CDInput::GetInstance()->CDKeyboard();
	CGamePad* GamePad = CDInput::GetInstance()->CDGamePad();

	if (m_IsDebug == false)
	{
		m_Camera.Position = m_Playerpos;
	}


	//カメラ位置の移動量の初期化.
	m_DirectionPos = { 0.0f, 0.0f, 0.0f };

	//Z座標の調整.
	if (m_IsDebug == true)
	{
		if (Key->IsKeyDown(DIK_UP))
		{
			CameraMove(Straight);
		}
		if (Key->IsKeyDown(DIK_DOWN))
		{
			CameraMove(Back);
		}
		//X座標の調整.
		if (Key->IsKeyDown(DIK_RIGHT))
		{
			CameraMove(Left);
		}
		if (Key->IsKeyDown(DIK_LEFT))
		{
			CameraMove(Right);
		}
		//Y座標の調整.
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

	//マウスカーソルの表示.
	ShowCursor(m_MoveMouse);
}

//マウス移動量の計算.
void CCamera::MouseMath()
{
	//マウスの移動量.
	m_MouseMoveDis.x = static_cast<float>(m_NowCurorPos.x - m_BeforCursorPos.x);
	m_MouseMoveDis.y = static_cast<float>(m_NowCurorPos.y - m_BeforCursorPos.y);
}

//マウスの移動量に応じてカメラの方向を変更.
void CCamera::MouseMove(float deltaX, float deltaY)
{
#if 0
	//マウスの移動量にマウス感度をかけてYawとPitchを出す.
	m_CameraYaw -= deltaX * m_MouseSens;
	m_CameraPitch -= deltaY * m_MouseSens;

	//カメラピッチ角を制限.
	if (m_CameraPitch >= 89.0f)
	{
		m_CameraPitch = 89.0f;
	}
	if (m_CameraPitch <= -89.0f)
	{
		m_CameraPitch = -89.0f;
	}

	//カメラ注視点の移動量の初期化.
	m_LookDirection = { 0.0f, 0.0f, 0.0f };
	//方向ベクトルを更新.
	m_LookDirection.x = cos(D3DXToRadian(m_CameraYaw)) * cos(D3DXToRadian(m_CameraPitch));
	m_LookDirection.y = sin(D3DXToRadian(m_CameraPitch));
	m_LookDirection.z = sin(D3DXToRadian(m_CameraYaw)) * cos(D3DXToRadian(m_CameraPitch));

	//ノーマライズ(正常化).
	//	ベクトルの大きさを1に固定すること.
	D3DXVec3Normalize(&m_LookDirection, &m_LookDirection);

	//カメラの位置を更新.
	m_Camera.Look = m_Camera.Position + m_LookDirection;
#else
	//マウスの移動量にマウス感度をかけてYawとPitchを出す.
	m_CameraRot.y -= deltaX * m_MouseSens;
	m_CameraRot.x -= deltaY * m_MouseSens;

	//カメラピッチ角を制限.
	if (m_CameraRot.x >= 89.0f)
	{
		m_CameraRot.x = 89.0f;
	}
	if (m_CameraRot.x <= -89.0f)
	{
		m_CameraRot.x = -89.0f;
	}
	if (m_CameraRot.y >= 360.0f)
	{
		m_CameraRot.y = 0.0f;
	}
	if (m_CameraRot.y <= -360.0f)
	{
		m_CameraRot.y = 0.0f;
	}

	//カメラ注視点の移動量の初期化.
	m_LookDirection = { 0.0f, 0.0f, 0.0f };
	//方向ベクトルを更新.
	m_LookDirection.x = cos(D3DXToRadian(m_CameraRot.y)) * cos(D3DXToRadian(m_CameraRot.x));
	m_LookDirection.y = sin(D3DXToRadian(m_CameraRot.x));
	m_LookDirection.z = sin(D3DXToRadian(m_CameraRot.y)) * cos(D3DXToRadian(m_CameraRot.x));

	//ノーマライズ(正常化).
	//	ベクトルの大きさを1に固定すること.
	D3DXVec3Normalize(&m_LookDirection, &m_LookDirection);

	//カメラの位置を更新.
	m_Camera.Look = m_Camera.Position + m_LookDirection;

#endif
}

//カメラの移動.
void CCamera::CameraMove(int vec)
{
	//カメラ注視点の移動量を初期化.
	m_DirectionPos = { 0.0f, 0.0f, 0.0f };

	//カメラの真横への移動.
	//カメラの注視点方向へベクトルを計算する.
	m_DirectionPos = m_Camera.Look - m_Camera.Position;
	//ノーマライズ.
	D3DXVec3Normalize(&m_DirectionPos, &m_DirectionPos);

	//移動方向によって違う処理にする.
	switch (vec)
	{//カメラ位置を左右させる.
	case CCamera::Right:
	case CCamera::Left:
	{
		//カメラの右方向ベクトルを計算する.
		D3DXVECTOR3 rightVector;
		D3DXVec3Cross(&rightVector, &m_Camera.UpVec, &m_DirectionPos);
		D3DXVec3Normalize(&rightVector, &rightVector);

		//カメラ位置を右方向に移動する.
		if (vec == Right)
		{
			m_Camera.Position -= rightVector * m_MoveValue;
			m_Camera.Look -= rightVector * m_MoveValue;
		}
		//カメラ位置を左方向に移動する.
		else
		{
			m_Camera.Position += rightVector * m_MoveValue;
			m_Camera.Look += rightVector * m_MoveValue;
		}
	}
	break;
	case CCamera::Straight:
		m_Camera.Position += m_DirectionPos * m_MoveValue;
		m_Camera.Look += m_DirectionPos * m_MoveValue;
		break;
	case CCamera::Back:
		m_Camera.Position -= m_DirectionPos * m_MoveValue;
		m_Camera.Look -= m_DirectionPos * m_MoveValue;
		break;
	default:
		break;
	}
}

//マウス位置の初期化.
void CCamera::MousePosReset()
{
	//マウスカーソルを中央に固定.
	SetCursorPos(WND_W / 2, WND_H / 2);

	//現在のマウス位置を退避.
	m_BeforCursorPos = { WND_W / 2, WND_H / 2 };
}

//カーソル位置.
void CCamera::MousePos(HDC hdc, POINT pos)
{
	int r = 10;
	::MoveToEx(hdc, pos.x - r, pos.y, NULL);
	::LineTo(hdc, pos.x + r, pos.y);

	::MoveToEx(hdc, pos.x, pos.y - r, NULL);
	::LineTo(hdc, pos.x, pos.y + r);
}
