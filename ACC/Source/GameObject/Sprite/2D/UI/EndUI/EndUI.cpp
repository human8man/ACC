#include "EndUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectSound/CSoundManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// EndUIのパス
	std::string EndJson = "Data\\Texture\\UIData\\End.json";

	// 非処理のUIリスト
	std::vector<std::string> ignoreList = {
		"EndBlack",
		"Frame",
	};
}


EndUI::EndUI(HWND hWnd)
	: m_hWnd			()
	, m_WindowRect		( ZEROVEC2 )
	, m_EndDeleteFlag	( false )
{
	m_hWnd = hWnd;
}

EndUI::~EndUI()
{
	Release();
}


//=============================================================================
//		作成処理
//=============================================================================
void EndUI::Create()
{
	LoadFromJson(EndJson, m_pUIs);
}

//=============================================================================
//		初期化処理
//=============================================================================
void EndUI::Init()
{
	m_WindowRect = ZEROVEC2;
	m_EndDeleteFlag = false;
}


//=============================================================================
//		更新処理
//=============================================================================
void EndUI::Update()
{
	// DInputの呼び出し
	CMouse* Mouse = CInput::GetInstance()->CDMouse();
	CKey* Key = CInput::GetInstance()->CDKeyboard();
	
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

		std::string spritename = m_pUIs[i]->GetSpriteData().Name;

		// UIのサイズと座標を変換する
		D3DXVECTOR2 SquarePos = D3DXVECTOR2(m_pUIs[i]->GetPos().x, m_pUIs[i]->GetPos().y);
		D3DXVECTOR2 SquareDisp = D3DXVECTOR2(m_pUIs[i]->GetSpriteData().Disp.w, m_pUIs[i]->GetSpriteData().Disp.h);

		// 点と四角の当たり判定
		if (m_pUIs[i]->PointInSquare(MousePos,CLIENTRECT))
		{
			//	前回選択されていなかった場合SEを鳴らす
			if (m_pUIs[i]->GetPatternNo().x == 0) {
				CSoundManager::GetInstance()->Play(CSoundManager::SE_SelectMove);
			}
			m_pUIs[i]->SetPatternNo(1, 0);
		}
		else {
			m_pUIs[i]->SetPatternNo(0, 0);
		}

		bool yesflag = false, noflag = false;

		// Yesにカーソルが重なっている時
		if (spritename == "SelectYes" && m_pUIs[i]->GetPatternNo().x) {
			if (Mouse->IsLAction()) { yesflag = true; }
		}
		if (Key->IsKeyAction(DIK_Y)) { yesflag = true; }

		// Noにカーソルが重なっている時
		if (spritename == "SelectNo" && m_pUIs[i]->GetPatternNo().x) {
			if (Mouse->IsLAction()) { noflag = true; }
		}
		if (Key->IsKeyAction(DIK_N)) { noflag = true; }
		
		// フラグの結果に合わせて処理をする
		if (yesflag) { DestroyWindow(m_hWnd); }
		if (noflag) { m_EndDeleteFlag = true; }
	}
}


//=============================================================================
//		描画処理
//=============================================================================
void EndUI::Draw()
{
	// UIそれぞれの描画処理
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
}


//=============================================================================
//		解放処理
//=============================================================================
void EndUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}