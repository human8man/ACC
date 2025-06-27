#pragma once

#include "Global.h"
#include "Singleton/Singleton.h"
#include "Collision/Ray/Ray.h"


class Camera
	: public Singleton<Camera>
{
public:
	// 移動方向の分岐
	enum Move {
		Right,
		Left,
		Straight,
		Back,
	};

public:
	friend class Singleton<Camera>;// シングルトンクラスをフレンド宣言
	Camera();
	~Camera();
	
	// 初期化処理
	void Init();
	// 更新処理
	void Update();
	// カメラ関数(ビュー行列計算)
	void CameraView(D3DXMATRIX& View) const;

	// カーソル位置の初期化関数
	void CursorInit();

	static bool& GetMoveMouse()		{ return GetInstance()->m_CanMoveMouse;	}	// マウスがウィンドウ外に移動可能かを取得
	static bool& GetMoveCamera()	{ return GetInstance()->m_CanMoveCamera;}	// カメラのキー操作が可能かを取得
	static float& GetSens()			{ return GetInstance()->m_MouseSens;	}	// マウス感度を取得
	static float& GetViewAngle()	{ return GetInstance()->m_Fov;			}	// 視野角を取得
	static float& GetDefaultFov()	{ return GetInstance()->m_DefaultFov;	}	// 初期視野角を取得
	static RAY&	  GetRay()			{ return GetInstance()->m_pRay;			}	// レイを取得
	static D3DXVECTOR3& GetLook()	{ return GetInstance()->m_Camera.Look;	}	// 注視点を取得
	static D3DXVECTOR3& GetPos()	{ return GetInstance()->m_Camera.Pos;	}	// 座標を取得
	static D3DXVECTOR3& GetVec()	{ return GetInstance()->m_Camera.UpVec;	}	// 情報ベクトルを取得
	static D3DXVECTOR3& GetRot()	{ return GetInstance()->m_CameraRot;	}	// 回転情報を取得
	static D3DXVECTOR3& GetCamDir()	{ return GetInstance()->m_CamDirection;	}	// 座標～注視点のベクトルの取得
	static D3DXVECTOR3& GetRayHit()	{ return GetInstance()->m_RayHitPoint;	}	// レイのあたった座標の取得
	
	static void SetUseMouse	 (const bool& Use) { GetInstance()->m_CanMoveMouse = Use;  }	// マウスがウィンドウ外に移動可能かを設定
	static void SetCanMove	 (const bool& can) { GetInstance()->m_CanMoveCamera = can; }	// カメラのキー操作が可能かを設定
	static void ChangeUseMouse() { GetInstance()->m_CanMoveMouse  = !GetInstance()->m_CanMoveMouse; }	// マウスがウィンドウ外に移動可能かを切替
	static void ChangeCanMove () { GetInstance()->m_CanMoveCamera = !GetInstance()->m_CanMoveCamera;}	// カメラのキー操作が可能かを切替

	static void SetViewAngle (const float& Fov)		  { GetInstance()->m_Fov = Fov;			}	// 視野角を設定
	static void SetSens		 (const float& Sens)	  { GetInstance()->m_MouseSens = Sens;	}	// マウス感度を設定
	static void SetPosition	 (const D3DXVECTOR3& Pos) { GetInstance()->m_Camera.Pos = Pos;	}	// カメラ座標の設定
	static void SetRayHit	 (const D3DXVECTOR3& Pos) { GetInstance()->m_RayHitPoint = Pos;	}	// レイが当たった座標の設定
	static void SetLook		 (const D3DXVECTOR3& Pos) { GetInstance()->m_Camera.Look = Pos;	}	// 注視点の設定
	static void SetRot		 (const D3DXVECTOR3& Rot) { GetInstance()->m_CameraRot	 = Rot;	}	// 回転の設定
private:
	// キー入力処理をまとめる関数
	void KeyInput();
	// カメラの移動
	void CameraMove(int vec);
	// マウスの移動量に応じてカメラの方向を変更
	void MouseMove(D3DXVECTOR2 value);

private:
	CAMERA	m_Camera;			// カメラ情報
	RAY		m_pRay;				// カメラの向きレイ

	POINT	m_WindowMid;		// 画面の中央
	POINT	m_NowCurorPos;		// 現在のマウスの位置
	POINT	m_BeforCursorPos;	// 過去のマウスの位置

	D3DXVECTOR2 m_MouseMoveDis;	// マウスの移動した距離
	D3DXVECTOR3 m_CameraRot;	// カメラの回転軸
	D3DXVECTOR3 m_DirectionPos;	// カメラ座標～注視点のベクトル
	D3DXVECTOR3 m_CamDirection;	// カメラ座標～注視点のベクトル取得用
	D3DXVECTOR3 m_RayHitPoint;	// カメラレイがあたった座標を保存する用

	float	m_Fov;				// 視野角
	float	m_DefaultFov;		// デフォルトの視野角
	float	m_MoveValue;		// 移動量
	float	m_CameraYaw;		// カメラ角度x軸
	float	m_CameraPitch;		// カメラ角度y軸
	float	m_MouseSens;		// マウス感度

	bool	m_CanMoveCamera;	// カメラのキー操作が可能か
	bool	m_CanMoveMouse;		// マウスが自由に移動できるか
};