#include "CGameUI.h"

#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// UICSVのパス.
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	// UIリスト.
	std::vector<std::string> ImageList = {
		"Bullets",
		"Crosshair",
		"HP",
		"LowHP",
		"Reload",
		"AutoAim",
		"Homing",
		"WallHack"
	};
}


//=================================================================================================
//		ゲームUIクラス.
//=================================================================================================
CGameUI::CGameUI()
	: m_HP				( 0 )
	, m_HPMax			( 0 )
	, m_Ammo			( 0 )
	, m_HitKind			( 0 )
	, m_ReloadTime		( 0.f )
	, m_ViewHitTime		( 0.f )
	, m_ViewHitTimeMax	( CTime::GetInstance()->GetDeltaTime() * 60.f )
	, m_AutoAim			( false )
	, m_Homing			( false )
	, m_WallHack		( false )
{
	// キャラクターCSVの情報保存用.
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得.
	m_StateList = FileManager::CSVLoad(UICSVPath);

	// 空でない場合は、外部で調整するべき変数の値を入れていく.
	if (!m_StateList.empty())
	{
		m_ViewHitTimeMax = StrToFloat(m_StateList["Game_HitViewTime"]) * CTime::GetInstance()->GetDeltaTime();
	}
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
	std::unordered_map<std::string, int> nameCounts; // 名前ごとの出現回数を記録.

	for (size_t i = 0; i < ImageList.size(); ++i)
	{
		// 名前被りがある場合の処理.
		std::string baseName = ImageList[i];
		std::string numberedName;

		if (nameCounts.count(baseName) == 0) {
			numberedName = baseName;	// 1個目はそのまま.
			nameCounts[baseName] = 1;	// 次からは1スタート.
		}
		else {
			numberedName = baseName + "_" + std::to_string(nameCounts[baseName]);
			nameCounts[baseName]++;
		}

		// インスタンス生成.
		m_pSprite2Ds.push_back(CSpriteManager::GetInstance()->GetSprite(baseName));
		m_pUIs.push_back(new CUIObject());
		CSprite2D* pSprite = CSpriteManager::GetInstance()->GetSprite(ImageList[i]);

		// 画像データの読み込み.
		m_pUIs.back()->AttachSprite(pSprite);
		m_pUIs.back()->SetPos(m_pSprite2Ds.back()->GetSpriteData().Pos);
		m_SpritePosList.push_back(m_pUIs.back()->GetPos());

		// 変更後の名前につけなおす.
		m_pUIs.back()->SetSpriteName(numberedName);
	}
}


//=================================================================================================
//		更新.
//=================================================================================================
void CGameUI::Update(std::unique_ptr<CPlayer>& chara)
{
	// カウントを回す.
	if (m_ViewHitTime >= 0.f) { m_ViewHitTime -= CTime::GetInstance()->GetDeltaTime(); }

	// プレイヤーの攻撃が命中していた場合.
	if (chara->GetHit()) {
		// 命中の種類を設定.
		m_HitKind = chara->GetHitKind();
		m_ViewHitTime = m_ViewHitTimeMax;
	}

	// ゲーム画面UIに必要な情報の設定.
	m_Homing		= chara->GetHoming();
	m_WallHack		= chara->GetWallHack();
	m_AutoAim		= chara->GetAutoAim();
	m_ReloadTime	= chara->GetReloadTime();
	m_HP			= chara->GetCharaInfo().HP;
	m_Ammo			= chara->GetCharaInfo().Ammo;
	m_HPMax			= chara->GetCharaInfo().MaxHP;
}


//=================================================================================================
//		描画.
//=================================================================================================
void CGameUI::Draw()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i ) {
		std::string spritename = m_pUIs[i]->GetSpriteData().Name;
		
		// 弾UIの描画設定.
		if( spritename == "Bullets" ) {
			// リロードしていた場合は描画しない.
			if (m_ReloadTime >= 0.f) { continue; }
			m_pUIs[i]->SetPatternNo(m_Ammo, 0);
		}

		// クロスヘアの描画設定.
		if ( spritename == "Crosshair" ) {
			if (m_ViewHitTime > 0.f) {
				m_pUIs[i]->SetPatternNo(m_HitKind, 0);
			}
			else {
				m_pUIs[i]->SetPatternNo(0, 0);
			}
		}

		// HPUIの描画設定.
		if ( spritename == "HP" ) {
			if (m_HP < 0) { m_HP = 0; }
			m_pUIs[i]->SetPatternNo(m_HP, 0);
		}

		// LOWHPUIの描画設定.
		if ( spritename == "LowHP" ) {
			// HPが半分以上の場合は描画しない.
			if (m_HP > m_HPMax / 2) { continue; }
		}

		// リロードUIの描画設定.
		if ( spritename == "Reload" ) {
			// リロードしていない場合は描画しない.
			if (m_ReloadTime <= 0.f) { continue; }
		}

		// チート系のUI描画設定.
		if (spritename == "AutoAim")	{ m_pUIs[i]->SetPatternNo(m_AutoAim, 0); }
		if (spritename == "Homing")		{ m_pUIs[i]->SetPatternNo(m_Homing, 0); }
		if (spritename == "WallHack")	{ m_pUIs[i]->SetPatternNo(m_WallHack, 0); }

		m_pUIs[i]->Draw();
	}
}


//=================================================================================================
//		解放処理.
//=================================================================================================
void CGameUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}