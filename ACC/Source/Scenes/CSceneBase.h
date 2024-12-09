#pragma once
#include "DirectX/CDirectX9.h"
#include "DirectX/CDirectX11.h"

//===================================================
//		シーンベースクラス.
//===================================================
class CSceneBase
{
public:
	CSceneBase();
	~CSceneBase();
	
	virtual void	Create	()	= 0; // 構築.
	virtual HRESULT LoadData()	= 0; // データ読込.
	virtual void	Init	()	= 0; // 初期化.	
	virtual void	Update	()	= 0; // 更新.
	virtual void	Draw	()	= 0; // 描画.
	virtual void	Release	()	= 0; // 破棄.

	// プロジェクション関数.
	void Projection( D3DXMATRIX& Proj );

protected:
	HWND		m_hWnd;		// ウィンドウハンドル.
	CAMERA		m_Camera;	// カメラ情報.
	LIGHT		m_Light;	// ライト情報.
	D3DXMATRIX	m_mView;	// ビュー(カメラ)行列.
	D3DXMATRIX	m_mProj;	// 射影（プロジェクション）行列.
};