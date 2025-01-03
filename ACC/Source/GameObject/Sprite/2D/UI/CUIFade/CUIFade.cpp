#include "CUIFade.h"
#include "DirectX/CDirectX11.h"	// Depthで使用.
#include "DirectSound/CSoundManager.h"
namespace {
	constexpr char TitleImagePath[] = "Data\\Texture\\other";
}

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

void CUIFade::Create()
{
	int index = 0;

	try
	{
		// 指定したディレクトリ内を走査.
		for (const auto& entry : std::filesystem::directory_iterator(TitleImagePath)) {
			// jsonの場合やり直す.
			std::string Extension = entry.path().string();
			Extension.erase(0, entry.path().string().rfind("."));
			if (Extension == ".json") continue;

			m_pSprite2Ds.push_back(new CSprite2D());
			m_pUIs.push_back(new CUIObject());

			m_pSprite2Ds[index]->Init(entry.path().string());
			m_pUIs[index]->AttachSprite(*m_pSprite2Ds[index]);
			index++;
		}
	}
	catch (const std::exception& e) { return; }

	//// スプライト情報を設定.
	//BlackFade = { WND_W,WND_H,WND_W,WND_H,WND_W,WND_H,	ZEROVEC3};

	//// ファイルパスとスプライトデータをまとめて準備.
	//SpriteDataList = {
	//	{ "Data\\Texture\\other\\Black.png", BlackFade},
	//};

	//// 各2DSpriteオブジェクトの初期設定をする.
	//for (size_t i = 0; i < SpriteDataList.size(); ++i) {
	//	m_pSprite2Ds.push_back(new CSprite2D());
	//	m_pSprite2Ds[i]->Init(SpriteDataList[i].first.c_str());
	//}

	//// 各UIオブジェクトの初期設定をする.
	//for (size_t i = 0; i < SpriteDataList.size(); ++i) {
	//	m_pUIs.push_back(new CUIObject());
	//	m_pUIs[i]->AttachSprite(*m_pSprite2Ds[i]);
	//	m_pUIs[i]->SetPosition(SpriteDataList[i].second.Pos);
	//}
}

void CUIFade::Update()
{
	// いつか追加されるかもしれない画像用にfor.
	for ( size_t i = 0; i < m_pUIs.size(); ++i ) 
	{

		// 黒画像の場合.
		if ( i == SpriteNum::Black )
		{

			// フェード開始.
			if ( m_FadeStart )
			{
				m_FadeStart = false;
				m_Fading	= true;

				m_FadePeak	= false;
				m_FadeEnd	= false;

				m_Peaking = false;

				m_AddAlpha	= m_AAVMAXVAL;
			}


			// フェード中.
			if( m_Fading )
			{
				CSoundManager::Stop(CSoundManager::SE_Move);


				// ピーク時間中.
				if ( m_Peaking )
				{
					m_FadeAlpha = 1.f;	// アルファを1で固定.
					m_PeakCnt--;

					if (m_PeakCnt <= 0)
					{
						m_AddAlpha = -m_AAVMAXVAL;
						m_Peaking  = false;
					}
				}
				else 
				{
					m_FadeAlpha += m_AddAlpha;
				}

				// フェードの上限についていない場合.
				if ( !m_FadePeak )
				{
					// アルファの最大値を超えた場合.
					if ( 1.f < m_FadeAlpha )
					{
						m_FadePeak = true;
						m_Peaking  = true;
					}
				}
				else 
				{
					m_FadePeak = false;
				}

				
				// フェード終了.
				if ( m_FadeAlpha <= 0 )
				{
					m_FadeAlpha = 0.f;
					m_Fading	= false;
					m_FadeEnd	= true;
				}
			}
		}

		// スプライトの方のアルファにセット.
		m_pUIs[i]->SetAlpha( m_FadeAlpha );

		// 更新.
		m_pUIs[i]->Update();
	}
}

void CUIFade::Draw()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i )
	{
		m_pUIs[i]->Draw();
	}
}

void CUIFade::Release()
{
	for (auto rit = m_pUIs.rbegin();
		rit != m_pUIs.rend(); ++rit)
	{
		delete* rit;
		*rit = nullptr;
	}
	m_pUIs.clear();
	m_pUIs.shrink_to_fit();

	for (auto rit = m_pSprite2Ds.rbegin();
		rit != m_pSprite2Ds.rend(); ++rit)
	{
		delete *rit;
		*rit = nullptr;
	}
	m_pSprite2Ds.clear();
	m_pSprite2Ds.shrink_to_fit();

	SpriteDataList.clear();
	SpriteDataList.shrink_to_fit();
}
