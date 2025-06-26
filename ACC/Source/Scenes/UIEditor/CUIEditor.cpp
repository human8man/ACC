#include "CUIEditor.h"
#include "DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
#include "FileManager/FileManager.h"
#include "FileManager/LoadImage/LoadImage.h"
#include "nlohmann/json.hpp"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "Time/CTime.h"
#include "ImGui/CImGui.h"


// json型を作成
using Json = nlohmann::json;

namespace {
	// シェーダーのパス
	const TCHAR ShaderPath[] = _T("Data\\Shader\\UIEditorLineShader.hlsl");

	struct Vertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR4 color;
	};

	Vertex lineVertices[] =
	{
		{ { -50.0f, 360.0f, 0.0f }, { 1, 0, 0, 1 } },  // 左上
		{ {  50.0f, 360.0f, 0.0f }, { 1, 0, 0, 1 } },  // 右上

		{ {  50.0f, 360.0f, 0.0f }, { 1, 0, 0, 1 } },  // 右上
		{ {  50.0f, -360.0f, 0.0f }, { 1, 0, 0, 1 } }, // 右下

		{ {  50.0f, -360.0f, 0.0f }, { 1, 0, 0, 1 } }, // 右下
		{ { -50.0f, -360.0f, 0.0f }, { 1, 0, 0, 1 } }, // 左下

		{ { -50.0f, -360.0f, 0.0f }, { 1, 0, 0, 1 } }, // 左下
		{ { -50.0f, 360.0f, 0.0f }, { 1, 0, 0, 1 } },  // 左上
	};

	struct CBUFFER_MATRIX {
		D3DXMATRIX mWorld;
		D3DXMATRIX mView;
		D3DXMATRIX mProj;
		float LineThickness; // 太さ（ピクセル単位）
		D3DXVECTOR3 padding; // サイズ調整 (16バイト境界)
	};

	char m_NewSceneName[64] = ""; // 新規作成用バッファをメンバ変数に追加
}


//============================================================================
//		UIエディタークラス
//============================================================================
CUIEditor::CUIEditor(HWND hWnd)
{
	m_hWnd = hWnd;
}
CUIEditor::~CUIEditor()
{
}


//=============================================================================
//		作成処理
//=============================================================================
void CUIEditor::Create()
{
	CDirectX11::GetInstance()->SetChangeDebugBuffer(true);
	CSoundManager::GetInstance()->AllStop();
	SelectSceneLoad("Title");
	SelectInit();
	LoadLineShader();
}


//=============================================================================
//		データ読込
//=============================================================================
HRESULT CUIEditor::LoadData()
{
	return E_NOTIMPL;
}


//=============================================================================
//		初期化処理
//=============================================================================
void CUIEditor::Init()
{
}


//-----------------------------------------------------------------------------
//		UI選択時に仮変数等を初期化する
//-----------------------------------------------------------------------------
void CUIEditor::SelectInit()
{
	m_PatternNo		= POINTS(0, 0);
	m_PatternMax	= POINTS(1, 1);
	m_PatternAuto	= false;
}


//=============================================================================
//		更新処理
//=============================================================================
void CUIEditor::Update()
{
	if (!ISDEBUG) { return; }
	// キー入力
	KeyInput();

	// シーンを選択する
	ImGuiSelectScene();

	//--------------------------------------------------------------
	//		UIの追加と削除
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UIの追加・削除"));
	// シーンにUIを追加
	AddDeleteSprite();
	// 画像名を変更し名前被りに対処
	RenameUIObjects();
	ImGui::End();

	//--------------------------------------------------------------
	//		UI調整
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UIエディターウィンドウ"));
	// UIリストの検索関数
	ImGuiSearchUI();

	//-----------------------------------------------------------
	//		選択中のオブジェクトの編集
	//-----------------------------------------------------------
	if (m_SelectedUIIndex >= 0 && m_SelectedUIIndex < m_pUIs.size()) {
		// 選択されたUIを呼びやすくする
		UIObject* selectedUI = m_pUIs[m_SelectedUIIndex];
		// 選択されているUIを表示
		ImGui::Text(IMGUI_JP("選択されているUI: %s"), selectedUI->GetSpriteData().Name.c_str());

		// 選択されたUIをハイライトする
		ImGuiSetShader(selectedUI);

		// 座標の調整
		ImGuiPosEdit(selectedUI);
		// Z座標を基準にソート
		SortBySpritePosZ(selectedUI);

		// 画像情報の調整
		ImGuiInfoEdit(selectedUI);
		// 画像パターンを試す
		ImGuiPatternTest(selectedUI);
		// その他の情報の調整
		ImGuiEtcInfoEdit(selectedUI);
	}

	ImGui::End();

	//--------------------------------------------------------------
	//		保存する
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI保存ウィンドウ"));
	if (ImGui::Button(IMGUI_JP("UIを保存"))) {
		SaveScene(); 
		UIEDIOR_MOVEANY = false;
	}
	ImGui::End();
}


