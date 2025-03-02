#include "CTitle.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "Common/DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
#include "Mesh/Static/CStaticMesh.h"
#include "Camera/CCamera.h"


#if _DEBUG
#include "Common/ImGui/CImGui.h"
#endif


namespace {
	// タイトル画面UIのパス.
	constexpr char TitleImagePath[] = "Data\\Texture\\Title";
}


//=================================================================================================
//		タイトルクラス.
//=================================================================================================
CTitle::CTitle(HWND hWnd)
	: m_Light			()
	, m_mView			()
	, m_mProj			()
	, m_SpriteDataList	()
	, m_SpritePosList	()
	, m_pUIs			()
	, m_pSprite2Ds		()
	, m_pEgg			()
{
	m_hWnd = hWnd;
	m_Light.vDirection = D3DXVECTOR3(1.f, 5.f, 0.f);	//ライト方向.
}

CTitle::~CTitle()
{
	Release();
}


//=================================================================================================
//		構築関数.
//=================================================================================================
void CTitle::Create()
{
	int index = 0;

	// 指定したディレクトリ内を走査.
	for (const auto& entry : std::filesystem::directory_iterator(TitleImagePath)) {
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
	m_pEgg = new CStaticMesh();

}


//=================================================================================================
//		読み込み.
//=================================================================================================
HRESULT CTitle::LoadData()
{
	m_pEgg->Init(_T("Data\\Mesh\\Static\\Player\\egg.x"));
	m_pEgg->SetPos(D3DXVECTOR3(9.f, -1.5f, 5.f));

	return S_OK;
}


//=================================================================================================
//		解放処理.
//=================================================================================================
void CTitle::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
	SAFE_DELETE(m_pEgg);
}


//=================================================================================================
//		初期化.
//=================================================================================================
void CTitle::Init()
{
	CCamera::GetInstance()->Init();	// カメラの初期化.
}


//=================================================================================================
//		更新.
//=================================================================================================
void CTitle::Update()
{
	ShowCursor(true);
	// BGM再生.
	CSoundManager::GetInstance()->PlayLoop(CSoundManager::enList::BGM_Title);

	CMouse* Mouse = CInput::GetInstance()->CDMouse();
	CXInput* XInput = CInput::GetInstance()->CDXInput();
	m_pEgg->SetRotY(m_pEgg->GetRot().y + 0.01f);

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
	POINT topLeft	  = { clientRect.left, clientRect.top };
	POINT bottomRight = { clientRect.right, clientRect.bottom };

	// クライアント領域の座標をスクリーン座標系に変換.
	ClientToScreen(m_hWnd, &topLeft);
	ClientToScreen(m_hWnd, &bottomRight);

	// ウィンドウ全体の左上座標とクライアント領域の左上座標の差分を計算.
	int borderLeft	= topLeft.x - WindowRect.left;
	int borderTop	= topLeft.y - WindowRect.top;

	// フレーム幅を含んだウィンドウの位置を算出.
	D3DXVECTOR2 windowrect = D3DXVECTOR2(
		static_cast<float>(borderLeft + WindowRect.left),
		static_cast<float>(borderTop + WindowRect.top));

	//----------------------------------------------------------------------------
	//		それぞれのUIの更新.
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		// 背景の処理はいらないので早期に切る.
		if (i == TitleSprite::FullScreen) { continue; }

		// UIのサイズと座標を変換する.
		D3DXVECTOR2 SquarePos	= D3DXVECTOR2( m_pUIs[i]->GetPos().x, m_pUIs[i]->GetPos().y );
		D3DXVECTOR2 SquareDisp	= D3DXVECTOR2( m_pUIs[i]->GetSpriteData().Disp.w, m_pUIs[i]->GetSpriteData().Disp.h );


		// 点と四角の当たり判定.
		if (PointInSquare(MousePos, SquarePos + windowrect, SquareDisp)) 
		{
			//	前回選択されていなかった場合SEを鳴らす.
			if ( m_pUIs[i]->GetPatternNo().x == 0 ) {
				CSoundManager::GetInstance()->PlaySE(CSoundManager::SE_SelectMove);
			}
			m_pUIs[i]->SetPatternNo(1, 0);
		}
		else {
			m_pUIs[i]->SetPatternNo(0, 0);
		}


		// スタートボタンにカーソルが重なっている時.
		if (i == TitleSprite::StartButton && m_pUIs[i]->GetPatternNo().x) {
			if (Mouse->IsLAction()) {
				// ゲームを開始する.
				CSceneManager::GetInstance()->LoadScene(SceneList::Game);
				CSoundManager::GetInstance()->Stop(CSoundManager::enList::BGM_Title);
			}
		}
		//else if (XInput->IsUp(XInput->A))
		//{
		//	// ゲームを開始する.
		//	CSceneManager::GetInstance()->LoadScene(SceneList::Game);
		//	CSoundManager::GetInstance()->Stop(CSoundManager::enList::BGM_Title);
		//}
	}
	
//#if _DEBUG
//	ImGui::Begin("cWindow");
//
//	ImGui::Text("%d", XInput->GetPadID());
//	ImGui::Text("%d", XInput->IsConnect());
//
//	ImGui::End();
//#endif
}


//=================================================================================================
//		描画.
//=================================================================================================
void CTitle::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);
	
	// UIそれぞれの描画処理.
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		m_pUIs[i]->Draw();
		if (i == TitleSprite::FullScreen)
		{
			m_pEgg->Render(m_mView, m_mProj, m_Light);
		}
	}
}


//------------------------------------------------------------------------------------------------
//		初期化.
//------------------------------------------------------------------------------------------------
bool CTitle::PointInSquare( POINT ppos, D3DXVECTOR2 spos, D3DXVECTOR2 sposs)
{
	if (spos.x < ppos.x
	&&  spos.y < ppos.y
	&&  ppos.x < spos.x + sposs.x
	&&  ppos.y < spos.y + sposs.y )
	{
		return true;
	}

	return false;
}
