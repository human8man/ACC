#include "CLoseUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "Common/DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"

#include "Common/Time/CTime.h"
#include "Common/Easing/Easing.h"


namespace {
	// LoseUIのパス.
	constexpr char OptionImagePath[] = "Data\\Texture\\Lose";
}


//=================================================================================================
//		LoseUIクラス.
//=================================================================================================
CLoseUI::CLoseUI()
	: m_Light			()
	, m_mView			()
	, m_mProj			()
	, m_SpriteDataList	()
	, m_SpritePosList	()
	, m_pUIs			()
	, m_pSprite2Ds		()

	, m_SpawnTimeMax	( CTime::GetInstance()->GetDeltaTime() * 300.f )
	, m_SpawnTime		( m_SpawnTimeMax )
{
	m_hWnd = nullptr;
	m_Light.vDirection = D3DXVECTOR3(1.f, 5.f, 0.f);
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
	for (const auto& entry : std::filesystem::directory_iterator(OptionImagePath)) {
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
	if (m_SpawnTime <= 0) { m_SpawnTime -= CTime::GetInstance()->GetDeltaTime(); }

	//----------------------------------------------------------------------------
	//		それぞれのUIの更新.
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) 
	{
		// 背景を透過させる.
		if (i == LoseSprite::Black) { m_pUIs[i]->SetAlpha(0.4f); }

		if (m_SpawnTime < 0.f) {
			// ゲームを開始する.
			CSceneManager::GetInstance()->LoadScene(SceneList::Title);
			CSoundManager::GetInstance()->Stop(CSoundManager::enList::BGM_Game);
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