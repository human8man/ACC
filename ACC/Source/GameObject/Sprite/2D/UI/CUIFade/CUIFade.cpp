#include "CUIFade.h"
#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// UIリスト.
	std::string FadeImage = "Black";
}


CUIFade::CUIFade()
	: m_pSprite2D	( nullptr )
	, m_Fading		( false )
	, m_FirstPeak	( false )
	, m_Peak		( false )
	, m_BeforePeak	( false )
	, m_End			( false )
	, m_EndFrame	( false )
	, m_Peaking		( false )

	, m_FadeAlpha	( 0.f )
	, m_AddAlpha	( 0.f )
	, m_InAddAlpha	( 0.f )
	, m_OutAddAlpha	( 0.f )
	
	, m_PeakCnt		( 0 )
{
}
CUIFade::~CUIFade()
{
}


//=================================================================================================
//		作成処理.
//=================================================================================================
void CUIFade::Create()
{
	// インスタンス生成
	m_pSprite2D = CSpriteManager::GetInstance()->Get2DSprite(FadeImage);
	m_pBlack = std::make_unique<CUIObject>();

	// 画像データの読み込み
	m_pBlack->AttachSprite(m_pSprite2D);
	m_pBlack->SetPos(m_pSprite2D->GetSpriteData().Pos);

	// アルファをセット.
	m_pBlack->SetAlpha(m_FadeAlpha);
}


//=================================================================================================
//		更新処理.
//=================================================================================================
void CUIFade::Update()
{
	m_EndFrame = false;

	// フェード中でない場合切る.
	if (!m_Fading) { return; }

	// アルファ減算値が0のままなら強制終了.
	if (m_Peaking && m_OutAddAlpha <= 0.f) {
		m_FadeAlpha = 0.f;
		m_Fading = false;
		m_Peaking = false;
		if (!m_End) { m_EndFrame = true; }
		m_End = true;
	}

	// 一度でも過去と現在のピーク情報が正の場合は不にする.
	if (m_BeforePeak && m_BeforePeak == m_Peak) { m_FirstPeak = false; }

	// ピーク時間中.
	if (m_Peaking) {
		if (m_PeakCnt > 0.f) {
			m_FadeAlpha = 1.f; // ピーク中はアルファ固定.
			m_PeakCnt -= CTime::GetDeltaTime();
		}
		else {
			m_FadeAlpha -= m_OutAddAlpha;

			// 安全マージン：負になったら0に補正
			if (m_FadeAlpha <= 0.f) {
				m_FadeAlpha = 0.f;
				m_Fading = false;
				m_Peaking = false;
				if (!m_End) { m_EndFrame = true; }
				m_End = true;
			}
		}
	}
	// ピークに対していない場合.
	else {
		m_FadeAlpha += m_InAddAlpha;

		// 上限補正.
		if (m_FadeAlpha >= 1.f) {
			m_FadeAlpha = 1.f;
		}

		// ピークに達したとき.
		if (1.f <= m_FadeAlpha && !m_Peak) {
			m_Peak = m_Peaking = m_FirstPeak = true;
		}
	}

	// フェード終了.
	if (m_FadeAlpha <= 0.f) {
		m_FadeAlpha = 0.f;
		m_Fading = false;
		m_End = true;
	}

	// 前フレームのピーク情報を保存.
	m_BeforePeak = m_Peak;

	if (m_pBlack) {
		m_pBlack->SetAlpha(m_FadeAlpha);
		m_pBlack->Update();
	}
}


//=================================================================================================
//		描画処理.
//=================================================================================================
void CUIFade::Draw() 
{
	m_pBlack->Draw();
}


//=================================================================================================
//		フェード開始処理.
//=================================================================================================
void CUIFade::DoFade(int in, int peak, int out)
{
	m_Peak = m_Peaking = m_End = m_FirstPeak = m_BeforePeak = false;
	m_Fading = true;
	m_FadeAlpha = 0.f;
	m_EndFrame = false;

	// 無効な入力を処理.
	if (in <= 0 && peak <= 0 && out <= 0) {
		m_Fading = false;
		m_End = true;
		m_EndFrame = true;
		return;
	}

	m_PeakCnt = peak * CTime::GetNormalDeltaTime();
	m_InAddAlpha = (in > 0) ? (1.f / in) : 0.f;
	m_OutAddAlpha = (out > 0) ? (1.f / out) : 0.f;
}