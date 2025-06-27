#include "SceneManager.h"

#include "Scenes/SceneBase.h"
#include "Scenes/UIEditor/UIEditor.h"
#include "Scenes/Game/Game.h"
#include "Scenes/Title/Title.h"

#include "Camera/Camera.h"
#include "Sprite/2D/UI/FadeUI/FadeUI.h"
#include "Sprite/2D/UI/EndUI/EndUI.h"
#include "DirectSound/SoundManager.h"
#include "FileManager/FileManager.h"
#include "ImGui/CImGui.h"


namespace {
	// Sceneのパス
	constexpr char ScenePath[] = "Data\\Scene.json";
}


SceneManager::SceneManager()
	: m_hWnd			()
	, m_SceneNo			()
	, m_NextSceneNo		()
	, m_pScene			()
	, m_FocusTime		( 0.f )
	, m_FocusTimeMax	( 35.f * Time::GetDeltaTime() )
	, m_EndDeleteFlag	( false )
{
}
SceneManager::~SceneManager()
{
}


//=============================================================================
//		シーンマネージャー構築関数
//=============================================================================
HRESULT SceneManager::Create(HWND hWnd)
{
	//自身のインスタンス
	auto* This = GetInstance();

	This->m_hWnd = hWnd;

	// 初めて呼び出されたときにインスタンス生成
	Json m_SceneData = nullptr;	// 画像情報
	m_SceneData = FileManager::JsonLoad(ScenePath);

	std::string SceneName = m_SceneData["Scene"].get<std::string>();
	if (SceneName == "Game") {
		This->m_pScene = This->CreateScene(SceneList::eGame);
	}
	else if (SceneName == "UIEditor") {
		This->m_pScene = This->CreateScene(SceneList::eUIEditor);
	}
	else {
		This->m_pScene = This->CreateScene(SceneList::eTitle);
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
void SceneManager::Update()
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
void SceneManager::Draw()
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
void SceneManager::Release()
{
	m_pScene->Release();
}


//=============================================================================
//		指定したシーンを読み込む
//=============================================================================
void SceneManager::LoadScene(SceneList Scene)
{
	m_pFade->DoFade(30,90,30);
	m_NextSceneNo = Scene;
}


//-----------------------------------------------------------------------------
//		指定されたシーンの生成
//-----------------------------------------------------------------------------
std::unique_ptr<SceneBase> SceneManager::CreateScene(SceneList No)
{
	m_SceneNo = No;
	RAINBOW_WINDOW = false;
	// コンストラクタでDeltaTimeを使用している変数用に初期化
	Time::GetInstance()->SetTimeScale(1.f);

	switch (No)
	{
	case eTitle:
		ChangeShowCursor(true);
		return std::make_unique<Title>( m_hWnd );
	case eGame:
		ChangeShowCursor(false);
		return std::make_unique<Game>( m_hWnd );
	case eUIEditor:
		ChangeShowCursor(true);
		return std::make_unique<UIEditor>( m_hWnd );
	default:
		return nullptr;
	}
	return nullptr;
}


//-----------------------------------------------------------------------------
//		カーソルの表示切替
//-----------------------------------------------------------------------------
void SceneManager::ChangeShowCursor(bool flag)
{
	// カーソル状態の取得
	CURSORINFO cursorInfo = {};
	cursorInfo.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&cursorInfo);

	// ShowCursorが重複して呼び出されないようにする
	if (cursorInfo.flags != CURSOR_SHOWING && flag) {
		Camera::GetInstance()->SetUseMouse(flag);
		ShowCursor(flag); 
	}
	else if (cursorInfo.flags == CURSOR_SHOWING && !flag) { 
		Camera::GetInstance()->SetUseMouse(flag);
		ShowCursor(flag);
	}
}


//-----------------------------------------------------------------------------
//		終了処理
//-----------------------------------------------------------------------------
void SceneManager::EndProcess()
{
	Key* Key = DirectInput::GetInstance()->CDKeyboard();

	// ESCが押された場合
	if (Key->IsKeyAction(DIK_ESCAPE)) {
		if (m_pEndUI == nullptr) {
			ChangeShowCursor(true);

			m_pEndUI = std::make_unique<EndUI>(m_hWnd);
			m_pEndUI->Create();
			m_EndDeleteFlag = false;
		}
		else if (m_pEndUI != nullptr) {
			if (m_SceneNo == SceneList::eTitle) { ChangeShowCursor(true); }
			else { ChangeShowCursor(false); }

			m_EndDeleteFlag = true;
		}
	}

	// EndUI削除フラグがあった場合
	if (m_EndDeleteFlag) {
		if (m_SceneNo == SceneList::eTitle) { ChangeShowCursor(true); }
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
void SceneManager::ConstantProcess() const
{
	BOOL isFullscreen = FALSE;
	IDXGISwapChain* swappchain = DirectX11::GetInstance()->GetSwapChain();

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
void SceneManager::ImGuiUpdate()
{
	if (!ISDEBUG) { return; }
	// ここでのみImGuiのUpdateを回す
	CImGui::GetInstance()->Update();

	// シーン選択のIMGUIを作成
	ImGui::Begin("SceneSelect");
	if (ImGui::Button("Game")) { GetInstance()->LoadScene(SceneList::eGame); }
	if (ImGui::Button("Title")) { GetInstance()->LoadScene(SceneList::eTitle); }
	ImGui::Separator();
	if (ImGui::Button("UIEditor")) { GetInstance()->LoadScene(SceneList::eUIEditor); }
	ImGui::End();

	ImGui::Begin("MousePos");
	POINT pos;
	if (GetCursorPos(&pos)) { ImGui::Text("%d,%d", pos.x, pos.y); }
	ImGui::End();
}


//-----------------------------------------------------------------------------
//		シーン更新処理
//-----------------------------------------------------------------------------
void SceneManager::SceneUpdate()
{
	if (m_pEndUI != nullptr) { return; }
	if (m_FocusTime > 0) { m_FocusTime -= Time::GetDeltaTime(); }

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