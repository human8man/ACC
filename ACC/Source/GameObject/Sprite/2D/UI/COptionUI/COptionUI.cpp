#include "COptionUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "Common/DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"


namespace {
	// オプションUIのパス.
	constexpr char OptionImagePath[] = "Data\\Texture\\Option";
}


//=================================================================================================
//		オプションクラス.
//=================================================================================================
COptionUI::COptionUI(HWND hWnd)
	: m_Light			()
	, m_mView			()
	, m_mProj			()
	, m_SpriteDataList	()
	, m_SpritePosList	()
	, m_pUIs			()
	, m_pSprite2Ds		()
{
	m_hWnd = hWnd;
	m_Light.vDirection = D3DXVECTOR3(1.f, 5.f, 0.f);
}

COptionUI::~COptionUI()
{
	Release();
}


//=================================================================================================
//		構築関数.
//=================================================================================================
void COptionUI::Create()
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
//		読み込み.
//=================================================================================================
HRESULT COptionUI::LoadData()
{
	return S_OK;
}


//=================================================================================================
//		初期化.
//=================================================================================================
void COptionUI::Init()
{

}


//=================================================================================================
//		更新.
//=================================================================================================
void COptionUI::Update()
{
	CMouse* Mouse = CInput::GetInstance()->CDMouse();

	// マウス位置を取得.
	POINT MousePos;
	GetCursorPos(&MousePos);

	// ウィンドウ全体の位置とサイズを取得（ウィンドウタブや枠を含む）.
	RECT WindowRect;
	GetWindowRect(m_hWnd, &WindowRect);

	// クライアント領域の位置とサイズを取得（ウィンドウ内の描画範囲）.
	RECT clientRect;
	GetClientRect(m_hWnd, &clientRect);

	// クライアント領域の左上と右下の座標を初期化.
	POINT topLeft = { clientRect.left, clientRect.top };
	POINT bottomRight = { clientRect.right, clientRect.bottom };

	// クライアント領域の座標をスクリーン座標系に変換.
	ClientToScreen(m_hWnd, &topLeft);
	ClientToScreen(m_hWnd, &bottomRight);

	// ウィンドウ全体の左上座標とクライアント領域の左上座標の差分を計算.
	int borderLeft = topLeft.x - WindowRect.left;
	int borderTop = topLeft.y - WindowRect.top;

	// フレーム幅を含んだウィンドウの位置を算出.
	D3DXVECTOR2 windowrect = D3DXVECTOR2(
		static_cast<float>(borderLeft + WindowRect.left),
		static_cast<float>(borderTop + WindowRect.top));

	//----------------------------------------------------------------------------
	//		それぞれのUIの更新.
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		// 背景の処理はいらないので早期に切る.
		if (i == OptionSprite::FullScreen) { continue; }

		// UIのサイズと座標を変換する.
		D3DXVECTOR2 SquarePos = D3DXVECTOR2(m_pUIs[i]->GetPos().x, m_pUIs[i]->GetPos().y);
		D3DXVECTOR2 SquareDisp = D3DXVECTOR2(m_pUIs[i]->GetSpriteData().Disp.w, m_pUIs[i]->GetSpriteData().Disp.h);


		// 点と四角の当たり判定.
		if (PointInSquare(MousePos, SquarePos + windowrect, SquareDisp))
		{
			//	前回選択されていなかった場合SEを鳴らす.
			if (m_pUIs[i]->GetPatternNo().x == 0) {
				CSoundManager::GetInstance()->PlaySE(CSoundManager::SE_SelectMove);
			}
			m_pUIs[i]->SetPatternNo(1, 0);
		}
		else {
			m_pUIs[i]->SetPatternNo(0, 0);
		}


		// スタートボタンにカーソルが重なっている時.
		if (i == OptionSprite::StartButton && m_pUIs[i]->GetPatternNo().x) {
			if (Mouse->IsLAction()) {
				// ゲームを開始する.
				CSceneManager::GetInstance()->LoadScene(SceneList::Game);
			}
		}
	}
}


//=================================================================================================
//		描画.
//=================================================================================================
void COptionUI::Draw()
{
	// UIそれぞれの描画処理.
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		m_pUIs[i]->Draw();
	}
}


//=================================================================================================
//		解放処理.
//=================================================================================================
void COptionUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}