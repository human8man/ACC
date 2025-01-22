#include "CGameUI.h"
#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"
#include "Common/Time/CTime.h"
#include "Common/Easing/Easing.h"

namespace {
	constexpr char FadeImagePath[] = "Data\\Texture\\Game";
}

//=================================================================================================
//		ゲームUIクラス.
//=================================================================================================
CGameUI::CGameUI()
	: m_Ammo			( 0 )
	, m_ReloadTime		( 0.f )
	, m_ViewHitTime		( 0.f )
	, m_ViewHitTimeMax	( CTime::GetInstance()->GetDeltaTime() * 60.f )
{
}

CGameUI::~CGameUI()
{
	Release();
}

//=================================================================================================
//		構築関数.
//=================================================================================================
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


//=================================================================================================
//		更新.
//=================================================================================================
void CGameUI::Update()
{
	// カウントを回す.
	if (m_ViewHitTime >= 0.f) { m_ViewHitTime -= CTime::GetInstance()->GetDeltaTime(); }

}


//=================================================================================================
//		描画.
//=================================================================================================
void CGameUI::Draw()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i ) {

		// 弾UIの描画設定.
		if(i == GameSprite::Bullets) {
			// リロードしていた場合は描画しない.
			if (m_ReloadTime >= 0.f) { continue; }
			m_pUIs[i]->SetPatternNo(m_Ammo, 0);
		}

		// クロスヘアの描画設定.
		if (i == GameSprite::Crosshair) {
			if (m_ViewHitTime > 0.f) {
				m_pUIs[i]->SetPatternNo(m_HitKind, 0);
			}
			else {
				m_pUIs[i]->SetPatternNo(0, 0);
			}
		}

		// リロードUIの描画設定.
		if (i == GameSprite::Reload) {
			// リロードしていない場合は描画しない.
			if (m_ReloadTime <= 0.f) { continue; }
		}


		m_pUIs[i]->Draw();
	}
}


//=================================================================================================
//		開放.
//=================================================================================================
void CGameUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pUIs[i]);
	}
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pSprite2Ds[i]);
	}
}


//=================================================================================================
//		Hit情報の設定.
//=================================================================================================
void CGameUI::SetHit(int hit)
{
	m_HitKind = hit; 
	m_ViewHitTime = m_ViewHitTimeMax;
}
