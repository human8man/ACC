#pragma once

// 警告についてのｺｰﾄﾞ分析を無効にする.4005:再定義.
#pragma warning(disable:4005)
#include <Windows.h>
#include <crtdbg.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <memory>

#pragma warning(push)
#pragma warning(disable: 26495 ) // 警告を無効にする.

#include <D3DX9.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <D3D11.h>
#include <D3D10.h>

#include "MyMacro.h"

// ライブラリ読み込み.
#pragma comment( lib, "winmm.lib"  )
#pragma comment( lib, "d3dx9.lib"  )
#pragma comment( lib, "d3d9.lib"   )
#pragma comment( lib, "d3dx10.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib"  )

#pragma warning(pop)

#define DEBUGMODE


//=================================================
//		定数.
//=================================================
constexpr int WND_W = 1280;	 // ウィンドウの幅.
constexpr int WND_H = 720;	 // ウィンドウの高さ.
constexpr int FPS   = 60;	 // フレームレート.
constexpr int STIC_LOW = 60; // スティック入力のデッドゾーン.

// 0にする定数(初期化などに使用).
const D3DXVECTOR3 ZEROVEC3( 0.f,0.f,0.f );
const D3DXVECTOR2 ZEROVEC2( 0.f,0.f );

//=================================================
//		構造体.
//================================================= 

// カメラ情報.
struct CAMERA {
	D3DXVECTOR3	Pos;	// 視点.
	D3DXVECTOR3 Look;	// 注視点.
	D3DXVECTOR3 UpVec;	// 情報ベクトル.
};

// ライト情報.
struct LIGHT {
	D3DXVECTOR3	Position;	//位置.
	D3DXVECTOR3	vDirection;	//方向.
	D3DXMATRIX	mRotation;	//回転行列.
	float		Intensity;	//強度(明るさ).
};