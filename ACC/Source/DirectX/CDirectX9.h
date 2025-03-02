#pragma once

// 警告についてのコード分析を無効にする.
#pragma warning(disable:4005)

#include <d3dx9.h>
#include "Singleton/CSingleton.h"


//===================================================================
//		DirectX9クラス.
//===================================================================
class CDirectX9
	: public CSingleton<CDirectX9>
{
private:
	friend class CSingleton<CDirectX9>;

public:
	CDirectX9();
	~CDirectX9();

	// 作成処理.
	HRESULT Create( HWND hWnd );
	// 解放処理.
	void Release();

	// デバイスオブジェクトを取得.
	LPDIRECT3DDEVICE9 GetDevice() const { return m_pDevice9; }

private:
	LPDIRECT3DDEVICE9	m_pDevice9;	// デバイスオブジェクト.
};