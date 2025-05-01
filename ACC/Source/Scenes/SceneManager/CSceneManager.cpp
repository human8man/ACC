#include "CSceneManager.h"

#include "Scenes/CSceneBase.h"
#include "Scenes/UIEditor/CUIEditor.h"
#include "Scenes/Game/CGame.h"
#include "Scenes/Title/CTitle.h"

#include "Camera/CCamera.h"
#include "Sprite/2D/UI/CUIFade/CUIFade.h"
#include "Sprite/2D/UI/CEndUI/CEndUI.h"
#include "DirectSound/CSoundManager.h"


#ifdef _DEBUG
	#include "ImGui/CImGui.h"
#endif


//=============================================================================
//		シーンマネージャークラス.
//=============================================================================
CSceneManager::CSceneManager()
	: m_hWnd			()
	, m_SceneNo			()
	, m_NextSceneNo		()
	, m_pScene			()
	, m_EndDeleteFlag	( false )
{
}

CSceneManager::~CSceneManager()
{
	Release();
}


//=============================================================================
//		シーンマネージャー構築関数.
//=============================================================================
HRESULT CSceneManager::Create(HWND hWnd)
{
	//自身のインスタンス
	auto* This = GetInstance();

	This->m_hWnd = hWnd;

	//初めて呼び出されたときにインスタンス生成
	This->m_pScene = This->CreateScene(SceneList::Title);
	This->m_pScene->Create();
	This->m_pScene->Init();
	This->m_pScene->LoadData();

	This->m_pFade = std::make_unique<CUIFade>();
	This->m_pFade->Create();


	// サウンドデータの読み込み.
	if (CSoundManager::GetInstance()->Load(hWnd) == false) {
		return E_FAIL;
	}

	return S_OK;
}


//=============================================================================
//		更新処理.
//=============================================================================
void CSceneManager::Update()
{
	CKey* Key = CInput::GetInstance()->CDKeyboard();
	BOOL isFullscreen = FALSE;
	IDXGISwapChain* swappchain = CDirectX11::GetInstance()->GetSwapChain();

	BOOL isfullscreen;

#pragma warning(suppress : 6387)
	swappchain->GetFullscreenState(&isfullscreen, nullptr);
	FULLSCREEN = isfullscreen;

	// UIに使うためのウィンドウのスケールを計算.
	RECT rect = {};
	GetClientRect(m_hWnd, &rect);
	NOWFWND_W = static_cast<float>(rect.right - rect.left);
	NOWFWND_H = static_cast<float>(rect.bottom - rect.top);
	FULLSCREENSCALEX = NOWFWND_W / FWND_W;
	FULLSCREENSCALEY = NOWFWND_H / FWND_H;

	// ESCが押された場合.
	if (Key->IsKeyAction(DIK_ESCAPE)) {
		if (m_pEndUI == nullptr) {
			ChangeShowCursor(true);

			m_pEndUI = std::make_unique<CEndUI>();
			m_pEndUI->Create(m_hWnd);
			m_EndDeleteFlag = false;
		}
		else if (m_pEndUI != nullptr) {
			if (m_SceneNo == SceneList::Title) { ChangeShowCursor(true); }
			else{ ChangeShowCursor(false); }

			m_EndDeleteFlag = true;
		}
	}

	// EndUI削除フラグがあった場合.
	if (m_EndDeleteFlag) {
		if (m_SceneNo == SceneList::Title) { ChangeShowCursor(true); }
		else { ChangeShowCursor(false); }
		m_pEndUI.reset();
		m_EndDeleteFlag = false;
	}

	// 存在する場合EndUI更新処理.
	if (m_pEndUI != nullptr) { 
		m_pEndUI->Update(); 
		m_EndDeleteFlag = m_pEndUI->GetDeleteFlag();
	}

#ifdef _DEBUG
	// ここでのみImGuiのUpdateを回す.
	CImGui::GetInstance()->Update();

	// シーン選択のIMGUIを作成.
	ImGui::Begin("SceneSelect");
	if (ImGui::Button("Game")) { GetInstance()->LoadScene(SceneList::Game); }
	if (ImGui::Button("Title")) { GetInstance()->LoadScene(SceneList::Title); }

	ImGui::Text("\n");
	ImGui::Text(IMGUI_JP("-----デバッグ用シーン------------------------------------"));
	if (ImGui::Button("UIEditor")) { GetInstance()->LoadScene(SceneList::UIEditor); }
	ImGui::End();

	ImGui::Begin("MousePos");
	POINT pos;
	if (GetCursorPos(&pos)) { ImGui::Text("%d,%d", pos.x, pos.y); }
	ImGui::End();
#endif

	if (m_pEndUI == nullptr) {
		// フェードのピーク時にシーンを切り替える.
		if (m_pFade->GetFadePeak()) {
			m_pScene.release();
			m_pScene = CreateScene(m_NextSceneNo);
			m_pScene->Create();
			m_pScene->Init();
			m_pScene->LoadData();
		}

		// フェード中以外、Update()を回す.
		//	フェードのピーク時に一度だけ通し、フェード明け用の背景を作成する.
		if (!m_pFade->GetFading() || m_pFade->GetFadePeak()) {
			GetInstance()->m_pScene->Update();
		}

		// フェードUIの更新処理.
		m_pFade->Update();
	}
}


//=============================================================================
//		描画処理.
//=============================================================================
void CSceneManager::Draw()
{
	m_pScene->Draw();
	m_pFade->Draw();

	// 存在する場合描画処理.
	if (m_pEndUI != nullptr) { m_pEndUI->Draw(); }

#ifdef _DEBUG
	// ここでのみImGuiのDrawを回す.
	CImGui::GetInstance()->Draw();
#endif
}


//=============================================================================
//		シーンマネージャー破棄.
//=============================================================================
void CSceneManager::Release()
{
	//m_pScene->Release();
	m_pFade->Release();
}


//=============================================================================
//		指定したシーンを読み込む.
//=============================================================================
void CSceneManager::LoadScene(SceneList Scene)
{
	m_pFade->DoFade(120);
	m_NextSceneNo = Scene;
}


//-----------------------------------------------------------------------------
//		指定されたシーンの生成.
//-----------------------------------------------------------------------------
std::unique_ptr<CSceneBase> CSceneManager::CreateScene(SceneList No)
{
	m_SceneNo = No;

	switch (No)
	{
	case Title:
		ChangeShowCursor(true);
		return std::make_unique<CTitle>( m_hWnd );
	case Game:
		ChangeShowCursor(false);
		return std::make_unique<CGame>( m_hWnd );
	case UIEditor:
		ChangeShowCursor(true);
		return std::make_unique<CUIEditor>( m_hWnd );
	default:
		return nullptr;
	}
	return nullptr;
}


//-----------------------------------------------------------------------------
//		カーソルの表示切替.
//-----------------------------------------------------------------------------
void CSceneManager::ChangeShowCursor(bool flag)
{
	// カーソル状態の取得.
	CURSORINFO cursorInfo = {};
	cursorInfo.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&cursorInfo);

	// ShowCursorが重複して呼び出されないようにする.
	if (cursorInfo.flags != CURSOR_SHOWING && flag) {
		CCamera::GetInstance()->SetUseMouse(flag);
		ShowCursor(flag); 
	}
	else if (cursorInfo.flags == CURSOR_SHOWING && !flag) { 
		CCamera::GetInstance()->SetUseMouse(flag);
		ShowCursor(flag);
	}
}