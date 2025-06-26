#pragma once

#include "Sprite/2D/UI/UIObject.h"
#include "Time/CTime.h"


class FadeUI
{
public:
	FadeUI();
	 ~FadeUI();

	void Create();	// 作成処理
	void Update();	// 更新処理
	void Draw();	// 描画処理
	
	/****************************************************************
	// @brief フェード開始処理(1秒 = 60フレーム)
	// @param in 黒になるまでの時間
	// @param peak 黒が表示される時間
	// @param out 黒が消えるまでにかかる時間
	****************************************************************/
	void DoFade(int in,int peak,int out);


	bool GetFading()		const { return m_Fading; }		// フェード中かを知らせる
	bool GetFirstPeak()		const { return m_FirstPeak; }	// フェードの初回ピークを知らせる
	bool GetFadePeak()		const { return m_Peaking; }		// フェードのピーク中を知らせる
	bool GetFadeEnd()		const { return m_End; }			// フェードの終了を知らせる
	bool GetFadeEndFrame()	const { return m_EndFrame; }	// フェードの終了フレームを知らせる

private:
	std::unique_ptr<UIObject>	m_pBlack;	// UIクラス
	Sprite2D*	m_pSprite2D;			// Sprite2Dクラス

	bool	m_Fading;		// フェード中
	bool	m_FirstPeak;	// 初回ピーク
	bool	m_Peak;			// ピークログ
	bool	m_BeforePeak;	// 前フレームにピークだったか
	bool	m_End;			// 終了
	bool	m_EndFrame;		// 終了フレーム
	bool	m_Peaking;		// ピーク時間中

	float	m_FadeAlpha;	// アルファ
	float	m_AddAlpha;		// アルファに追加する値
	float	m_InAddAlpha;	// フェードイン時のアルファに追加する値
	float	m_OutAddAlpha;	// フェードアウト時のアルファに追加する値

	float	m_PeakCnt;	// ピーク時間のカウント
};