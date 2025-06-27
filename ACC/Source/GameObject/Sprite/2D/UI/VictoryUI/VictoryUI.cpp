#include "VictoryUI.h"

#include "Scenes/SceneManager/SceneManager.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectSound/SoundManager.h"
#include "Time/Time.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// UICSVのパス
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	std::string WinJson = "Data\\Texture\\UIData\\Victory.json";
}


VictoryUI::VictoryUI()
	: m_SpawnTimeMax	( Time::GetInstance()->GetDeltaTime() * 300.f )
	, m_SpawnTime		( m_SpawnTimeMax )
{
	// キャラクターCSVの情報保存用
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得
	m_StateList = FileManager::CSVLoad(UICSVPath);

	// 空でない場合は、外部で調整するべき変数の値を入れていく
	if (!m_StateList.empty())
	{
		m_SpawnTimeMax = StrToFloat(m_StateList["Win_SpawnTimeMax"]) * Time::GetInstance()->GetDeltaTime();
	}
}
VictoryUI::~VictoryUI()
{
	Release();
}


//=============================================================================
//		作成処理
//=============================================================================
void VictoryUI::Create()
{
	LoadFromJson(WinJson, m_pUIs);
}


//=============================================================================
//		読込処理
//=============================================================================
HRESULT VictoryUI::LoadData()
{
	return S_OK;
}


//=============================================================================
//		初期化処理
//=============================================================================
void VictoryUI::Init()
{
}


//=============================================================================
//		更新処理
//=============================================================================
void VictoryUI::Update()
{
	if (m_SpawnTime >= 0) { m_SpawnTime -= Time::GetInstance()->GetDeltaTime(); }

	//----------------------------------------------------------------------------
	//		それぞれのUIの更新
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) 
	{
		// 背景を透過させる
		if ( m_pUIs[i]->GetSpriteData().Name == "Black" ) { m_pUIs[i]->SetAlpha(0.6f); }

		// 出現時間が終了した場合
		if( m_SpawnTime < 0.f ) {
			// タイトル画面に遷移
			SceneManager::GetInstance()->LoadScene(SceneList::eTitle);
			SoundManager::GetInstance()->AllStop();
		}
	}
}


//=============================================================================
//		描画処理
//=============================================================================
void VictoryUI::Draw()
{	
	// UIそれぞれの描画処理
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		m_pUIs[i]->Draw();
	}
}


//=============================================================================
//		解放処理処理
//=============================================================================
void VictoryUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}