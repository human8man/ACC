#pragma once
#include "Common/Global.h"
#include "Common/Singleton/CSingleton.h"

//視錐台構造体
struct Frustum
{
	D3DXPLANE Planes[6]; //6つの平面:上下左右近遠
};

class CCamera
	: public CSingleton<CCamera>
{
private:
	friend class CSingleton<CCamera>; // シングルトンクラスをフレンド宣言.
	
	// 重力.
	static constexpr float Gravity = 1.0f;
public:
	// 移動方向の分岐.
	enum Move {
		Right,
		Left,
		Straight,
		Back,
	};
	// マウス座標をfloatで保存する用.
	struct Point {
		float x;
		float y;
	};

public:
	CCamera();
	~CCamera();

	void Update();
	void Camera(D3DXMATRIX& View);
	void Init();

	//デバッグ用カメラ
	void DebugCamera();
	//デバッグ用カメラの視点移動と移動
	void MouseFirstPersonCamera();
	void DebugCameraMove();

	//壁走り時のカメラ
	void WallRunCamera();
	//カメラの向きが一定の向きになったら
	bool CameraDirConstancy();

	void GetPlayerInfo(D3DXVECTOR3 Pos);

	D3DXVECTOR3 GetLook() { return m_Camera.Look; }
	D3DXVECTOR3& GetPos() { return m_Camera.Position; }
	D3DXVECTOR3 GetVec() { return m_Camera.UpVec; }
	
	//カメラ感度を取得
	float GetSens() const { return m_MouseSens; }
	//カメラ感度を設定
	void SetSens(float Value) { m_MouseSens = Value; }
	
	//通常視野角を取得
	float GetDefaultFov() const { return m_DefaultFov; }

	//位置をセット
	static void SetPosition(const D3DXVECTOR3& Pos) { GetInstance()->m_Camera.Position = Pos; };
	//カメラの高さをセット.
	static void SetPositionY(const float& PosY) { GetInstance()->m_Camera.Position.y = PosY; }

	void SetVec(D3DXVECTOR3 UpVec) { m_Camera.UpVec = UpVec; }

	//カメラの回転量を取得
	static D3DXVECTOR3& GetRot() { return GetInstance()->m_CameraRot; }

	//視野角を取得
	static float GetViewAngle() { return GetInstance()->m_ViewAngle; }
	//視野角をセット
	static void SetViewAngle(float Angle) { GetInstance()->m_ViewAngle = Angle; }
	//通常視野角の設定
	void SetDafualtFov(float ViewAngle) { m_DefaultFov = ViewAngle; }

	void SetDebgFlg(bool IsDebg) { m_IsDebug = IsDebg; }

	void SetPosX(D3DXVECTOR3 Playerpos, bool flag);

	void SetPosZ(D3DXVECTOR3 Playerpos, bool RLflag, bool flag);

	void SetIsSliding(bool IsSliding) { m_IsSliding = IsSliding; }

private:
	//マウスの移動量の初期化.
	void InitMouseDis();
	//キー入力.
	void KeyInput();
	//マウス移動量の計算.
	void MouseMath();
	//マウスの移動量に応じてカメラの方向を変更.
	void MouseMove(float deltaX, float deltaY);
	//カメラの移動.
	void CameraMove(int vec);
	//マウス位置の初期化.
	void MousePosReset();
	//カーソル位置.
	void MousePos(HDC hdc, POINT pos);


private:
	//カメラ情報.
	CAMERA			m_Camera;
	//視野角
	float m_ViewAngle;

	D3DXVECTOR3     m_LookDirection;    //カメラ注視点の移動量.
	D3DXVECTOR3     m_DirectionPos;     //カメラ位置の移動量.
	D3DXVECTOR3     m_Playerpos;        //外部からのプイレイヤー座標を保存する用.

	//マウス位置情報.
	POINT           m_NowCurorPos;      //現在のマウスの位置.
	POINT           m_BeforCursorPos;   //マウス位置の避難所.

	float           m_MoveValue;        //移動量.

	float           m_CameraYaw;        //カメラ角度x軸
	float           m_CameraPitch;      //カメラ角度y軸.
	D3DXVECTOR3     m_CameraRot;        //カメラの回転軸

	Point           m_MouseMoveDis;     //マウスの移動した距離.
	float           m_MouseSens;        //マウス感度.

	bool            m_MoveMouse;        //マウスデバック用.

	bool            m_IsDebug;
	bool            m_IsSliding;

	float m_DefaultFov; //デフォルトの視野角

	//デバッグカメラ用
	POINT m_MousePos;    //今のマウス位置
	POINT m_OldMousePos; //前のマウス位置

	D3DXVECTOR3 m_MouseMoveVolume; //マウスの移動量
	D3DXVECTOR3 m_LookMoveVolume; //注視点の移動量
};