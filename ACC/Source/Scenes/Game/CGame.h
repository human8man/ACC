#pragma once
#include "Scenes/CSceneBase.h"
#include "DirectX/CDirectX9.h"
#include "DirectX/CDirectX11.h"

#include "DebugText/CDebugText.h"
#include "Collision/GJK/CGJK.h"


class CStaticMesh;
class CGJK;
class CRay;
class MeshCollider;
class CGround;
class CBullet;
class CGun;
class CEnemy;
class CPlayer;
class CCharacter;

//============================================================================
//		ゲームクラス.
//============================================================================
class CGame
	: public CSceneBase
{
public:
	CGame( HWND hWnd );
	~CGame();

	void Create()	override;
	HRESULT LoadData()override;
	void Release()	override;
	void Init()		override;
	void Update()	override;
	void Draw()		override;


private:
	// 当たり判定関数.
	void CollisionJudge();
	void UpdateSatellitePosition(
		const D3DXVECTOR3& center,	// Aの位置.
		D3DXVECTOR3& pos,			// Bの位置（out）.
		float radius,				// AからBまでの距離.
		float angle					// 回転角度（度数法）.
	);

	void UpdateGunPosition(
		const D3DXVECTOR3& center,
		D3DXVECTOR3& pos,
		float radius,
		float playerYaw);

private:
	HWND	m_hWnd;	// ウィンドウハンドル.
	LIGHT	m_Light;// ライト情報.

	D3DXMATRIX	m_mView;	// ビュー(カメラ)行列.
	D3DXMATRIX	m_mProj;	// 射影（プロジェクション）行列.

	CStaticMesh*	m_pMeshFighter;	// 自機.
	CStaticMesh*	m_pMeshGround;	// 地面.
	CStaticMesh*	m_pMeshBullet;	// 弾.
	CStaticMesh*	m_pMeshGun;	// 弾.
		
	CCharacter*	m_pPlayer;
	CGround*	m_pGround;
	CGun*		m_pGun;

	// 弾配列.
	std::vector<CBullet*>	m_pBullets;


	// GJKクラス.
	std::unique_ptr<CGJK> m_pGJK;
	MeshCollider m_MeshA;
	MeshCollider m_MeshB;

	CRay* m_pCamRay;

	// カメラに位置を渡す際にプレイヤーのサイズを足す.
	D3DXVECTOR3 m_PLayerSize;

	float m_Angle;
};