//-----------------------------------------------------------------------------
//		キー入力処理
//-----------------------------------------------------------------------------
void CUIEditor::KeyInput()
{
	m_DragValue = 1.f;
	if (GetAsyncKeyState(VK_SHIFT) && 0x8000) { m_DragValue *= 0.01f; }
	if (GetAsyncKeyState(VK_SPACE) && 0x8000) { m_DragValue *= 0.001f; }
}


//=============================================================================
//		描画処理
//=============================================================================
void CUIEditor::Draw()
{
	if (m_pUIs.empty()) { return; }
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
	UpdateShader();
}


//=============================================================================
//		解放処理
//=============================================================================
void CUIEditor::Release()
{
	SAFE_RELEASE(m_pCBufferMatrix);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pGeometryShader);
}


//-----------------------------------------------------------------------------------
//		シェーダ読込関数（一回のみ）
//-----------------------------------------------------------------------------------
HRESULT CUIEditor::LoadLineShader()
{
	auto directx11 = CDirectX11::GetInstance();
	ID3D11Device* device = directx11->GetDevice();

	// シェーダ読み込み
	ID3D10Blob* vsBlob = nullptr;
	ID3D10Blob* psBlob = nullptr;
	ID3D10Blob* gsBlob = nullptr;

	D3DX11CompileFromFile(ShaderPath, nullptr, nullptr, "VS_Main", "vs_5_0", 0, 0, nullptr, &vsBlob, nullptr, nullptr);
	D3DX11CompileFromFile(ShaderPath, nullptr, nullptr, "PS_Main", "ps_5_0", 0, 0, nullptr, &psBlob, nullptr, nullptr);
	D3DX11CompileFromFile(ShaderPath, nullptr, nullptr, "GS_Main", "gs_5_0", 0, 0, nullptr, &gsBlob, nullptr, nullptr);

	// シェーダ作成
	device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	device->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &m_pGeometryShader);

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_pInputLayout);

	// 頂点バッファ
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bd, nullptr, &m_pVertexBuffer);

	// 定数バッファ
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = sizeof(CBUFFER_MATRIX);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(device->CreateBuffer(&cbDesc, nullptr, &m_pCBufferMatrix)))
	{
		MessageBoxA(nullptr, "定数バッファ作成失敗", "Error", MB_OK);
		return E_FAIL;
	}


	return S_OK;
}


//-----------------------------------------------------------------------------------
//		シェーダ更新関数
//-----------------------------------------------------------------------------------
void CUIEditor::UpdateShader()
{
	auto directx11 = CDirectX11::GetInstance();
	ID3D11DeviceContext* context = directx11->GetContext();

	// 定数バッファ更新
	D3D11_VIEWPORT vp;
	UINT vpCount = 1;
	context->RSGetViewports(&vpCount, &vp);

	D3DXMATRIX mProj;
	D3DXMatrixOrthoLH(&mProj, vp.Width, vp.Height, 0.0f, 1.0f);

	D3D11_MAPPED_SUBRESOURCE mappedMatrix = {};
	if (SUCCEEDED(context->Map(m_pCBufferMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMatrix)))
	{
		CBUFFER_MATRIX cbMatrix = {};
		D3DXMatrixIdentity(&cbMatrix.mWorld);
		D3DXMatrixIdentity(&cbMatrix.mView);
		cbMatrix.mProj = mProj;
		cbMatrix.LineThickness = m_LineThickness;
		memcpy(mappedMatrix.pData, &cbMatrix, sizeof(cbMatrix));
		context->Unmap(m_pCBufferMatrix, 0);
	}

	context->VSSetConstantBuffers(0, 1, &m_pCBufferMatrix);
	context->GSSetConstantBuffers(0, 1, &m_pCBufferMatrix);
	context->PSSetConstantBuffers(0, 1, &m_pCBufferMatrix);

	// 頂点バッファ更新
	if (lineVertices) {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(context->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			memcpy(mappedResource.pData, lineVertices, sizeof(Vertex) * 8);
			context->Unmap(m_pVertexBuffer, 0);
		}
	}

	// パイプライン設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetInputLayout(m_pInputLayout);
	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	context->VSSetShader(m_pVertexShader, nullptr, 0);
	context->GSSetShader(m_pGeometryShader, nullptr, 0);
	context->PSSetShader(m_pPixelShader, nullptr, 0);

	// 描画
	context->Draw(8, 0);
}


