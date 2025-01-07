#pragma once
#include "Common/Global.h"
#include "Common/Singleton/CSingleton.h"

class CCamera
	: public CSingleton<CCamera>
{
private:
	friend class CSingleton<CCamera>; // シングルトンクラスをフレンド宣言.

public:
	// 移動方向の分岐.
	enum Move {
		Right,
		Left,
		Straight,
		Back,
	};

	// WND_WM(M = MID)は全体で使わなさそうなのでここで宣言.
	static constexpr int WND_HM = WND_H / 2;
	static constexpr int WND_WM = WND_W / 2;
	static constexpr POINT RESETPOS = { WND_WM, WND_HM };
public:
	CCamera();
	~CCamera();

	void Update();
	void Camera(D3DXMATRIX& View) const;
	void Init();

	static bool& GetMoveMouse()	  { return GetInstance()->m_CanMoveMouse; }
	static bool& GetMoveCamera()  { return GetInstance()->m_CanMoveCamera;}
	static float& GetSens()		  { return GetInstance()->m_MouseSens;	  }
	static float& GetViewAngle()  { return GetInstance()->m_Fov;		  }
	static float& GetDefaultFov() { return GetInstance()->m_DefaultFov;	  }
	static D3DXVECTOR3& GetLook() { return GetInstance()->m_Camera.Look;  }
	static D3DXVECTOR3& GetPos()  { return GetInstance()->m_Camera.Pos;	  }
	static D3DXVECTOR3& GetVec()  { return GetInstance()->m_Camera.UpVec; }
	static D3DXVECTOR3& GetRot()  { return GetInstance()->m_CameraRot;	  }
	
	static void SetUseMouse	 (const bool& Use) { GetInstance()->m_CanMoveMouse = Use;  }
	static void SetCanMove	 (const bool& can) { GetInstance()->m_CanMoveCamera = can; }
	static void ChangeUseMouse() { GetInstance()->m_CanMoveMouse  = !GetInstance()->m_CanMoveMouse; }
	static void ChangeCanMove () { GetInstance()->m_CanMoveCamera = !GetInstance()->m_CanMoveCamera;}

	static void SetViewAngle (const float& Fov)		  { GetInstance()->m_Fov = Fov;			  }
	static void SetSens		 (const float& Sens)	  { GetInstance()->m_MouseSens = Sens;	  }
	static void SetPosition	 (const D3DXVECTOR3& Pos) { GetInstance()->m_Camera.Pos = Pos;	  }
private:
	void KeyInput();					// キー入力.
	void CameraMove(int vec);			// カメラの移動.
	void MouseMove(D3DXVECTOR2 value);	// マウスの移動量に応じてカメラの方向を変更.

private:
	CAMERA	m_Camera;			// カメラ情報.

	POINT	m_NowCurorPos;		// 現在のマウスの位置.
	POINT	m_BeforCursorPos;	// 過去のマウスの位置.

	D3DXVECTOR2 m_MouseMoveDis;	// マウスの移動した距離.
	D3DXVECTOR3 m_CameraRot;	// カメラの回転軸.
	D3DXVECTOR3 m_DirectionPos;	// カメラ座標～注視点のベクトル.

	float	m_Fov;				// 視野角.
	float	m_DefaultFov;		// デフォルトの視野角.
	float	m_MoveValue;		// 移動量.
	float	m_CameraYaw;		// カメラ角度x軸.
	float	m_CameraPitch;		// カメラ角度y軸.
	float	m_MouseSens;		// マウス感度.

	bool	m_CanMoveCamera;	// カメラのキー操作が可能か.
	bool	m_CanMoveMouse;		// マウスが自由に移動できるかどうか.
};