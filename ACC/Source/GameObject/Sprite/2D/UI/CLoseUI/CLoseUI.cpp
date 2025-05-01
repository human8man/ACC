#include "CLoseUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectSound/CSoundManager.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"


namespace {
	// UICSVのパス.
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	// LoseUIのパス.
	constexpr char LoseImagePath[] = "Data\\Texture\\Lose";
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
	int index = 0;

	// 指定したディレクトリ内を走査.
	for (const auto& entry : std::filesystem::directory_iterator(LoseImagePath)) {
		// 文末がjsonの場合やり直す.
		std::string Extension = entry.path().string();
		Extension.erase(0, entry.path().string().rfind("."));
		if (Extension == ".json") continue;

		// インスタンス生成.
		m_pSprite2Ds.push_back(new CSprite2D());
		m_pUIs.push_back(new CUIObject());

		// 画像データの読み込み.
		m_pSprite2Ds[index]->Init(entry.path().string());
		m_pUIs[index]->AttachSprite(m_pSprite2Ds[index]);
		m_pUIs[index]->SetPos(m_pSprite2Ds[index]->GetSpriteData().Pos);
		m_SpritePosList.push_back(m_pUIs[index]->GetPos());
		index++;
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
		if (i == LoseSprite::Black) { m_pUIs[i]->SetAlpha(0.6f); }

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