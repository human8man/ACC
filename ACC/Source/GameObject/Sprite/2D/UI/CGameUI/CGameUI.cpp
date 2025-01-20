#include "CGameUI.h"
#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"

namespace {
	constexpr char FadeImagePath[] = "Data\\Texture\\Game";
}

//======================================================================================================================
//		ゲームUIクラス.
//======================================================================================================================
CGameUI::CGameUI()
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

CGameUI::~CGameUI()
{
	Release();
}

//======================================================================================================================
//		構築関数.
//======================================================================================================================
void CGameUI::Create()
{
	int index = 0;

	// 指定したディレクトリ内を走査.
	for (const auto& entry : std::filesystem::directory_iterator(FadeImagePath)) {
		// 文末がjsonの場合やり直す.
		std::string Extension = entry.path().string();
		Extension.erase(0, entry.path().string().rfind("."));
		if (Extension == ".json") continue;

		// インスタンス生成.
		m_pSprite2Ds.push_back(new CSprite2D());
		m_pUIs.push_back(new CUIObject());

		// 画像データの読み込み.
		m_pSprite2Ds[index]->Init(entry.path().string());
		m_pUIs[index]->AttachSprite(*m_pSprite2Ds[index]);
		m_pUIs[index]->SetPos(m_pSprite2Ds[index]->GetSpriteData().Pos);
		m_SpritePosList.push_back(m_pUIs[index]->GetPos());
		index++;
	}
}


//======================================================================================================================
//		更新.
//======================================================================================================================
void CGameUI::Update()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i ) 
	{
	}
}


//======================================================================================================================
//		描画.
//======================================================================================================================
void CGameUI::Draw()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i )
	{
		m_pUIs[i]->Draw();
	}
}


//======================================================================================================================
//		開放.
//======================================================================================================================
void CGameUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pUIs[i]);
	}
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pSprite2Ds[i]);
	}
}
