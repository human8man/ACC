#include "CUIEditor.h"
#include "Time/CTime.h"
#include "nlohmann/json.hpp"
#include "FileManager/FileManager.h"
#include "DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
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
	m_hWnd = hWnd;
}
CUIEditor::~CUIEditor()
{
}


//=============================================================================
//		作成処理.
//=============================================================================
void CUIEditor::Create()
{
	CSoundManager::GetInstance()->Stop(CSoundManager::enList::BGM_Title);
	CSoundManager::GetInstance()->Stop(CSoundManager::enList::BGM_Game);
	SelectSceneLoad(UISceneList::Title);
	SelectInit();
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


//-----------------------------------------------------------------------------
//		UI選択時に仮変数等を初期化する.
//-----------------------------------------------------------------------------
void CUIEditor::SelectInit()
{
	m_PatternNo		= POINTS(0, 0);
	m_PatternMax	= POINTS(1, 1);
	m_PatternAuto	= false;
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
				SelectInit();
			}
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}

	//-----------------------------------------------------------
	//		選択中のオブジェクトの編集.
	//-----------------------------------------------------------
	if (m_SelectedUIIndex >= 0 && m_SelectedUIIndex < m_pUIs.size()) {
		// 選択されたUIを呼びやすくする.
		CUIObject* selectedUI = m_pUIs[m_SelectedUIIndex];
		// 選択されているUIを表示.
		ImGui::Text(IMGUI_JP("選択されているUI: %s"), selectedUI->GetSpriteData().Name.c_str());

		//-----------------------------------------------------------
		//		座標の調整.
		//-----------------------------------------------------------
		if (ImGui::TreeNode(IMGUI_JP("座標"))) {
			// ドラッグ用にマウス操作のDirectInpuを用意.
			CMouse* Mouse = CInput::GetInstance()->CDMouse();
			D3DXVECTOR3 pos	= selectedUI->GetPos();
			bool posdrag	= ImGui::DragFloat3("##Position", pos, 1.f);
			bool posinput	= ImGui::InputFloat3("##InputPos", pos);

			// マウス位置を取得.
			POINT MousePos;
			GetCursorPos(&MousePos);
			// 画像範囲内で左クリック入力中の場合、ドラッグ操作を開始.
			if (selectedUI->PointInSquare(MousePos, selectedUI->WindowRect(m_hWnd)) && !m_DoDrag) {
				if (Mouse->IsLAction() ) { 
					m_DoDrag = true; 
					m_OffsetPos = D3DXVECTOR2(pos.x - MousePos.x, pos.y - MousePos.y);
				}
			}
			if (m_DoDrag) {
				posdrag = true;
				// 補正値+マウス座標した座標を入れる.
				pos = D3DXVECTOR3(MousePos.x + m_OffsetPos.x, MousePos.y + m_OffsetPos.y, pos.z);
				// マウスの左クリックを話した場合、ドラッグ操作を停止.
				if (!Mouse->IsLDown()) { m_DoDrag = false; }
			}

			// 変更があった場合保存する.
			if (posdrag || posinput) {
				selectedUI->SetPos(pos); 
				m_MovedSomething = true;
			}
			ImGui::TreePop();
		}

		//-----------------------------------------------------------
		//		画像情報の調整.
		//-----------------------------------------------------------
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

		//-----------------------------------------------------------
		//		画像パターンを試す.
		//-----------------------------------------------------------
		if (ImGui::TreeNode(IMGUI_JP("画像パターンを試す"))) {
			 m_PatternNo = selectedUI->GetPatternNo();
			int pattern[2] = { m_PatternNo.x,m_PatternNo.y };
			int patternmax[2] = { m_PatternMax.x,m_PatternMax.y };


			// パターンの最大数を決める.
			ImGui::Text(IMGUI_JP("パターンの上限"));
			ImGui::PushItemWidth(100.0f);
			ImGui::InputInt("##x", &patternmax[0]); ImGui::SameLine(); ImGui::InputInt("##y", &patternmax[1]);
			ImGui::PopItemWidth(); 
			// 下限は1固定.
			if (patternmax[0] < 1) { patternmax[0] = 1; }
			if (patternmax[1] < 1) { patternmax[1] = 1; }
			m_PatternMax = POINTS(patternmax[0], patternmax[1]);

			// パターンのクリック調整.
			if (ImGui::TreeNode(IMGUI_JP("クリック調整"))) {
				ImGui::PushItemWidth(100.0f);
				ImGui::InputInt("##xclickpattern", &pattern[0]); ImGui::SameLine(); ImGui::InputInt("##yclickpattern", &pattern[1]);
				ImGui::PopItemWidth();
				ImGui::TreePop();
			}

			// パターンのオートラン調整.
			if (ImGui::TreeNode(IMGUI_JP("オートラン調整"))) {
				// 実行中の処理.
				if (m_PatternAuto) {
					ImGui::Text("On");
					m_AnimationSpeed -= CTime::GetInstance()->GetDeltaTime();
					if (m_AnimationSpeed < 0) {
						m_AnimationSpeed = m_AnimationSpeedMax * CTime::GetInstance()->GetDeltaTime();
						pattern[0]++;

						// xが最大値を超え、yが最大値の場合アニメーションが最初から送られるようにする.
						if (m_PatternMax.x < pattern[0] && m_PatternMax.y == pattern[1]) {
							// yが0以下になった場合は初期状態にする.
							pattern[0] = 0; pattern[1] = 0;
						}
					}
				}
				else {
					ImGui::Text("Off");
					m_AnimationSpeed = m_AnimationSpeedMax * CTime::GetInstance()->GetDeltaTime();
				}
				ImGui::PushItemWidth(100.0f);
				// 実行の切り替え.
				if (ImGui::Button(IMGUI_JP("切替"))) { m_PatternAuto = !m_PatternAuto; }
				// 送り速度の設定.
				ImGui::InputFloat(IMGUI_JP("送り速度設定(フレーム)"), &m_AnimationSpeedMax);
				ImGui::PopItemWidth();

				ImGui::TreePop();
			}

			// Xが最大値を超えた場合.
			if (m_PatternMax.x < pattern[0]) {
				// Yが最大値以上の場合、Xを最大値にする.
				if (m_PatternMax.y <= pattern[1]) {
					pattern[0] = m_PatternMax.x;
				}
				else {
					pattern[0] = 0; pattern[1]++;
				}
			}
			else if (pattern[0] < 0) {
				// 最低値は0に固定し、yの値を繰り下げる.
				pattern[0] = 0; pattern[1]--;
			}

			// Yが最大値を超えた場合.
			if (m_PatternMax.y < pattern[1]) {
				pattern[0] = m_PatternMax.x;
				pattern[1] = m_PatternMax.y;
			}
			else if (pattern[1] < 0) {
				// yが0以下になった場合は初期状態にする.
				pattern[0] = 0; pattern[1] = 0;
			}

			// 反映する.
			m_PatternNo = POINTS(pattern[0], pattern[1]);
			selectedUI->SetPatternNo(m_PatternNo.x, m_PatternNo.y);
			ImGui::TreePop();
		}

		//-----------------------------------------------------------
		//		その他の情報の調整.
		//-----------------------------------------------------------
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