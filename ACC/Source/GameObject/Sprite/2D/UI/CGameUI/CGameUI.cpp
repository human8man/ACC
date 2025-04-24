#include "CGameUI.h"

#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"


namespace {
	// UICSVのパス.
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	// 画像のパス.
	constexpr char GameImagePath[] = "Data\\Texture\\Game";
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
	int index = 0;
	
	// 指定したディレクトリ内を走査.
	for (const auto& entry : std::filesystem::directory_iterator(GameImagePath)) {
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
		// 敵フレームは別で処理.
		if (i == GameSprite::EnemyFrame) { continue; }

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

		// HPUIの描画設定.
		if (i == GameSprite::HP) {
			if (m_HP < 0) { m_HP = 0; }
			m_pUIs[i]->SetPatternNo(m_HP, 0);
		}

		// LOWHPUIの描画設定.
		if (i == GameSprite::LowHP) {
			// HPが半分以上の場合は描画しない.
			if (m_HP > m_HPMax / 2) { continue; }
		}

		// リロードUIの描画設定.
		if (i == GameSprite::Reload) {
			// リロードしていない場合は描画しない.
			if (m_ReloadTime <= 0.f) { continue; }
		}

		// チート系のUI描画設定.
		if (i == GameSprite::AutoAim) {m_pUIs[i]->SetPatternNo(m_AutoAim, 0);}
		if (i == GameSprite::Homing)  {m_pUIs[i]->SetPatternNo(m_Homing, 0);}
		if (i == GameSprite::WallHack)  {m_pUIs[i]->SetPatternNo(m_WallHack, 0);}

		m_pUIs[i]->Draw();
	}
}


//=================================================================================================
//		解放処理.
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