#pragma once

//警告についてのコード分析を無効にする.4005:再定義.
#pragma warning(disable:4005)

//DirectX9.
#include <d3dx9.h>

#include "Common/Singleton/CSingleton.h"


//=================================================
//		DirectX9 セットアップ.
//=================================================
class CDirectX9
	: public CSingleton<CDirectX9>
{
private:
	friend class CSingleton<CDirectX9>; // シングルトンクラスをフレンド宣言.

public:
	CDirectX9();
	~CDirectX9();

	// DirectX9構築.
	HRESULT Create( HWND hWnd );
	// DirectX9解放.
	void Release();

	// デバイスオブジェクトを取得.
	LPDIRECT3DDEVICE9 GetDevice() const { return m_pDevice9; }


private:
	LPDIRECT3DDEVICE9	m_pDevice9;	// デバイスオブジェクト.
};