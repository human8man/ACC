#include "TitleUI.h"

#include "Scenes/SceneManager/SceneManager.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectSound/SoundManager.h"
#include "Time/Time.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "Sprite/2D/UI/NumberUI/NumberUI.h"


namespace {
	std::string TitleJson = "Data\\Texture\\UIData\\Title.json";

	// 非処理のUIリスト
	std::vector<std::string> ignoreList = {
		"Black",
		"Title"
	};
}


TitleUI::TitleUI()
	: m_pNumberUI	( nullptr )
	, m_NumPos		( ZEROVEC3 )
	, m_Size		( ZEROVEC3 )
	, m_Start		( false )
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
	m_pNumberUI = std::make_unique<NumberUI>();
	m_pNumberUI->Create();
	LoadFromJson(TitleJson, m_pUIs);

	for (auto it = m_pUIs.begin(); it != m_pUIs.end(); )
	{
		// UIEditor内で設定した数字の中央上座標を取得
		if ((*it)->GetSpriteData().Name == "Number") {
			m_NumPos = (*it)->GetPos();
			m_NumPos.x += (*it)->GetSpriteData().Disp.w/2;
			m_Size = D3DXVECTOR3((*it)->GetSpriteData().Disp.w, (*it)->GetSpriteData().Disp.h,0.f);
			delete* it;
			it = m_pUIs.erase(it);
		}
		else {
			++it;
		}
	}
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
	Mouse*	Mouse	= DirectInput::GetInstance()->CDMouse();
	Key*	Key		= DirectInput::GetInstance()->CDKeyboard();
	
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
			m_pUIs[i]->GetSpriteData().Name) != ignoreList.end()) {
			continue;
		}

		// 点と四角の当たり判定
		if (m_pUIs[i]->PointInSquare(MousePos, CLIENTRECT)
			&& !(m_pUIs[i]->GetSpriteData().Name == "Number"))
		{
			//	前回選択されていなかった場合SEを鳴らす
			if (m_pUIs[i]->GetPatternNo().x == 0) {
				SoundManager::GetInstance()->Play(SoundManager::SE_SelectMove);
			}
			m_pUIs[i]->SetPatternNo(1, m_pUIs[i]->GetPatternNo().y);
		}
		else {
			m_pUIs[i]->SetPatternNo(0, m_pUIs[i]->GetPatternNo().y);
		}

		// 特定の名前の場合
		if (m_pUIs[i]->GetSpriteData().Name == "StartButton")
		{
			// すでにカーソルで選択されている場合
			if (m_pUIs[i]->GetPatternNo().x) {
				if (Mouse->IsLAction()) { m_Start = true; }
			}
			// SPACEキーでゲーム開始
			if (Key->IsKeyAction(DIK_SPACE)) {
				// 選択状態にする
				m_pUIs[i]->SetPatternNo(1, 0);
				m_Start = true;
			}
		}


		if (m_pUIs[i]->GetSpriteData().Name == "LRButton"){
			// 同名のため中央座標を基準に判断する
			if (m_NumPos.x > m_pUIs[i]->GetPos().x) {
				if (m_pUIs[i]->GetPatternNo().x) {
					if (Mouse->IsLDown()) { ENEMY_COUNT--; }
				}
				if (Key->IsKeyAction(DIK_LEFT)
					|| Key->IsKeyDown(DIK_A)) {
					ENEMY_COUNT--;
					m_pUIs[i]->SetPatternNo(1, m_pUIs[i]->GetPatternNo().y);
				}
				
			}
			else {
				m_pUIs[i]->SetPatternNo(m_pUIs[i]->GetPatternNo().x, 1);
				if (m_pUIs[i]->GetPatternNo().x) {
					if (Mouse->IsLDown()) { ENEMY_COUNT++; }
				}
				if (Key->IsKeyAction(DIK_RIGHT)
					|| Key->IsKeyDown(DIK_D)) {
					ENEMY_COUNT++;
					m_pUIs[i]->SetPatternNo(1, m_pUIs[i]->GetPatternNo().y);
				}
			}
		}
	}


	// 桁数によって座標を変える
	D3DXVECTOR3 numposition = m_NumPos;
	if (ENEMY_COUNT / 10 > 0) {
		numposition = D3DXVECTOR3(m_NumPos.x - m_Size.x, m_NumPos.y, m_NumPos.z);
	}
	else {
		numposition = D3DXVECTOR3(m_NumPos.x - m_Size.x / 2, m_NumPos.y, m_NumPos.z);
	}
	ENEMY_COUNT = std::clamp(ENEMY_COUNT, 1, ENEMY_COUNTMAX);
	m_pNumberUI->SetNumber(numposition, ENEMY_COUNT, 1.f);
}


//=================================================================================================
//		描画処理
//=================================================================================================
void TitleUI::Draw()
{
	// UIそれぞれの描画処理
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
	m_pNumberUI->Draw();
}


//=================================================================================================
//		解放処理
//=================================================================================================
void TitleUI::Release()
{
	m_pNumberUI.reset();
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}
