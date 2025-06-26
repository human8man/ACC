#include "TitleUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectSound/CSoundManager.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	std::string TitleJson = "Data\\Texture\\UIData\\Title.json";

	// 非処理のUIリスト
	std::vector<std::string> ignoreList = {
		"Black",
		"Title"
	};
}


TitleUI::TitleUI()
	: m_Start			( false )
{
}
TitleUI::~TitleUI()
{
	Release();
}


//=================================================================================================
//		作成処理
//=================================================================================================
void TitleUI::Create()
{
	LoadFromJson(TitleJson, m_pUIs);
}


//=================================================================================================
//		読込処理
//=================================================================================================
HRESULT TitleUI::LoadData()
{
	return S_OK;
}


//=================================================================================================
//		初期化処理
//=================================================================================================
void TitleUI::Init()
{
}


//=================================================================================================
//		更新処理
//=================================================================================================
void TitleUI::Update()
{
	CMouse*	Mouse	= CInput::GetInstance()->CDMouse();
	CKey*	Key		= CInput::GetInstance()->CDKeyboard();
	
	// マウス位置を取得
	POINT MousePos;
	GetCursorPos(&MousePos);
	
	//----------------------------------------------------------------------------
	//		それぞれのUIの更新
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		// 処理のいらないUIを早期に切る
		if (std::find(
			ignoreList.begin(),
			ignoreList.end(), 
			m_pUIs[i]->GetSpriteData().Name) != ignoreList.end()) { continue; }

		// 点と四角の当たり判定
		if ( m_pUIs[i]->PointInSquare( MousePos, CLIENTRECT ) )
		{
			//	前回選択されていなかった場合SEを鳴らす
			if ( m_pUIs[i]->GetPatternNo().x == 0 ) {
				CSoundManager::GetInstance()->Play(CSoundManager::SE_SelectMove);
			}
			m_pUIs[i]->SetPatternNo(1, 0);
		}
		else {
			m_pUIs[i]->SetPatternNo(0, 0);
		}

		// 特定の名前の場合
		if ( m_pUIs[i]->GetSpriteData().Name == "StartButton" )
		{
			// すでにカーソルで選択されている場合
			if ( m_pUIs[i]->GetPatternNo().x ) {
				if (Mouse->IsLAction()) { m_Start = true; }
			}
			else {
				// SPACEキーでゲーム開始
				if (Key->IsKeyAction(DIK_SPACE)) {
					// 選択状態にする
					m_pUIs[i]->SetPatternNo(1, 0);
					m_Start = true;
				}
			}
		}
	}
}


//=================================================================================================
//		描画処理
//=================================================================================================
void TitleUI::Draw()
{
	// UIそれぞれの描画処理
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
}


//=================================================================================================
//		解放処理
//=================================================================================================
void TitleUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}
