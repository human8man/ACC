#include "CUIEditor.h"
#include "nlohmann/json.hpp"
#include "FileManager/FileManager.h"
#include "FileManager/LoadImage/LoadImage.h"


#ifdef _DEBUG
#include "ImGui/CImGui.h"
#endif


// json型を作成.
using Json = nlohmann::json;

namespace {
	// TitleUIのパス.
	constexpr char TitleImagePath[] = "Data\\Texture\\Title";
	// GameUIのパス.
	constexpr char GameImagePath[] = "Data\\Texture\\Game";
	// WinUIのパス.
	constexpr char WinImagePath[] = "Data\\Texture\\Win";
	// LoseUIのパス.
	constexpr char LoseImagePath[] = "Data\\Texture\\Lose";
}


//============================================================================
//		UIエディタークラス.
//============================================================================
CUIEditor::CUIEditor(HWND hWnd)
{
}
CUIEditor::~CUIEditor()
{
}


//=============================================================================
//		作成処理.
//=============================================================================
void CUIEditor::Create()
{
	SelectSceneLoad(UISceneList::Title);
}


//=============================================================================
//		データ読込.
//=============================================================================
HRESULT CUIEditor::LoadData()
{
	return E_NOTIMPL;
}


//=============================================================================
//		初期化処理.
//=============================================================================
void CUIEditor::Init()
{
}


//=============================================================================
//		更新処理.
//=============================================================================
void CUIEditor::Update()
{
	//--------------------------------------------------------------
	//		シーンを選択する.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("シーン選択ウィンドウ"));
	UISceneList scene;
	bool click = false;
	if (ImGui::Button("Title"))	{ scene = UISceneList::Title;	click = true; }
	if (ImGui::Button("Game"))	{ scene = UISceneList::Game;	click = true; }
	if (ImGui::Button("Lose"))	{ scene = UISceneList::Lose;	click = true; }
	if (ImGui::Button("Win"))	{ scene = UISceneList::Win;		click = true; }
	if (click && (MessageBox( NULL,
		L"本当に切り替えますか？\n(保存していない場合リセットされます)",  L"確認",
		MB_YESNO | MB_ICONQUESTION)) == IDYES) {
		SelectSceneLoad(scene);
	}
	ImGui::End();


	//--------------------------------------------------------------
	//		UI調整する.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI調整ウィンドウ"));

	ImGui::End();


	//--------------------------------------------------------------
	//		保存する.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI保存ウィンドウ"));
	if (ImGui::Button(IMGUI_JP("UIを保存"))) { SaveScene(); }
	ImGui::End();
}


//=============================================================================
//		描画処理.
//=============================================================================
void CUIEditor::Draw()
{
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
}


//=============================================================================
//		解放処理.
//=============================================================================
void CUIEditor::Release()
{
}


//-----------------------------------------------------------------------------
//		選択したシーンのUIを読み込み.
//-----------------------------------------------------------------------------
void CUIEditor::SelectSceneLoad(UISceneList scene)
{
	// 現在のUIをクリア.
	for (auto sprite : m_pSprite2Ds) { SAFE_DELETE(sprite); }
	m_pSprite2Ds.clear(); 
	for (auto ui : m_pUIs) { SAFE_DELETE(ui); }
	m_pUIs.clear(); 
	m_SpritePosList.clear();

	// シーンごとのパスを設定.
	switch (scene) {
	case Title:	m_ScenePath = TitleImagePath;	break;
	case Game:	m_ScenePath = GameImagePath;	break;
	case Win:	m_ScenePath = WinImagePath;		break;
	case Lose:	m_ScenePath = LoseImagePath;	break;
	default: return;
	}

	// 指定したパスを走査.
	for (const auto& entry : std::filesystem::directory_iterator(m_ScenePath)) {
		std::string extension = entry.path().extension().string();
		if (extension == ".json") continue;

		// インスタンス生成.
		auto sprite = new CSprite2D();
		auto ui = new CUIObject();

		// 画像データの読み込み.
		sprite->Init(entry.path().string());
		ui->AttachSprite(*sprite);
		ui->SetPos(sprite->GetSpriteData().Pos);

		// リストに追加.
		m_pSprite2Ds.push_back(sprite);
		m_pUIs.push_back(ui);
		m_SpritePosList.push_back(ui->GetPos());
	}
}


//-----------------------------------------------------------------------------
//		現在シーンのUI情報を保存.
//-----------------------------------------------------------------------------
HRESULT CUIEditor::SaveScene()
{
	for (size_t i = 0; i < m_pUIs.size(); ++i) 
	{
		// 作成するファイルパス.
		std::string TextPath = m_pUIs[i]->GetSpriteData().Path;

		// 情報を追加していく.
		Json SpriteState;
		SpriteState["Pos"]["x"]		= m_pUIs[i]->GetPos().x;
		SpriteState["Pos"]["y"]		= m_pUIs[i]->GetPos().y;
		SpriteState["Pos"]["z"]		= m_pUIs[i]->GetPos().z;
		SpriteState["Base"]["w"]	= m_pUIs[i]->GetSpriteData().Base.w;
		SpriteState["Base"]["h"]	= m_pUIs[i]->GetSpriteData().Base.h;
		SpriteState["Disp"]["w"]	= m_pUIs[i]->GetSpriteData().Disp.w;
		SpriteState["Disp"]["h"]	= m_pUIs[i]->GetSpriteData().Disp.h;
		SpriteState["Stride"]["w"]	= m_pUIs[i]->GetSpriteData().Stride.w;
		SpriteState["Stride"]["h"]	= m_pUIs[i]->GetSpriteData().Stride.h;

		SpriteState["Alpha"]		= m_pUIs[i]->GetAlpha();
		SpriteState["Scale"]["x"]	= m_pUIs[i]->GetScale().x;
		SpriteState["Scale"]["y"]	= m_pUIs[i]->GetScale().y;
		SpriteState["Scale"]["z"]	= m_pUIs[i]->GetScale().z;
		SpriteState["Rotate"]["x"]	= m_pUIs[i]->GetRot().x;
		SpriteState["Rotate"]["y"]	= m_pUIs[i]->GetRot().y;
		SpriteState["Rotate"]["z"]	= m_pUIs[i]->GetRot().z;

		// スプライト情報の作成.
		if (FAILED(FileManager::JsonSave(TextPath, SpriteState))) return E_FAIL;
	}
	return S_OK;
}