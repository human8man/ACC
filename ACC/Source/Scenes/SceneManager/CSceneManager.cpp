#include "CSceneManager.h"

#include "Scenes/CSceneBase.h"
#include "Scenes/UIEditor/CUIEditor.h"
#include "Scenes/Game/CGame.h"
#include "Scenes/Title/CTitle.h"

#include "Camera/CCamera.h"
#include "Sprite/2D/UI/FadeUI/FadeUI.h"
#include "Sprite/2D/UI/EndUI/EndUI.h"
#include "DirectSound/CSoundManager.h"
#include "FileManager/FileManager.h"
#include "ImGui/CImGui.h"


namespace {
	// Sceneのパス
	constexpr char ScenePath[] = "Data\\Scene.json";
}


//=============================================================================
//		シーンマネージャークラス
//=============================================================================
CSceneManager::CSceneManager()
	: m_hWnd			()
	, m_SceneNo			()
	, m_NextSceneNo		()
	, m_pScene			()
	, m_FocusTime		( 0.f )
	, m_FocusTimeMax	( 35.f * CTime::GetDeltaTime() )
	, m_EndDeleteFlag	( false )
{
}
CSceneManager::~CSceneManager()
{
}


//=============================================================================
//		シーンマネージャー構築関数
//=============================================================================
HRESULT CSceneManager::Create(HWND hWnd)
{
	//自身のインスタンス
	auto* This = GetInstance();

	This->m_hWnd = hWnd;

	// 初めて呼び出されたときにインスタンス生成
	Json m_SceneData = nullptr;	// 画像情報
	m_SceneData = FileManager::JsonLoad(ScenePath);

	std::string SceneName = m_SceneData["Scene"].get<std::string>();
	if (SceneName == "Game") {
		This->m_pScene = This->CreateScene(SceneList::Game);
	}
	else if (SceneName == "UIEditor") {
		This->m_pScene = This->CreateScene(SceneList::UIEditor);
	}
	else {
		This->m_pScene = This->CreateScene(SceneList::Title);
	}
	This->m_pScene->Create();
	This->m_pScene->Init();
	This->m_pScene->LoadData();

	This->m_pFade = std::make_unique<FadeUI>();
	This->m_pFade->Create();

	return S_OK;
}


//=============================================================================
//		更新処理
//=============================================================================
void CSceneManager::Update()
{
	// 定数処理
	GetInstance()->ConstantProcess();
	// 終了処理
	GetInstance()->EndProcess();
	// ImGui処理
	GetInstance()->ImGuiUpdate();
	// シーン更新処理
	GetInstance()->SceneUpdate();
}


//=============================================================================
//		描画処理
//=============================================================================
void CSceneManager::Draw()
{
	m_pScene->Draw();
	m_pFade->Draw();

	// 存在する場合描画処理
	if (m_pEndUI != nullptr) { m_pEndUI->Draw(); }

	if (ISDEBUG) {
		// ここでのみImGuiのDrawを回す
		CImGui::GetInstance()->Draw();
	}
}


//=============================================================================
//		シーンマネージャー破棄
//=============================================================================
void CSceneManager::Release()
{
	m_pScene->Release();
}


//=============================================================================
//		指定したシーンを読み込む
//=============================================================================
void CSceneManager::LoadScene(SceneList Scene)
{
	m_pFade->DoFade(30,90,30);
	m_NextSceneNo = Scene;
}


//-----------------------------------------------------------------------------
//		指定されたシーンの生成
//-----------------------------------------------------------------------------
std::unique_ptr<CSceneBase> CSceneManager::CreateScene(SceneList No)
{
	m_SceneNo = No;
	RAINBOW_WINDOW = false;
	// コンストラクタでDeltaTimeを使用している変数用に初期化
	CTime::GetInstance()->SetTimeScale(1.f);

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
//		カーソルの表示切替
//-----------------------------------------------------------------------------
void CSceneManager::ChangeShowCursor(bool flag)
{
	// カーソル状態の取得
	CURSORINFO cursorInfo = {};
	cursorInfo.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&cursorInfo);

	// ShowCursorが重複して呼び出されないようにする
	if (cursorInfo.flags != CURSOR_SHOWING && flag) {
		CCamera::GetInstance()->SetUseMouse(flag);
		ShowCursor(flag); 
	}
	else if (cursorInfo.flags == CURSOR_SHOWING && !flag) { 
		CCamera::GetInstance()->SetUseMouse(flag);
		ShowCursor(flag);
	}
}


