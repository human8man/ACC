#include "CSceneBase.h"
#include "Camera/CCamera.h"


//===================================================
//		シーンベースクラス
//===================================================
CSceneBase::CSceneBase()
	:m_hWnd		()
	,m_Light	()
	,m_mView	()
	,m_mProj	()
{
}

CSceneBase::~CSceneBase()
{
}


//===================================================
//		プロジェクション関数
//===================================================
void CSceneBase::Projection( D3DXMATRIX& Proj )
{
	// y方向の視野角。数値を大きくしたら視野が狭くなる
	float fov_y = static_cast<FLOAT>(D3DXToRadian(CCamera::GetViewAngle()));
	
	// アスペクト（幅÷高さ）
	float aspect = static_cast<FLOAT>(WND_W) / static_cast<FLOAT>(WND_H);
	float near_z = 0.1f;	// 手前
	float far_z = 1000.0f;	// 奥(値を増やすと見える距離が伸びる)

	// プロジェクション（射影）変換
	D3DXMatrixPerspectiveFovLH(
		&Proj,	// (out)プロジェクション計算結果
		fov_y,	// 視野角（FOV：Field of View）
		aspect,	// アスペクト
		near_z,	// 近いビュー平面のz値
		far_z);	// 遠いビュー平面のz値
}