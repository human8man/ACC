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
#ifdef _DEBUG
	//--------------------------------------------------------------
	//		シーンを選択する.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UIEditor用シーン選択"));
	UISceneList scene;
	bool click = false;
	if (ImGui::Button("Title"))	{ scene = UISceneList::Title;	click = true; }
	if (ImGui::Button("Game"))	{ scene = UISceneList::Game;	click = true; }
	if (ImGui::Button("Lose"))	{ scene = UISceneList::Lose;	click = true; }
	if (ImGui::Button("Win"))	{ scene = UISceneList::Win;		click = true; }
	if (m_MovedSomething && click && (MessageBox(NULL,
		L"本当に切り替えますか？\n(保存していない場合リセットされます)",  L"確認",
		MB_YESNO | MB_ICONQUESTION)) == IDYES) {
		SelectSceneLoad(scene);
		m_MovedSomething = false;
	}
	else if(!m_MovedSomething && click){
		SelectSceneLoad(scene);
	}

	ImGui::End();


	//--------------------------------------------------------------
	//		UI調整する.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI調整ウィンドウ"));

	if (ImGui::TreeNodeEx(IMGUI_JP("UIリスト"), ImGuiTreeNodeFlags_DefaultOpen)) {
		// 検索バー.
		ImGui::InputText(IMGUI_JP("検索"), m_SearchBuffer, IM_ARRAYSIZE(m_SearchBuffer));
		// スクロール可能なリスト.
		ImGui::BeginChild(IMGUI_JP("リスト"), ImVec2(315, 100), true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < m_pUIs.size(); ++i) {
			// 検索フィルタリング.
			if (strlen(m_SearchBuffer) > 0 
			&& m_pUIs[i]->GetSpriteData().Name.find(m_SearchBuffer) == std::string::npos) {
				continue;
			}

			bool isSelected = (m_SelectedUIIndex == i);
			if (ImGui::Selectable(m_pUIs[i]->GetSpriteData().Name.c_str(), isSelected)) {
				m_SelectedUIIndex = i; // 選択更新.
			}
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}

	// 選択中のオブジェクトを編集.
	if (m_SelectedUIIndex >= 0 && m_SelectedUIIndex < m_pUIs.size()) {
		// 選択されたUIを呼びやすくする.
		CUIObject* selectedUI = m_pUIs[m_SelectedUIIndex];
		// 選択されているUIを表示.
		ImGui::Text(IMGUI_JP("選択されているUI: %s"), selectedUI->GetSpriteData().Name.c_str());

		// 座標の調整.
		if (ImGui::TreeNode(IMGUI_JP("座標"))) {
			D3DXVECTOR3 pos	= selectedUI->GetPos();
			bool posdrag	= ImGui::DragFloat3("##Position", pos, 1.f);
			bool posinput	= ImGui::InputFloat3("##InputPos", pos);

			// 変更があった場合保存する.
			if (posdrag || posinput) {
				selectedUI->SetPos(pos); 
				m_MovedSomething = true;
			}
			ImGui::TreePop();
		}

		// 画像情報の調整.
		if (ImGui::TreeNode(IMGUI_JP("画像情報"))) {
			D3DXVECTOR2 base	= D3DXVECTOR2(selectedUI->GetSpriteData().Base.w, selectedUI->GetSpriteData().Base.h);
			D3DXVECTOR2 disp	= D3DXVECTOR2(selectedUI->GetSpriteData().Disp.w, selectedUI->GetSpriteData().Disp.h);
			D3DXVECTOR2 stride	= D3DXVECTOR2(selectedUI->GetSpriteData().Stride.w, selectedUI->GetSpriteData().Stride.h);

			ImGui::Text(IMGUI_JP("元のサイズ(x,y)"));
			bool basedrag	= ImGui::DragFloat2("##BaseDrag", base, 1.f);
			bool baseinput	= ImGui::InputFloat2("##BaseInput", base);

			ImGui::Text(IMGUI_JP("表示サイズ(x,y)"));
			bool dispdrag	= ImGui::DragFloat2("##DispDrag", disp, 1.f);
			bool dispinput	= ImGui::InputFloat2("##DispInput", disp);

			ImGui::Text(IMGUI_JP("分割サイズ(x,y)"));
			bool stridedrag		= ImGui::DragFloat2("##StrideDrag", stride, 1.f);
			bool strideinput	= ImGui::InputFloat2("##StrideInput", stride);

			// 変更があった場合保存する.
			if (basedrag	|| baseinput
			||	dispdrag	|| dispinput
			||	stridedrag	|| strideinput)
			{
				selectedUI->SetBase(base);
				selectedUI->SetDisp(disp);
				selectedUI->SetStride(stride);
				m_MovedSomething = true;
			}
			ImGui::TreePop();
		}

		// その他の情報の調整.
		if (ImGui::TreeNode(IMGUI_JP("その他"))) {
			float alpha = selectedUI->GetAlpha();
			D3DXVECTOR3 scale	= selectedUI->GetScale();
			D3DXVECTOR3 rot		= selectedUI->GetRot();

			ImGui::Text(IMGUI_JP("アルファ"));
			bool alphaslider	= ImGui::SliderFloat("##AlphaSlider", &alpha, 0.f, 1.f);
			bool alphainput		= ImGui::InputFloat("##AlphaInput", &alpha);

			ImGui::Text(IMGUI_JP("スケール"));
			bool scaledrag	= ImGui::DragFloat3("##ScaleDrag", scale, 0.1f);
			bool scaleinput	= ImGui::InputFloat3("##ScaleInput", scale);

			ImGui::Text(IMGUI_JP("回転"));
			bool rotdrag	= ImGui::DragFloat3("##RotDrag", rot, 0.1f);
			bool rotinput	= ImGui::InputFloat3("##RotInput", rot);

			// 変更があった場合保存する.
			if (alphaslider	|| alphainput
			||	scaledrag	|| scaleinput
			||	rotdrag		|| rotinput) 
			{
				selectedUI->SetAlpha(alpha);
				selectedUI->SetScale(scale);
				selectedUI->SetRot(rot);
				m_MovedSomething = true;
			}
			ImGui::TreePop();
		}
	}


	ImGui::End();

	//--------------------------------------------------------------
	//		保存する.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI保存ウィンドウ"));
	if (ImGui::Button(IMGUI_JP("UIを保存"))) {
		SaveScene(); 
		m_MovedSomething = false;
	}
	ImGui::End();
#endif
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