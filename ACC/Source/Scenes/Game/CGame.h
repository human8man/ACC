#pragma once
#include "Scenes/CSceneBase.h"
#include "DirectX/CDirectX9.h"
#include "DirectX/CDirectX11.h"

#include "DebugText/CDebugText.h"
#include "Collision/GJK/CGJK.h"


class CStaticMesh;
class CGJK;
class MeshCollider;
class CGround;
class CBullet;
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
	CBullet*	m_pBullet;	// 弾配列に入れるデータを保存する用.

	// 弾配列.
	std::vector<CBullet*>	m_pBullets;


	// GJKクラス.
	std::unique_ptr<CGJK> m_pGJK;
	MeshCollider m_MeshA;
	MeshCollider m_MeshB;

	// カメラに位置を渡す際にプレイヤーのサイズを足す.
	D3DXVECTOR3 m_PLayerSize;
};