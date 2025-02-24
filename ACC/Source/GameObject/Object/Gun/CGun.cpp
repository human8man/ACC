#include "CGun.h"


//============================================================================
//		銃クラス.
//============================================================================
CGun::CGun()
	:m_ShootPoint	(0.1f, 3.1f, 0.1f )
{
}

CGun::~CGun()
{
}


//============================================================================
//		更新処理.
//============================================================================
void CGun::Update()
{
}


//============================================================================
//		描画処理.
//============================================================================
void CGun::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light)
{
	// オブジェクトの描画.
	CStaticMeshObject::Draw( View, Proj, Light );
}


//============================================================================
//		銃の位置をプレイヤーに合わせて変更する.
//============================================================================
void CGun::UpdateGunPos(const D3DXVECTOR3& center, float radius, float playerYaw)
{
	// 初期位置（プレイヤーの右方向にradius分だけ離れた位置）.
	D3DXVECTOR3 initialPosition(radius, 0.f, 0.f);
	D3DXVECTOR3 Upvec(0.f, -1.f, 0.f);	// 逆回転にする.

	// プレイヤーのyaw回転を表すクォータニオン.
	D3DXQUATERNION playerYawQuat;
	D3DXQuaternionRotationAxis(&playerYawQuat, &Upvec, playerYaw);

	// クォータニオンを回転行列に変換.
	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationQuaternion(&rotationMatrix, &playerYawQuat);

	// 初期位置を回転して最終的な位置を計算.
	D3DXVECTOR3 rotatedPosition;
	D3DXVec3TransformCoord(&rotatedPosition, &initialPosition, &rotationMatrix);
	rotatedPosition.y += 2.f;

	// プレイヤーの位置を基準に最終位置を計算.
	m_vPosition = center + rotatedPosition;

	// 銃口の相対位置を指定.
	// X: 銃の右方向（銃の幅が必要なら考慮）、Y: 銃の高さ、Z: 銃の前方向（銃身の長さ）.
	D3DXVECTOR3 barrelOffset(3.f, 0.7f, 1.f);

	// オフセットを銃の回転に基づいて回転させる.
	D3DXVECTOR3 rotatedBarrelOffset;
	D3DXVec3TransformCoord(&rotatedBarrelOffset, &barrelOffset, &rotationMatrix);

	// 銃口の最終位置を計算.
	m_ShootPoint = m_vPosition + rotatedBarrelOffset;
}
