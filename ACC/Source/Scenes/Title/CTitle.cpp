#include "CTitle.h"

#include "Camera/CCamera.h"
#include "Mesh/Static/CStaticMesh.h"
#include "DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"

#ifdef _DEBUG
#include "ImGui/CImGui.h"
#endif

namespace {
	// タイトル画面UIのパス.
	constexpr char TitleImagePath[] = "Data\\Texture\\Title";
	
	// UIリスト.
	std::vector<std::string> TitleImageList = {
		"Black",
		"StartButton",
		"StartButton",
		"StartButton",
		"Title"
	};

	// 非処理UIリスト.
	std::vector<std::string> ignoreList = {
		"Black",
		"Title"
	};
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
	, m_Start			( false )
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
	std::unordered_map<std::string, int> nameCounts; // 名前ごとの出現回数を記録.

	for (size_t i = 0; i < TitleImageList.size(); ++i)
	{
		std::string baseName = TitleImageList[i];
		std::string numberedName;

		if (nameCounts.count(baseName) == 0)
		{
			numberedName = baseName;    // 1個目はそのまま
			nameCounts[baseName] = 1;    // 次からは1スタート
		}
		else
		{
			numberedName = baseName + "_" + std::to_string(nameCounts[baseName]);
			nameCounts[baseName]++;
		}

		// インスタンス生成
		m_pSprite2Ds.push_back(CSpriteManager::GetInstance()->GetSprite(baseName)); // ←ここはbaseNameでいい！
		m_pUIs.push_back(new CUIObject());

		// 画像データの読み込み
		m_pUIs.back()->AttachSprite(*m_pSprite2Ds.back());
		m_pUIs.back()->SetPos(m_pSprite2Ds.back()->GetSpriteData().Pos);
		m_SpritePosList.push_back(m_pUIs.back()->GetPos());

		// 名前だけしっかりつける
		m_pUIs.back()->SetSpriteName(numberedName);
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
	m_Start = false;
	CCamera::GetInstance()->Init();	// カメラの初期化.
}


//=================================================================================================
//		更新処理.
//=================================================================================================
void CTitle::Update()
{
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
		// 処理のいらないUIを早期に切る.
		if (std::find(ignoreList.begin(), ignoreList.end(), 
			m_pUIs[i]->GetSpriteData().Name) != ignoreList.end()) { continue; }

		// 点と四角の当たり判定.
		if (m_pUIs[i]->PointInSquare(MousePos, CLIENTRECT))
		{
			//	前回選択されていなかった場合SEを鳴らす.
			if ( m_pUIs[i]->GetPatternNo().x == 0 ) {
				CSoundManager::GetInstance()->Play(CSoundManager::SE_SelectMove);
			}
			m_pUIs[i]->SetPatternNo(1, 0);
		}
		else {
			m_pUIs[i]->SetPatternNo(0, 0);
		}

		// 特定の名前の場合.
		if (m_pUIs[i]->GetSpriteData().Name == "StartButton_1")
		{
			// すでにカーソルで選択されている場合.
			if ( m_pUIs[i]->GetPatternNo().x ) {
				if (Mouse->IsLAction()) { m_Start = true; }
			}
			else {
				// SPACEキーでゲーム開始.
				if (Key->IsKeyAction(DIK_SPACE)) {
					// 選択状態にする.
					m_pUIs[i]->SetPatternNo(1, 0);
					m_Start = true;
				}
			}
		}

		if (m_pUIs[i]->GetSpriteData().Name == "StartButton_2")
		{
			D3DXVECTOR3 pos = m_pUIs[i]->GetPos();
			pos.y += 1;
			m_pUIs[i]->SetPos(pos);
		}
	}

	if (m_Start) {
		// ゲームを開始する.
		CSceneManager::GetInstance()->LoadScene(SceneList::Game);
		CSoundManager::GetInstance()->AllStop();
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
		if (m_pUIs[i]->GetSpriteData().Name == TitleImageList[0])
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