//-----------------------------------------------------------------------------
//		選択したシーンのUIを読み込み
//-----------------------------------------------------------------------------
void CUIEditor::SelectSceneLoad(const std::string& sceneName)
{
	m_pSprite2Ds.clear();
	for (auto sprite : m_pSprite2Ds) SAFE_DELETE(sprite);
	for (auto ui : m_pUIs) SAFE_DELETE(ui);
	m_pUIs.clear();
	m_SpritePosList.clear();

	m_CurrentSceneName = sceneName;
	m_ScenePath = "Data\\Texture\\UIData\\" + sceneName + ".json";

	// JSON読み込み
	Json jsonData = FileManager::JsonLoad(m_ScenePath);

	// 空なら初期UIを1個追加
	if (jsonData.is_null() || jsonData.empty()) {
		auto sprite = new Sprite2D();
		auto ui = new UIObject();

		sprite->Init("Data\\Texture\\Other\\Black.png"); // 黒画像を初期で出す
		ui->AttachSprite(sprite);
		ui->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));

		m_pSprite2Ds.push_back(sprite);
		m_pUIs.push_back(ui);
		m_SpritePosList.push_back(ui->GetPos());

		// ついでにSaveScene()で書き込んでおく
		SaveScene();
	}

	// 保存されたUIデータを読み込み、展開
	for (auto& [imageName, spriteArray] : jsonData.items()) {
		// 拡張子が .json ならスキップ
		std::string::size_type dotPos = imageName.find_last_of('.');
		if (dotPos != std::string::npos) {
			std::string ext = imageName.substr(dotPos);
			if (ext == ".json" || ext == ".JSON") continue;
		}

		// スプライト取得
		Sprite2D* pSprite = CSpriteManager::GetInstance()->Get2DSprite(GetBaseName(imageName));
		if (!pSprite) {
			MessageBoxA(NULL, ("スプライトが見つかりません: " + imageName).c_str(), "Error", MB_OK);
			continue;
		}

		// 各UIインスタンスを展開
		for (auto& value : spriteArray) {
			auto ui = new UIObject();

			// 各種情報を設定
			pSprite->SetPos(D3DXVECTOR3(value["Pos"]["x"], value["Pos"]["y"], value["Pos"]["z"]));
			pSprite->SetColor(D3DXVECTOR3(value["Color"]["x"], value["Color"]["y"], value["Color"]["z"]));
			pSprite->SetAlpha(value["Alpha"]);
			pSprite->SetScale(D3DXVECTOR3(value["Scale"]["x"], value["Scale"]["y"], value["Scale"]["z"]));
			pSprite->SetPivot(D3DXVECTOR3(value["Pivot"]["x"], value["Pivot"]["y"], value["Pivot"]["z"]));
			pSprite->SetRot(D3DXVECTOR3(value["Rotate"]["x"], value["Rotate"]["y"], value["Rotate"]["z"]));

			// SpriteDataの一部も上書き
			pSprite->SetBase(D3DXVECTOR2(value["Base"]["w"], value["Base"]["h"]));
			pSprite->SetDisp(D3DXVECTOR2(value["Disp"]["w"], value["Disp"]["h"]));
			pSprite->SetStride(D3DXVECTOR2(value["Stride"]["w"], value["Stride"]["h"]));

			// リストに追加
			m_pSprite2Ds.push_back(pSprite);
			ui->AttachSprite(pSprite);
			m_pUIs.push_back(ui);
			m_SpritePosList.push_back(ui->GetPos());
		}
	}
	// Z座標を基準にソートする
	std::sort(m_pUIs.begin(), m_pUIs.end(), [](const UIObject* a, const UIObject* b) {
		return a->GetPos().z < b->GetPos().z;
		});
}


