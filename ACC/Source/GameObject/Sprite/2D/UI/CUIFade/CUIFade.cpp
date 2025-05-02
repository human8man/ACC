#include "CUIFade.h"
#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// json 読込につかうパスの設定.
	constexpr char FadeImagePath[] = "Data\\Texture\\other";

	// UIリスト.
	std::string FadeImage = "Black";

}


//=============================================================================
//		フェードクラス.
//=============================================================================
CUIFade::CUIFade()
	: m_FadeStart	( false )
	, m_Fading		( false )
	, m_FadePeak	( false )
	, m_FadeEnd		( false )
	, m_Peaking		( false )

	, m_FadeAlpha	( 0.f )
	, m_AddAlpha	( 0.f )
	, m_AAVMAXVAL	( 0.02f )
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
	m_pSprite2Ds.push_back(new CSprite2D);
	m_pUIs.push_back(new CUIObject());
	m_pSprite2Ds.back() = CSpriteManager::GetInstance()->GetSprite(FadeImage);

	// 画像データの読み込み
	m_pUIs.back()->AttachSprite(m_pSprite2Ds.back());
	m_pUIs.back()->SetPos(m_pSprite2Ds.back()->GetSpriteData().Pos);
	m_SpritePosList.push_back(m_pUIs.back()->GetPos());
}


//=================================================================================================
//		更新処理.
//=================================================================================================
void CUIFade::Update()
{
	// フェード開始.
	if ( m_FadeStart ) {
		m_FadeStart = false;
		m_Fading = true;
		m_FadePeak = false;
		m_FadeEnd = false;
		m_Peaking = false;

		m_AddAlpha = m_AAVMAXVAL;
	}

	// フェード中.
	if ( m_Fading ) {
		// ピーク時間中.
		if ( m_Peaking ) {
			m_FadeAlpha = 1.f;	// アルファを1で固定.
			m_PeakCnt--;

			if ( m_PeakCnt <= 0 ) {
				m_AddAlpha = -m_AAVMAXVAL;
				m_Peaking = false;
			}
		}
		else {
			m_FadeAlpha += m_AddAlpha;
		}

		// フェードの上限についていない場合.
		if ( !m_FadePeak ) {
			// アルファの最大値を超えた場合.
			if ( 1.f < m_FadeAlpha )
			{
				m_FadePeak = true;
				m_Peaking = true;
			}
		}
		else {
			m_FadePeak = false;
		}

		// フェード終了.
		if ( m_FadeAlpha <= 0 ) {
			m_FadeAlpha = 0.f;
			m_Fading = false;
			m_FadeEnd = true;
		}
	}

	// スプライトの方のアルファにセット.
	m_pUIs[0]->SetAlpha( m_FadeAlpha );

	// 更新.
	m_pUIs[0]->Update();
}


//=================================================================================================
//		描画処理.
//=================================================================================================
void CUIFade::Draw()
{
	m_pUIs[0]->Draw();
}