//-----------------------------------------------------------------------------
//		終了処理
//-----------------------------------------------------------------------------
void CSceneManager::EndProcess()
{
	CKey* Key = CInput::GetInstance()->CDKeyboard();

	// ESCが押された場合
	if (Key->IsKeyAction(DIK_ESCAPE)) {
		if (m_pEndUI == nullptr) {
			ChangeShowCursor(true);

			m_pEndUI = std::make_unique<EndUI>(m_hWnd);
			m_pEndUI->Create();
			m_EndDeleteFlag = false;
		}
		else if (m_pEndUI != nullptr) {
			if (m_SceneNo == SceneList::Title) { ChangeShowCursor(true); }
			else { ChangeShowCursor(false); }

			m_EndDeleteFlag = true;
		}
	}

	// EndUI削除フラグがあった場合
	if (m_EndDeleteFlag) {
		if (m_SceneNo == SceneList::Title) { ChangeShowCursor(true); }
		else { ChangeShowCursor(false); }
		m_pEndUI.reset();
		m_EndDeleteFlag = false;
	}

	// 存在する場合EndUI更新処理
	if (m_pEndUI != nullptr) {
		m_pEndUI->Update();
		m_EndDeleteFlag = m_pEndUI->GetDeleteFlag();
	}
}


//-----------------------------------------------------------------------------
//		定数処理
//-----------------------------------------------------------------------------
void CSceneManager::ConstantProcess()
{
	BOOL isFullscreen = FALSE;
	IDXGISwapChain* swappchain = CDirectX11::GetInstance()->GetSwapChain();

	BOOL isfullscreen;

#pragma warning(suppress : 6387)
	swappchain->GetFullscreenState(&isfullscreen, nullptr);
	FULLSCREEN = isfullscreen;

	// UIに使うためのウィンドウのスケールを計算
	RECT rect = {};
	GetClientRect(m_hWnd, &rect);
	NOWFWND_W = static_cast<float>(rect.right - rect.left);
	NOWFWND_H = static_cast<float>(rect.bottom - rect.top);
	FULLSCREENSCALEX = NOWFWND_W / FWND_W;
	FULLSCREENSCALEY = NOWFWND_H / FWND_H;
}


//-----------------------------------------------------------------------------
//		ImGui処理
//-----------------------------------------------------------------------------
void CSceneManager::ImGuiUpdate()
{
	if (!ISDEBUG) { return; }
	// ここでのみImGuiのUpdateを回す
	CImGui::GetInstance()->Update();

	// シーン選択のIMGUIを作成
	ImGui::Begin("SceneSelect");
	if (ImGui::Button("Game")) { GetInstance()->LoadScene(SceneList::Game); }
	if (ImGui::Button("Title")) { GetInstance()->LoadScene(SceneList::Title); }
	ImGui::Separator();
	if (ImGui::Button("UIEditor")) { GetInstance()->LoadScene(SceneList::UIEditor); }
	ImGui::End();

	ImGui::Begin("MousePos");
	POINT pos;
	if (GetCursorPos(&pos)) { ImGui::Text("%d,%d", pos.x, pos.y); }
	ImGui::End();
}


//-----------------------------------------------------------------------------
//		シーン更新処理
//-----------------------------------------------------------------------------
void CSceneManager::SceneUpdate()
{
	if (m_pEndUI != nullptr) { return; }
	if (m_FocusTime > 0) { m_FocusTime -= CTime::GetDeltaTime(); }

	// フェードのピーク時にシーンを切り替える
	if (m_pFade->GetFirstPeak()) {
		m_pScene.release();
		m_pScene = CreateScene(m_NextSceneNo);
		m_pScene->Create();
		m_pScene->Init();
		m_pScene->LoadData();
	}

	// フェード中以外、Update()を回す
	//	フェードのピーク時に一度だけ通し、フェード明け用の背景を作成する
	if (!m_pFade->GetFading() || m_pFade->GetFirstPeak()) {
		GetInstance()->m_pScene->Update();
	}

	// フェード終了時のみ
	if (m_pFade->GetFadeEndFrame()) {
		m_FocusTime = m_FocusTimeMax;
	}

	// フォーカス時間中は固定でウィンドウにフォーカスを合わせる
	if ( 0 < m_FocusTime) {
		SetForegroundWindow(m_hWnd);
		SetFocus(m_hWnd);
	}

	// フェードUIの更新処理
	m_pFade->Update();
}