//-----------------------------------------------------------------------------
//		現在シーンのUI情報を保存
//-----------------------------------------------------------------------------
HRESULT CUIEditor::SaveScene()
{
	Json jsonData;
	for (size_t i = 0; i < m_pUIs.size(); ++i)
	{
		std::string imageName = m_pUIs[i]->GetSpriteData().Name;

		// 画像名ごとのリストにUI情報を追加
		Json SpriteState;
		SpriteState["Pos"]["x"] = m_pUIs[i]->GetPos().x;
		SpriteState["Pos"]["y"] = m_pUIs[i]->GetPos().y;
		SpriteState["Pos"]["z"] = m_pUIs[i]->GetPos().z;
		SpriteState["Base"]["w"] = m_pUIs[i]->GetSpriteData().Base.w;
		SpriteState["Base"]["h"] = m_pUIs[i]->GetSpriteData().Base.h;
		SpriteState["Disp"]["w"] = m_pUIs[i]->GetSpriteData().Disp.w;
		SpriteState["Disp"]["h"] = m_pUIs[i]->GetSpriteData().Disp.h;
		SpriteState["Stride"]["w"] = m_pUIs[i]->GetSpriteData().Stride.w;
		SpriteState["Stride"]["h"] = m_pUIs[i]->GetSpriteData().Stride.h;

		SpriteState["Color"]["x"] = m_pUIs[i]->GetColor().x;
		SpriteState["Color"]["y"] = m_pUIs[i]->GetColor().y;
		SpriteState["Color"]["z"] = m_pUIs[i]->GetColor().z;
		SpriteState["Alpha"]	 = m_pUIs[i]->GetAlpha();

		SpriteState["Scale"]["x"] = m_pUIs[i]->GetScale().x;
		SpriteState["Scale"]["y"] = m_pUIs[i]->GetScale().y;
		SpriteState["Scale"]["z"] = m_pUIs[i]->GetScale().z;
		SpriteState["Pivot"]["x"] = m_pUIs[i]->GetPivot().x;
		SpriteState["Pivot"]["y"] = m_pUIs[i]->GetPivot().y;
		SpriteState["Pivot"]["z"] = m_pUIs[i]->GetPivot().z;
		SpriteState["Rotate"]["x"] = m_pUIs[i]->GetRot().x;
		SpriteState["Rotate"]["y"] = m_pUIs[i]->GetRot().y;
		SpriteState["Rotate"]["z"] = m_pUIs[i]->GetRot().z;

		// jsonData[画像名] に配列として追加
		jsonData[imageName].push_back(SpriteState);
	}
	std::string outPath = "Data\\Texture\\UIData\\" + m_CurrentSceneName + ".json";
	if (!SUCCEEDED(FileManager::JsonSave(outPath, jsonData))) return E_FAIL;

	return S_OK;
}


//-----------------------------------------------------------------------------
//		Z座標を元にソートする関数
//-----------------------------------------------------------------------------
void CUIEditor::SortBySpritePosZ(UIObject* object)
{
	// 何も座標に関して変更がない場合早期リターン
	if (!m_MovedSpritePos) { return; }

	// ソート前の選択されていた UI のポインタを保存
	UIObject* pPreviousSelectedUI = object;

	// Z座標を基準にソートする
	std::sort(m_pUIs.begin(), m_pUIs.end(), [](const UIObject* a, const UIObject* b) {
		if (a && a->GetPos() && b && b->GetPos()) {
			return a->GetPos().z < b->GetPos().z;
		}
		return false;
		});

	// ソート後に、以前選択されていた UI がまだリストに存在するか確認し、再選択
	if (pPreviousSelectedUI != nullptr)
	{
		auto it = std::find(m_pUIs.begin(), m_pUIs.end(), pPreviousSelectedUI);
		if (it != m_pUIs.end()) {
			// 再選択
			m_SelectedUIIndex = static_cast<int>(std::distance(m_pUIs.begin(), it));
		}
		else {
			// 以前選択されていたUIがリストにない場合
			object = nullptr;
			m_SelectedUIIndex = 0; // または適切なデフォルト値
		}
	}

	m_MovedSpritePos = false;
}


