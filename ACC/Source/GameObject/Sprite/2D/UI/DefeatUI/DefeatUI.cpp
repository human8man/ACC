#include "DefeatUI.h"

#include "Scenes/SceneManager/SceneManager.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectSound/SoundManager.h"
#include "Time/Time.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// UICSVのパス
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	std::string LoseJson = "Data\\Texture\\UIData\\Defeat.json";
}


DefeatUI::DefeatUI()
	: m_SpawnTimeMax	( Time::GetDeltaTime() * 180.f )
	, m_SpawnTime		( m_SpawnTimeMax )
{
	// キャラクターCSVの情報保存用
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得
	m_StateList = FileManager::CSVLoad(UICSVPath);

	// 空でない場合は、外部で調整するべき変数の値を入れていく
	if (!m_StateList.empty())
	{
		m_SpawnTimeMax = StrToFloat(m_StateList["Lose_SpawnTimeMax"]) * Time::GetDeltaTime();
	}
}
DefeatUI::~DefeatUI()
{
	Release();
}


//=================================================================================================
//		作成処理
//=================================================================================================
void DefeatUI::Create()
{
	LoadFromJson(LoseJson, m_pUIs);
}


//=================================================================================================
//		読込処理
//=================================================================================================
HRESULT DefeatUI::LoadData()
{
	return S_OK;
}


//=================================================================================================
//		初期化処理
//=================================================================================================
void DefeatUI::Init()
{
}


//=================================================================================================
//		更新処理
//=================================================================================================
void DefeatUI::Update()
{
	if (m_SpawnTime >= 0) { m_SpawnTime -= Time::GetDeltaTime(); }

	//----------------------------------------------------------------------------
	//		それぞれのUIの更新
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) 
	{
		// 背景を透過させる
		if ( m_pUIs[i]->GetSpriteData().Name == "Black") { m_pUIs[i]->SetAlpha(0.6f); }

		// 出現時間が終了した場合
		if (m_SpawnTime < 0.f) {
			// タイトルに遷移
			SceneManager::GetInstance()->LoadScene(SceneList::eTitle);
			SoundManager::GetInstance()->AllStop();
		}
	}
}


//=================================================================================================
//		描画処理
//=================================================================================================
void DefeatUI::Draw()
{	
	// UIそれぞれの描画処理
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
}


//=================================================================================================
//		解放処理
//=================================================================================================
void DefeatUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}