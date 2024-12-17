#pragma once

#include "GameObject/Sprite/2D/UI/CUIObject.h"

class CUIFade
{
public:
	// 情報まとめる用.
	struct FileData {
		std::string filepath;			// ファイルのパス.
		CSprite2D::SPRITE_STATE sprite;	// スプライト情報.
	};
	// 画像種類ごとに番号を振り当てる.
	enum SpriteNum {
		Black = 0,
	};

public:
	CUIFade();
	 ~CUIFade();

	void Create();
	void Update();
	void Draw();
	void Release();

	// フェードを開始する(黒が表示されている時間).
	void DoFade(int peaktime) { m_FadeStart = true; m_PeakCnt = peaktime; }

	// 値を返す関数.
	bool GetFading()	const { return m_Fading;   } // フェード中かを知らせる.
	bool GetFadePeak()	const { return m_FadePeak; } // フェードのピークを知らせる.
	bool GetFadeEnd()	const { return m_FadeEnd;  } // フェードの終了を知らせる.
private:
	std::vector<std::pair<std::wstring, CSprite2D::SPRITE_STATE>> SpriteDataList;	// スプライト情報をまとめる.
	std::vector<CUIObject*> m_pUIs;			// UIクラス.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2Dクラス.

	CSprite2D::SPRITE_STATE BlackFade;		// 黒フェード.

	bool	m_FadeStart;	// フェード開始.
	bool	m_Fading;		// フェード中.
	bool	m_FadePeak;		// フェード上限.
	bool	m_FadeEnd;		// フェード終了.
	bool	m_Peaking;		// ピーク時間中.

	float	m_FadeAlpha;	// フェードのアルファ.
	float	m_AddAlpha;		// フェードのアルファに追加する値.
	float	m_AAVMAXVAL;	// フェードのアルファに追加する値の定数化.

	int		m_PeakCnt;		// ピークに達している時間のカウント.
};