//-----------------------------------------------------------------------------
//		ImGuiを用いたシーン選択関数
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiSelectScene()
{
	ImGui::Begin(IMGUI_JP("シーン管理"));

	// 新規シーンの作成
	ImGui::InputText(IMGUI_JP("新規シーン名"), m_NewSceneName, IM_ARRAYSIZE(m_NewSceneName));
	if (ImGui::Button(IMGUI_JP("新規シーン作成"))) {
		std::string newPath = "Data\\Texture\\UIData\\" + std::string(m_NewSceneName) + ".json";
		if (!std::filesystem::exists(newPath)) {
			std::ofstream ofs(newPath);
			ofs << "{}"; // 空のJSONを書き込む
			ofs.close();
		}
		m_CurrentSceneName = m_NewSceneName;
		SelectSceneLoad(m_CurrentSceneName);
		UIEDIOR_MOVEANY = false;
	}

	ImGui::Separator();
	ImGui::Text(IMGUI_JP("既存のシーン"));

	static std::string sceneToDelete; // 削除候補のシーン名
	static bool showDeleteConfirm = false; // 削除確認ダイアログ表示フラグ

	for (const auto& entry : std::filesystem::directory_iterator("Data\\Texture\\UIData\\")) {
		if (entry.path().extension() == ".json") {
			std::string sceneName = entry.path().stem().string();

			ImGui::PushID(sceneName.c_str());

			if (ImGui::Button(sceneName.c_str())) {
				m_CurrentSceneName = sceneName;
				SelectSceneLoad(m_CurrentSceneName);
				UIEDIOR_MOVEANY = false;
			}
			ImGui::SameLine();

			if (ImGui::Button(IMGUI_JP("削除"))) {
				sceneToDelete = sceneName;
				showDeleteConfirm = true;
			}

			ImGui::PopID();
		}
	}

	// 削除確認モーダル
	if (showDeleteConfirm) {
		ImGui::OpenPopup(IMGUI_JP("削除確認"));
	}
	if (ImGui::BeginPopupModal(IMGUI_JP("削除確認"), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("%s\n%s", sceneToDelete.c_str(), IMGUI_JP("を削除しますか？"));
		ImGui::Separator();

		if (ImGui::Button(IMGUI_JP("はい"), ImVec2(120, 0))) {
			std::string deletePath = "Data\\Texture\\UIData\\" + sceneToDelete + ".json";
			if (std::filesystem::exists(deletePath)) {
				try {
					std::filesystem::remove(deletePath);
				}
				catch (...) {}
			}

			if (m_CurrentSceneName == sceneToDelete) {
				// 削除対象のシーンを現在のシーンから外す
				m_CurrentSceneName.clear();

				// UIなどのデータをクリア
				m_pUIs.clear();
				m_pSprite2Ds.clear();
				m_SpritePosList.clear();
				UIEDIOR_MOVEANY = false;
			}

			sceneToDelete.clear();
			showDeleteConfirm = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button(IMGUI_JP("いいえ"), ImVec2(120, 0))) {
			sceneToDelete.clear();
			showDeleteConfirm = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}


//-----------------------------------------------------------------------------
//		UIリスト検索関数
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiSearchUI()
{
	if (!ISDEBUG) { return; }
	if (ImGui::TreeNodeEx(IMGUI_JP("UIリスト"), ImGuiTreeNodeFlags_DefaultOpen)) {
		// 検索バー
		ImGui::InputText(IMGUI_JP("検索"), m_SearchBuffer, IM_ARRAYSIZE(m_SearchBuffer));
		// スクロール可能なリスト
		ImGui::BeginChild(IMGUI_JP("リスト"), ImVec2(315, 100), true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < m_pUIs.size(); ++i) {
			// 検索フィルタリング
			if (strlen(m_SearchBuffer) > 0
				&& m_pUIs[i]->GetSpriteData().Name.find(m_SearchBuffer) == std::string::npos) {
				continue;
			}

			bool isSelected = (m_SelectedUIIndex == i);
			if (ImGui::Selectable(m_pUIs[i]->GetSpriteData().Name.c_str(), isSelected)) {
				m_SelectedUIIndex = i; // 選択更新
				SelectInit();
			}
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}
}


//-----------------------------------------------------------------------------
//		SpriteManagerからUIを追加
//-----------------------------------------------------------------------------
void CUIEditor::AddDeleteSprite()
{
	if (!ISDEBUG) { return; }
	std::vector<std::string> spriteNames = CSpriteManager::GetInstance()->GetSpriteNames();
	if (ImGui::TreeNodeEx(IMGUI_JP("追加可能UIリスト"), ImGuiTreeNodeFlags_DefaultOpen)) {
		// 検索バー
		ImGui::InputText(IMGUI_JP("検索"), m_SpriteSearchBuffer,
			IM_ARRAYSIZE(m_SpriteSearchBuffer));
		// スクロール可能なリスト
		ImGui::BeginChild(IMGUI_JP("リスト"),
			ImVec2(315, 100), true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < spriteNames.size(); ++i) {
			const std::string& name = spriteNames[i];

			// 検索フィルタ
			if (strlen(m_SpriteSearchBuffer) > 0 &&
				name.find(m_SpriteSearchBuffer) == std::string::npos) {
				continue;
			}

			bool isSelected = (m_SelectedSpriteName == name);
			if (ImGui::Selectable(name.c_str(), isSelected)) {
				m_SelectedSpriteName = name; // 選択更新
			}
		}
		ImGui::EndChild();

		// 選択されたスプライトをUIとして追加
		if (ImGui::Button(IMGUI_JP("UI追加"))) {
			if (!m_SelectedSpriteName.empty()) {
				Sprite2D* pSprite = CSpriteManager::GetInstance()->Get2DSprite(m_SelectedSpriteName);
				if (!pSprite) return;

				auto ui = new UIObject();
				ui->AttachSprite(pSprite);
				ui->SetPos(pSprite->GetPos());

				m_pSprite2Ds.push_back(pSprite);
				m_pUIs.push_back(ui);
				m_SpritePosList.push_back(ui->GetPos());
			}
		}

		// 選択されたUIを削除
		if (ImGui::Button(IMGUI_JP("UI削除")) && !m_pUIs.empty()) {
			SAFE_DELETE(m_pUIs[m_SelectedUIIndex]);

			m_pUIs.erase(m_pUIs.begin() + m_SelectedUIIndex);
			m_pSprite2Ds.erase(m_pSprite2Ds.begin() + m_SelectedUIIndex);
			m_SpritePosList.erase(m_SpritePosList.begin() + m_SelectedUIIndex);

			// インデックスをリセット
			m_SelectedUIIndex = 0;
		}

		ImGui::TreePop();
	}
}


//-----------------------------------------------------------------------------
//		ソートと名前変更を同時に行う
//-----------------------------------------------------------------------------
void CUIEditor::RenameUIObjects()
{
	if (m_pUIs.empty()) { return; }
	std::vector<std::pair<std::string, UIObject*>> nameUIList;

	// UI名とUIオブジェクトのペアを収集
	for (auto* ui : m_pUIs) {
		std::string baseName = GetBaseName(ui->GetSpriteData().Name);
		nameUIList.emplace_back(baseName, ui);
	}

	// ナンバリングして名前を再設定
	std::unordered_map<std::string, int> nameCount;

	for (auto& [baseName, ui] : nameUIList) {
		int index = nameCount[baseName]++;
		std::string newName = baseName + "_" + std::to_string(index);
		ui->SetSpriteName(newName);
	}
}


//-----------------------------------------------------------------------------
//		選択されたUIをハイライトする
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiSetShader(UIObject* object)
{
	if (!ISDEBUG) { return; }
	auto deviceContext = CDirectX11::GetInstance()->GetContext();

	// 角調整用の倍数
	int offsetcorner = 180;

	ImGui::Begin(IMGUI_JP("ハイライト線"));
	ImGui::DragFloat("Thickness", &m_LineThickness, 0.001f, 0.001f, 50.0f);
	ImGui::ColorEdit4("Line Color##", (float*)&m_LineColor);
	ImGui::End();

	float centerOffsetX = NOWFWND_W * 0.5f;
	float centerOffsetY = NOWFWND_H * 0.5f;
	// 基準となる頂点座標（中央）
	D3DXVECTOR3 basePos = {
		object->GetPos().x - centerOffsetX,
		(NOWFWND_H - object->GetPos().y) - centerOffsetY,  // Y座標を反転
		object->GetPos().z
	};
	// 上辺の右頂点座標
	D3DXVECTOR3 toplineright = {
		basePos.x + object->GetSpriteData().Disp.w,
		basePos.y,
		basePos.z
	};

	// 右辺の上下座標
	D3DXVECTOR3 rightlinetop = {
		basePos.x + object->GetSpriteData().Disp.w,
		basePos.y + m_LineThickness * offsetcorner,
		basePos.z
	};
	D3DXVECTOR3 rightlinebottom = {
		basePos.x + object->GetSpriteData().Disp.w,
		basePos.y - object->GetSpriteData().Disp.h - m_LineThickness * offsetcorner,
		basePos.z
	};

	// 下辺の左右座標
	D3DXVECTOR3 bottomlineright= {
		basePos.x + object->GetSpriteData().Disp.w,
		basePos.y - object->GetSpriteData().Disp.h,
		basePos.z
	};
	D3DXVECTOR3 bottomlineleft = {
		basePos.x,
		basePos.y - object->GetSpriteData().Disp.h,
		basePos.z
	};
	// 左辺の上下座標
	D3DXVECTOR3 leftlinebottom = {
		basePos.x,
		basePos.y - object->GetSpriteData().Disp.h - m_LineThickness * offsetcorner,
		basePos.z
	};
	D3DXVECTOR3 leftlinetop= {
		basePos.x,
		basePos.y + m_LineThickness * offsetcorner,
		basePos.z
	};

	// 上辺
	lineVertices[0] = { basePos,		m_LineColor };
	lineVertices[1] = { toplineright,	m_LineColor };
	// 右辺
	lineVertices[2] = { rightlinetop,	m_LineColor };
	lineVertices[3] = { rightlinebottom,m_LineColor };
	// 下辺
	lineVertices[4] = { bottomlineright,m_LineColor };
	lineVertices[5] = { bottomlineleft,	m_LineColor };
	// 左辺
	lineVertices[6] = { leftlinebottom,	m_LineColor };
	lineVertices[7] = { leftlinetop,	m_LineColor };


	// シェーダーのバッファに頂点情報をセットする
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(deviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		memcpy(mapped.pData, lineVertices, sizeof(Vertex) * 8); // 8頂点分
		deviceContext->Unmap(m_pVertexBuffer, 0);
	}

	// コンスタントバッファ更新（LineThickness用）
	D3D11_MAPPED_SUBRESOURCE mappedCB;
	if (SUCCEEDED(deviceContext->Map(m_pCBufferMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCB)))
	{
		CBUFFER_MATRIX* cbData = (CBUFFER_MATRIX*)mappedCB.pData;
		cbData->LineThickness = m_LineThickness;

		deviceContext->Unmap(m_pCBufferMatrix, 0);
	}
}


//-----------------------------------------------------------------------------
//		座標調整関数(選択されたUIObect)
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiPosEdit(UIObject* object)
{
	if (!ISDEBUG) { return; }
	if (ImGui::TreeNode(IMGUI_JP("座標"))) 
	{
		// ドラッグ用にマウス操作のDirectInpuを用意
		CMouse* Mouse = CInput::GetInstance()->CDMouse();
		D3DXVECTOR3 pos = object->GetPos();
		bool posdrag = ImGui::DragFloat3("##Position", pos, m_DragValue);

		// マウス位置を取得
		POINT MousePos;
		GetCursorPos(&MousePos);
		// 画像範囲内で左クリック入力中の場合、ドラッグ操作を開始
		if (object->PointInSquare(MousePos, CLIENTRECT) && !m_DoDrag) {
			if (Mouse->IsLAction()) {
				m_DoDrag = true;
				m_OffsetPos = D3DXVECTOR2(pos.x - MousePos.x, pos.y - MousePos.y);
			}
		}
		if (m_DoDrag) {
			posdrag = true;
			// 補正値+マウス座標した座標を入れる
			pos = D3DXVECTOR3(MousePos.x + m_OffsetPos.x, MousePos.y + m_OffsetPos.y, pos.z);
			// マウスの左クリックを話した場合、ドラッグ操作を停止
			if (!Mouse->IsLDown()) { m_DoDrag = false; }
		}

		// 変更があった場合保存する
		if (posdrag) {
			object->SetPos(pos);
			UIEDIOR_MOVEANY = true;
			m_MovedSpritePos = true;
		}
		ImGui::TreePop();
	}
}


//-----------------------------------------------------------------------------
//		情報調整関数(選択されたUIObect)
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiInfoEdit(UIObject* object)
{
	if (!ISDEBUG) { return; }
	if (ImGui::TreeNode(IMGUI_JP("画像情報")))
	{
		// 元、表示、分割それぞれのサイズを代入
		D3DXVECTOR2 base = D3DXVECTOR2(
			object->GetSpriteData().Base.w, 
			object->GetSpriteData().Base.h);
		D3DXVECTOR2 disp = D3DXVECTOR2(
			object->GetSpriteData().Disp.w, 
			object->GetSpriteData().Disp.h);
		D3DXVECTOR2 stride = D3DXVECTOR2(
			object->GetSpriteData().Stride.w, 
			object->GetSpriteData().Stride.h);

		ImGui::Text(IMGUI_JP("元のサイズ(x,y)"));
		bool basedrag	= ImGui::DragFloat2("##BaseDrag", base, m_DragValue);

		ImGui::Text(IMGUI_JP("表示サイズ(x,y)"));
		bool dispdrag	= ImGui::DragFloat2("##DispDrag", disp, m_DragValue);

		ImGui::Text(IMGUI_JP("分割サイズ(x,y)"));
		bool stridedrag	= ImGui::DragFloat2("##StrideDrag", stride, m_DragValue);

		// 変更があった場合保存する
		if (basedrag
		||	dispdrag
		||	stridedrag)
		{
			object->SetBase(base);
			object->SetDisp(disp);
			object->SetStride(stride);
			UIEDIOR_MOVEANY = true;
		}
		ImGui::TreePop();
	}
}


//-----------------------------------------------------------------------------
//		画像パターンお試し関数(選択されたUIObect)
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiPatternTest(UIObject* object)
{
	if (!ISDEBUG) { return; }
	if (ImGui::TreeNode(IMGUI_JP("画像パターンを試す"))) 
	{
		m_PatternNo = object->GetPatternNo();
		int pattern[2] = { m_PatternNo.x,m_PatternNo.y };
		int patternmax[2] = { m_PatternMax.x,m_PatternMax.y };

		// パターンの最大数を決める
		ImGui::Text(IMGUI_JP("パターンの上限"));
		ImGui::PushItemWidth(100.0f);
		ImGui::InputInt("##x", &patternmax[0]); ImGui::SameLine(); ImGui::InputInt("##y", &patternmax[1]);
		ImGui::PopItemWidth();

		// 下限は1固定
		if (patternmax[0] < 1) { patternmax[0] = 1; }
		if (patternmax[1] < 1) { patternmax[1] = 1; }
		m_PatternMax = POINTS(patternmax[0], patternmax[1]);

		// パターンのクリック調整
		if (ImGui::TreeNode(IMGUI_JP("クリック調整"))) {
			ImGui::PushItemWidth(100.0f);
			ImGui::InputInt("##xclickpattern", &pattern[0]); ImGui::SameLine(); ImGui::InputInt("##yclickpattern", &pattern[1]);
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}

		// パターンのオートラン調整
		if (ImGui::TreeNode(IMGUI_JP("オートラン調整"))) {
			// 実行中の処理
			if (m_PatternAuto) {
				ImGui::Text("On");
				m_AnimationSpeed -= CTime::GetInstance()->GetDeltaTime();
				if (m_AnimationSpeed < 0) {
					m_AnimationSpeed = m_AnimationSpeedMax * CTime::GetInstance()->GetDeltaTime();
					pattern[0]++;

					// xが最大値を超え、yが最大値の場合アニメーションが最初から送られるようにする
					if (m_PatternMax.x < pattern[0] && m_PatternMax.y == pattern[1]) {
						// yが0以下になった場合は初期状態にする
						pattern[0] = 0; pattern[1] = 0;
					}
				}
			}
			else {
				ImGui::Text("Off");
				m_AnimationSpeed = m_AnimationSpeedMax * CTime::GetInstance()->GetDeltaTime();
			}
			ImGui::PushItemWidth(100.0f);
			// 実行の切り替え
			if (ImGui::Button(IMGUI_JP("切替"))) { m_PatternAuto = !m_PatternAuto; }
			// 送り速度の設定
			ImGui::DragFloat(IMGUI_JP("送り速度設定(フレーム)"), &m_AnimationSpeedMax, m_DragValue);
			ImGui::PopItemWidth();

			ImGui::TreePop();
		}

		// Xが最大値を超えた場合
		if (m_PatternMax.x < pattern[0]) {
			// Yが最大値以上の場合、Xを最大値にする
			if (m_PatternMax.y <= pattern[1]) {
				pattern[0] = m_PatternMax.x;
			}
			else {
				pattern[0] = 0; pattern[1]++;
			}
		}
		else if (pattern[0] < 0) {
			// 最低値は0に固定し、yの値を繰り下げる
			pattern[0] = 0; pattern[1]--;
		}

		// Yが最大値を超えた場合
		if (m_PatternMax.y < pattern[1]) {
			pattern[0] = m_PatternMax.x;
			pattern[1] = m_PatternMax.y;
		}
		else if (pattern[1] < 0) {
			// yが0以下になった場合は初期状態にする
			pattern[0] = 0; pattern[1] = 0;
		}

		// 反映する
		m_PatternNo = POINTS(pattern[0], pattern[1]);
		object->SetPatternNo(m_PatternNo.x, m_PatternNo.y);
		ImGui::TreePop();
	}
}


//-----------------------------------------------------------------------------
//		その他の情報調整関数(選択されたUIObect)
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiEtcInfoEdit(UIObject* object)
{
	if (!ISDEBUG) { return; }
	if (ImGui::TreeNode(IMGUI_JP("その他")))
	{
		D3DXVECTOR4 color = D3DXVECTOR4(object->GetColor(), object->GetAlpha());
		D3DXVECTOR3 scale = object->GetScale();
		D3DXVECTOR3 rot = object->GetRot();
		D3DXVECTOR3 pivot = object->GetPivot();

		ImGui::Text(IMGUI_JP("カラー"));
		bool colorslider = ImGui::ColorEdit4("##Color", color);

		ImGui::Text(IMGUI_JP("スケール"));
		bool scaledrag = ImGui::DragFloat3("##ScaleDrag", scale, m_DragValue);

		ImGui::Text(IMGUI_JP("回転軸"));
		bool Pivotdrag = ImGui::DragFloat3("##PivotDrag", pivot, m_DragValue);
		ImGui::Text(IMGUI_JP("回転"));
		bool rotdrag = ImGui::DragFloat3("##RotDrag", rot, m_DragValue);

		// 変更があった場合保存する
		if (scaledrag
			|| Pivotdrag
			|| rotdrag
			|| colorslider)
		{
			object->SetColor(D3DXVECTOR3(color.x, color.y, color.z));
			object->SetAlpha(color.w);
			object->SetScale(scale);
			object->SetPivot(pivot);
			object->SetRot(rot);
			UIEDIOR_MOVEANY = true;
		}
		ImGui::TreePop();
	}
}
