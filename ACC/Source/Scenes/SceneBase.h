#pragma once

#include "DirectX/DirectX9.h"
#include "DirectX/DirectX11.h"


class SceneBase
{
public:
	SceneBase();
	~SceneBase();
	
	virtual void	Create	()	= 0; // 作成処理
	virtual HRESULT LoadData()	= 0; // 読込処理
	virtual void	Init	()	= 0; // 初期化処理
	virtual void	Update	()	= 0; // 更新処理
	virtual void	Draw	()	= 0; // 描画処理
	virtual void	Release	()	= 0; // 解放処理

	// プロジェクション関数
	void Projection( D3DXMATRIX& Proj );

protected:
	HWND		m_hWnd;	 // ウィンドウハンドル
	LIGHT		m_Light; // ライト情報
	D3DXMATRIX	m_mView; // ビュー(カメラ)行列
	D3DXMATRIX	m_mProj; // 射影（プロジェクション）行列
};