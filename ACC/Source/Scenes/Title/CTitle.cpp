#include "CTitle.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
#include "Mesh/Static/CStaticMesh.h"
#include "Camera/CCamera.h"

#ifdef _DEBUG
#include "ImGui/CImGui.h"
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
//		作成処理.
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
//		読込処理.
//=================================================================================================
HRESULT CTitle::LoadData()
{
	m_pEgg->Init(_T("Data\\Mesh\\Static\\Player\\egg.x"));
	m_pEgg->SetPos(D3DXVECTOR3(10.f, -1.f, 4.f));

	return S_OK;
}


//=================================================================================================
//		初期化処理.
//=================================================================================================
void CTitle::Init()
{
	CCamera::GetInstance()->Init();	// カメラの初期化.
}


//=================================================================================================
//		更新処理.
//=================================================================================================
void CTitle::Update()
{
	//カーソルを表示させる.
	ShowCursor(true);
	// BGM再生.
	CSoundManager::GetInstance()->PlayLoop(CSoundManager::enList::BGM_Title);

	CMouse*	Mouse	= CInput::GetInstance()->CDMouse();
	CKey*	Key		= CInput::GetInstance()->CDKeyboard();

	m_pEgg->SetRotY(m_pEgg->GetRot().y + 0.01f);

	// マウス位置を取得.
	POINT MousePos;
	GetCursorPos(&MousePos);

	//----------------------------------------------------------------------------
	//		それぞれのUIの更新.
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		// 背景の処理はいらないので早期に切る.
		if (i == TitleSprite::FullScreen) { continue; }
		if (i == TitleSprite::Title) { continue; }

		// UIのサイズと座標を変換する.
		D3DXVECTOR2 SquarePos	= D3DXVECTOR2( m_pUIs[i]->GetPos().x, m_pUIs[i]->GetPos().y );
		D3DXVECTOR2 SquareDisp	= D3DXVECTOR2( m_pUIs[i]->GetSpriteData().Disp.w, m_pUIs[i]->GetSpriteData().Disp.h );

		// 点と四角の当たり判定.
		if (m_pUIs[i]->PointInSquare(MousePos, m_pUIs[i]->WindowRect(m_hWnd)))
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
		if (i == TitleSprite::StartButton) 
		{
			// すでにカーソルで選択されている場合.
			if ( m_pUIs[i]->GetPatternNo().x ) {
				if (Mouse->IsLAction()) {
					// ゲームを開始する.
					CSceneManager::GetInstance()->LoadScene(SceneList::Game);
					CSoundManager::GetInstance()->Stop(CSoundManager::enList::BGM_Title);
				}
			}
			else {
				// SPACEキーでゲーム開始.
				if (Key->IsKeyAction(DIK_SPACE)) {
					// 選択状態にする.
					m_pUIs[i]->SetPatternNo(1, 0);
					// ゲームを開始する.
					CSceneManager::GetInstance()->LoadScene(SceneList::Game);
					CSoundManager::GetInstance()->Stop(CSoundManager::enList::BGM_Title);
				}
			}
		}
	}
}


//=================================================================================================
//		描画処理.
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


//=================================================================================================
//		解放処理.
//=================================================================================================
void CTitle::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
	SAFE_DELETE(m_pEgg);
}