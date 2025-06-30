#include "Global.h"

// ここで再定義し初期化
bool	FULLSCREEN = false;
bool	RAINBOW_WINDOW = false;

float	FULLSCREENSCALEX = 1.f;
float	FULLSCREENSCALEY = 1.f;

float	NOWFWND_W = FWND_W;// フルウィンドウの幅(float)
float	NOWFWND_H = FWND_H;// フルウィンドウの高さ(float)

D3DXVECTOR2	WINDOWRECT = { 0.f,0.f }; // ウィンドウ位置
D3DXVECTOR2	CLIENTRECT = { 0.f,0.f }; // ウィンドウの描画開始位置

bool	ISDEBUG = false;
bool	UIEDIOR_MOVEANY = false;
int		ENEMY_COUNT = 1;
int		ENEMY_COUNTMAX = 30;