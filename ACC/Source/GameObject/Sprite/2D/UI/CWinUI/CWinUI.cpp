#include "CWinUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectSound/CSoundManager.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// UICSVのパス.
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	// UIリスト.
	std::vector<std::string> ImageList = {
		"Black",
		"Victory"
	};
}


//=============================================================================
//		WinUIクラス.
//=============================================================================
CWinUI::CWinUI()
	: m_SpawnTimeMax	( CTime::GetInstance()->GetDeltaTime() * 300.f )
	, m_SpawnTime		( m_SpawnTimeMax )
{
	// キャラクターCSVの情報保存用.
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得.
	m_StateList = FileManager::CSVLoad(UICSVPath);

	// 空でない場合は、外部で調整するべき変数の値を入れていく.
	if (!m_StateList.empty())
	{
		m_SpawnTimeMax = StrToFloat(m_StateList["Win_SpawnTimeMax"]) * CTime::GetInstance()->GetDeltaTime();
	}
}
CWinUI::~CWinUI()
{
	Release();
}


//=============================================================================
//		作成処理.
//=============================================================================
void CWinUI::Create()
{
	LoadSpriteList(ImageList, m_pUIs, m_pSprite2Ds);
}


//=============================================================================
//		読込処理.
//=============================================================================
HRESULT CWinUI::LoadData()
{
	return S_OK;
}


//=============================================================================
//		初期化処理.
//=============================================================================
void CWinUI::Init()
{
}


//=============================================================================
//		更新処理.
//=============================================================================
void CWinUI::Update()
{
	if (m_SpawnTime >= 0) { m_SpawnTime -= CTime::GetInstance()->GetDeltaTime(); }

	//----------------------------------------------------------------------------
	//		それぞれのUIの更新.
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) 
	{
		// 背景を透過させる.
		if ( m_pUIs[i]->GetSpriteData().Name == "Black" ) { m_pUIs[i]->SetAlpha(0.6f); }

		// 出現時間が終了した場合.
		if( m_SpawnTime < 0.f ) {
			// タイトル画面に遷移.
			CSceneManager::GetInstance()->LoadScene(SceneList::Title);
			CSoundManager::GetInstance()->AllStop();
		}
	}
}


//=============================================================================
//		描画処理.
//=============================================================================
void CWinUI::Draw()
{	
	// UIそれぞれの描画処理.
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		m_pUIs[i]->Draw();
	}
}


//=============================================================================
//		解放処理処理.
//=============================================================================
void CWinUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}