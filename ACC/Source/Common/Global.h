#pragma once

// 警告についてのｺｰﾄﾞ分析を無効にする.4005:再定義
#pragma warning(disable:4005)
#include <Windows.h>
#include <crtdbg.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <unordered_map>

#pragma warning(push)
#pragma warning(disable: 26495 ) // 警告を無効にする

#include <D3DX9.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <D3D11.h>
#include <D3D10.h>

#include "MyMacro.h"

// ライブラリ読み込み
#pragma comment( lib, "winmm.lib"  )
#pragma comment( lib, "d3dx9.lib"  )
#pragma comment( lib, "d3d9.lib"   )
#pragma comment( lib, "d3dx10.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib"  )

#pragma warning(pop)

#define DEBUGMODE


//=============================================================================
//		定数
//=============================================================================
constexpr int WND_W = 1280;		 // デフォルトウィンドウの幅(int)
constexpr int WND_H = 720;		 // デフォルトウィンドウの高さ(int)
constexpr float FWND_W = 1280.f; // デフォルトウィンドウの幅(float)
constexpr float FWND_H = 720.f;	 // デフォルトウィンドウの高さ(float)

// 画面倍率を図るためfloatのみ、かつ、動的に変更されるためexternで宣言
extern float NOWFWND_W;	// フルウィンドウの幅(float)
extern float NOWFWND_H;	// フルウィンドウの高さ(float)

// フルスクリーン(毎フレーム判定をとり、別のクラス同士で参照させるためextern)
extern bool FULLSCREEN;
extern bool RAINBOW_WINDOW;
// フルスクリーン時のUI調整用の変数
extern float FULLSCREENSCALEX;
extern float FULLSCREENSCALEY;
// ウィンドウ位置
extern D3DXVECTOR2 WINDOWRECT;
// ウィンドウの描画開始位置
extern D3DXVECTOR2 CLIENTRECT;

constexpr int FPS   = 60;	 // フレームレート
constexpr int STIC_LOW = 60; // スティック入力のデッドゾーン

// 0にする定数(初期化などに使用)
constexpr float ZEROFLOAT3[3] = { 0.f,0.f,0.f };
constexpr float ZEROFLOAT2[2] = { 0.f,0.f };

const D3DXVECTOR3 ZEROVEC3( 0.f,0.f,0.f );
const D3DXVECTOR2 ZEROVEC2( 0.f,0.f );

// デバッグ切り替え
extern bool ISDEBUG;
// UIeditorSceneで編集があった際に使う
extern bool UIEDIOR_MOVEANY;

// ゲームとタイトルに共通敵数の引用に使用
extern int ENEMY_COUNT;
extern int ENEMY_COUNTMAX;

//=============================================================================
//		構造体
//=============================================================================
// カメラ情報
struct CAMERA {
	D3DXVECTOR3	Pos;	// 視点
	D3DXVECTOR3 Look;	// 注視点
	D3DXVECTOR3 UpVec;	// 情報ベクトル
};

// ライト情報
struct LIGHT {
	D3DXVECTOR3	Position;			// 位置
	D3DXVECTOR3	vDirection;			// 方向
	D3DXMATRIX	mRotation;			// 回転行列
	float		Intensity = 0.5;	// 強度(明るさ)
};

// オブジェクトの座標、回転、サイズをまとめた構造体
struct ObjectInfo {
	D3DXVECTOR3 pos;
	D3DXVECTOR3 rot;
	D3DXVECTOR3 scale;
};