#pragma once

// 警告についてのコード分析を無効にする
#pragma warning(disable:4005)

#include <d3dx9.h>
#include "Singleton/Singleton.h"


class DirectX9
	: public Singleton<DirectX9>
{
private:
	friend class Singleton<DirectX9>;

public:
	DirectX9();
	~DirectX9();

	// 作成処理
	HRESULT Create( HWND hWnd );
	// 解放処理
	void Release();

	// デバイスオブジェクトを取得
	LPDIRECT3DDEVICE9 GetDevice() const { return m_pDevice9; }

private:
	LPDIRECT3DDEVICE9	m_pDevice9;	// デバイスオブジェクト
};