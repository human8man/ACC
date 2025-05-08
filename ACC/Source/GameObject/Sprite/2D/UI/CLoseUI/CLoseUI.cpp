#include "CLoseUI.h"

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
		"Lose"
	};
}


//=================================================================================================
//		LoseUIクラス.
//=================================================================================================
CLoseUI::CLoseUI()
	: m_SpriteDataList	()
	, m_SpritePosList	()
	, m_pUIs			()
	, m_pSprite2Ds		()

	, m_SpawnTimeMax	( CTime::GetInstance()->GetDeltaTime() * 300.f )
	, m_SpawnTime		( m_SpawnTimeMax )
{
	// キャラクターCSVの情報保存用.
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得.
	m_StateList = FileManager::CSVLoad(UICSVPath);

	// 空でない場合は、外部で調整するべき変数の値を入れていく.
	if (!m_StateList.empty())
	{
		m_SpawnTimeMax = StrToFloat(m_StateList["Lose_SpawnTimeMax"]) * CTime::GetInstance()->GetDeltaTime();
	}
}
CLoseUI::~CLoseUI()
{
	Release();
}


//=================================================================================================
//		作成処理.
//=================================================================================================
void CLoseUI::Create()
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
//		読込処理.
//=================================================================================================
HRESULT CLoseUI::LoadData()
{
	return S_OK;
}


//=================================================================================================
//		初期化処理.
//=================================================================================================
void CLoseUI::Init()
{
}


//=================================================================================================
//		更新処理.
//=================================================================================================
void CLoseUI::Update()
{
	if (m_SpawnTime >= 0) { m_SpawnTime -= CTime::GetInstance()->GetDeltaTime(); }

	//----------------------------------------------------------------------------
	//		それぞれのUIの更新.
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) 
	{
		// 背景を透過させる.
		if ( m_pUIs[i]->GetSpriteData().Name == "Black") { m_pUIs[i]->SetAlpha(0.6f); }

		// 出現時間が終了した場合.
		if (m_SpawnTime < 0.f) {
			// タイトルに遷移.
			CSceneManager::GetInstance()->LoadScene(SceneList::Title);
			CSoundManager::GetInstance()->AllStop();
		}
	}
}


//=================================================================================================
//		描画処理.
//=================================================================================================
void CLoseUI::Draw()
{	
	// UIそれぞれの描画処理.
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
}


//=================================================================================================
//		解放処理.
//=================================================================================================
void CLoseUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}