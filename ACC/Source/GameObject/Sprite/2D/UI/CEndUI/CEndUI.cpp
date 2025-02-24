#include "CEndUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "GameObject/Sprite/2D/UI/CUIObject.h"
#include "Common/DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"


namespace {
	// EndUIのパス.
	constexpr char OptionImagePath[] = "Data\\Texture\\End";
}


//=============================================================================
//		EndUIクラス.
//=============================================================================
CEndUI::CEndUI()
	: m_hWnd			()
	, m_SpriteDataList	()
	, m_SpritePosList	()
	, m_pUIs			()
	, m_pSprite2Ds		()
	, m_WindowRect		( ZEROVEC2 )
	, m_EndDeleteFlag	( false )
{
}

CEndUI::~CEndUI()
{
	Release();
}


//=============================================================================
//		作成処理.
//=============================================================================
void CEndUI::Create(HWND hWnd)
{
	m_hWnd = hWnd;
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

//=============================================================================
//		初期化処理.
//=============================================================================
void CEndUI::Init()
{
	m_WindowRect = ZEROVEC2;
	m_EndDeleteFlag = false;
}


//=============================================================================
//		更新処理.
//=============================================================================
void CEndUI::Update()
{
	// DInputの呼び出し.
	CMouse* Mouse = CInput::GetInstance()->CDMouse();
	CKey* Key = CInput::GetInstance()->CDKeyboard();
	
	// マウス位置を取得.
	POINT MousePos;
	GetCursorPos(&MousePos);

	// ウィンドウ位置を計算.
	m_WindowRect = WindowRectMath();

	//----------------------------------------------------------------------------
	//		それぞれのUIの更新.
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		// 処理が必要のないものは早期に返す.
		if (i == EndSprite::Back) { continue; }
		if (i == EndSprite::Frame) { continue; }

		// UIのサイズと座標を変換する.
		D3DXVECTOR2 SquarePos = D3DXVECTOR2(m_pUIs[i]->GetPos().x, m_pUIs[i]->GetPos().y);
		D3DXVECTOR2 SquareDisp = D3DXVECTOR2(m_pUIs[i]->GetSpriteData().Disp.w, m_pUIs[i]->GetSpriteData().Disp.h);

		// 点と四角の当たり判定.
		if (CUIObject::PointInSquare(MousePos, SquarePos + m_WindowRect, SquareDisp))
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

		bool yesflag = false, noflag = false;

		// Yesにカーソルが重なっている時.
		if (i == EndSprite::SelectYes && m_pUIs[i]->GetPatternNo().x) {
			if (Mouse->IsLAction()) {
				yesflag = true;
			}
		}
		if (Key->IsKeyAction(DIK_Y)) {
			yesflag = true;
		}

		// Noにカーソルが重なっている時.
		if (i == EndSprite::SelectNo && m_pUIs[i]->GetPatternNo().x) {
			if (Mouse->IsLAction()) {
				noflag = true;
			}
		}
		if (Key->IsKeyAction(DIK_N)) {
			noflag = true;
		}
		

		// フラグの結果に合わせて処理をする.
		if (yesflag) {
			DestroyWindow(m_hWnd);
		}
		if (noflag) {
			m_EndDeleteFlag = true;
		}
	}
}


//=============================================================================
//		描画処理.
//=============================================================================
void CEndUI::Draw()
{
	// UIそれぞれの描画処理.
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		m_pUIs[i]->Draw();
	}
}


//=============================================================================
//		解放処理.
//=============================================================================
void CEndUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}


//-----------------------------------------------------------------------------
//		ウィンドウ位置の計算をまとめた関数.
//-----------------------------------------------------------------------------
D3DXVECTOR2 CEndUI::WindowRectMath()
{
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

	// フレーム幅を含んだウィンドウの位置を返す.
	return D3DXVECTOR2(
		static_cast<float>(borderLeft + WindowRect.left),
		static_cast<float>(borderTop + WindowRect.top));
}
