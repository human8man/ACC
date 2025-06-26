#include "CUIEditor.h"
#include "DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
#include "FileManager/FileManager.h"
#include "FileManager/LoadImage/LoadImage.h"
#include "nlohmann/json.hpp"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "Time/CTime.h"
#include "ImGui/CImGui.h"


// json�^���쐬
using Json = nlohmann::json;

namespace {
	// �V�F�[�_�[�̃p�X
	const TCHAR ShaderPath[] = _T("Data\\Shader\\UIEditorLineShader.hlsl");

	struct Vertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR4 color;
	};

	Vertex lineVertices[] =
	{
		{ { -50.0f, 360.0f, 0.0f }, { 1, 0, 0, 1 } },  // ����
		{ {  50.0f, 360.0f, 0.0f }, { 1, 0, 0, 1 } },  // �E��

		{ {  50.0f, 360.0f, 0.0f }, { 1, 0, 0, 1 } },  // �E��
		{ {  50.0f, -360.0f, 0.0f }, { 1, 0, 0, 1 } }, // �E��

		{ {  50.0f, -360.0f, 0.0f }, { 1, 0, 0, 1 } }, // �E��
		{ { -50.0f, -360.0f, 0.0f }, { 1, 0, 0, 1 } }, // ����

		{ { -50.0f, -360.0f, 0.0f }, { 1, 0, 0, 1 } }, // ����
		{ { -50.0f, 360.0f, 0.0f }, { 1, 0, 0, 1 } },  // ����
	};

	struct CBUFFER_MATRIX {
		D3DXMATRIX mWorld;
		D3DXMATRIX mView;
		D3DXMATRIX mProj;
		float LineThickness; // �����i�s�N�Z���P�ʁj
		D3DXVECTOR3 padding; // �T�C�Y���� (16�o�C�g���E)
	};

	char m_NewSceneName[64] = ""; // �V�K�쐬�p�o�b�t�@�������o�ϐ��ɒǉ�
}


//============================================================================
//		UI�G�f�B�^�[�N���X
//============================================================================
CUIEditor::CUIEditor(HWND hWnd)
{
	m_hWnd = hWnd;
}
CUIEditor::~CUIEditor()
{
}


//=============================================================================
//		�쐬����
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
//		�f�[�^�Ǎ�
//=============================================================================
HRESULT CUIEditor::LoadData()
{
	return E_NOTIMPL;
}


//=============================================================================
//		����������
//=============================================================================
void CUIEditor::Init()
{
}


//-----------------------------------------------------------------------------
//		UI�I�����ɉ��ϐ���������������
//-----------------------------------------------------------------------------
void CUIEditor::SelectInit()
{
	m_PatternNo		= POINTS(0, 0);
	m_PatternMax	= POINTS(1, 1);
	m_PatternAuto	= false;
}


//=============================================================================
//		�X�V����
//=============================================================================
void CUIEditor::Update()
{
	if (!ISDEBUG) { return; }
	// �L�[����
	KeyInput();

	// �V�[����I������
	ImGuiSelectScene();

	//--------------------------------------------------------------
	//		UI�̒ǉ��ƍ폜
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI�̒ǉ��E�폜"));
	// �V�[����UI��ǉ�
	AddDeleteSprite();
	// �摜����ύX�����O���ɑΏ�
	RenameUIObjects();
	ImGui::End();

	//--------------------------------------------------------------
	//		UI����
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI�G�f�B�^�[�E�B���h�E"));
	// UI���X�g�̌����֐�
	ImGuiSearchUI();

	//-----------------------------------------------------------
	//		�I�𒆂̃I�u�W�F�N�g�̕ҏW
	//-----------------------------------------------------------
	if (m_SelectedUIIndex >= 0 && m_SelectedUIIndex < m_pUIs.size()) {
		// �I�����ꂽUI���Ăт₷������
		UIObject* selectedUI = m_pUIs[m_SelectedUIIndex];
		// �I������Ă���UI��\��
		ImGui::Text(IMGUI_JP("�I������Ă���UI: %s"), selectedUI->GetSpriteData().Name.c_str());

		// �I�����ꂽUI���n�C���C�g����
		ImGuiSetShader(selectedUI);

		// ���W�̒���
		ImGuiPosEdit(selectedUI);
		// Z���W����Ƀ\�[�g
		SortBySpritePosZ(selectedUI);

		// �摜���̒���
		ImGuiInfoEdit(selectedUI);
		// �摜�p�^�[��������
		ImGuiPatternTest(selectedUI);
		// ���̑��̏��̒���
		ImGuiEtcInfoEdit(selectedUI);
	}

	ImGui::End();

	//--------------------------------------------------------------
	//		�ۑ�����
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI�ۑ��E�B���h�E"));
	if (ImGui::Button(IMGUI_JP("UI��ۑ�"))) {
		SaveScene(); 
		UIEDIOR_MOVEANY = false;
	}
	ImGui::End();
}


//-----------------------------------------------------------------------------
//		�L�[���͏���
//-----------------------------------------------------------------------------
void CUIEditor::KeyInput()
{
	m_DragValue = 1.f;
	if (GetAsyncKeyState(VK_SHIFT) && 0x8000) { m_DragValue *= 0.01f; }
	if (GetAsyncKeyState(VK_SPACE) && 0x8000) { m_DragValue *= 0.001f; }
}


//=============================================================================
//		�`�揈��
//=============================================================================
void CUIEditor::Draw()
{
	if (m_pUIs.empty()) { return; }
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
	UpdateShader();
}


//=============================================================================
//		�������
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
//		�V�F�[�_�Ǎ��֐��i���̂݁j
//-----------------------------------------------------------------------------------
HRESULT CUIEditor::LoadLineShader()
{
	auto directx11 = CDirectX11::GetInstance();
	ID3D11Device* device = directx11->GetDevice();

	// �V�F�[�_�ǂݍ���
	ID3D10Blob* vsBlob = nullptr;
	ID3D10Blob* psBlob = nullptr;
	ID3D10Blob* gsBlob = nullptr;

	D3DX11CompileFromFile(ShaderPath, nullptr, nullptr, "VS_Main", "vs_5_0", 0, 0, nullptr, &vsBlob, nullptr, nullptr);
	D3DX11CompileFromFile(ShaderPath, nullptr, nullptr, "PS_Main", "ps_5_0", 0, 0, nullptr, &psBlob, nullptr, nullptr);
	D3DX11CompileFromFile(ShaderPath, nullptr, nullptr, "GS_Main", "gs_5_0", 0, 0, nullptr, &gsBlob, nullptr, nullptr);

	// �V�F�[�_�쐬
	device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	device->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &m_pGeometryShader);

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_pInputLayout);

	// ���_�o�b�t�@
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bd, nullptr, &m_pVertexBuffer);

	// �萔�o�b�t�@
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = sizeof(CBUFFER_MATRIX);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(device->CreateBuffer(&cbDesc, nullptr, &m_pCBufferMatrix)))
	{
		MessageBoxA(nullptr, "�萔�o�b�t�@�쐬���s", "Error", MB_OK);
		return E_FAIL;
	}


	return S_OK;
}


//-----------------------------------------------------------------------------------
//		�V�F�[�_�X�V�֐�
//-----------------------------------------------------------------------------------
void CUIEditor::UpdateShader()
{
	auto directx11 = CDirectX11::GetInstance();
	ID3D11DeviceContext* context = directx11->GetContext();

	// �萔�o�b�t�@�X�V
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

	// ���_�o�b�t�@�X�V
	if (lineVertices) {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(context->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			memcpy(mappedResource.pData, lineVertices, sizeof(Vertex) * 8);
			context->Unmap(m_pVertexBuffer, 0);
		}
	}

	// �p�C�v���C���ݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetInputLayout(m_pInputLayout);
	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	context->VSSetShader(m_pVertexShader, nullptr, 0);
	context->GSSetShader(m_pGeometryShader, nullptr, 0);
	context->PSSetShader(m_pPixelShader, nullptr, 0);

	// �`��
	context->Draw(8, 0);
}


//-----------------------------------------------------------------------------
//		�I�������V�[����UI��ǂݍ���
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

	// JSON�ǂݍ���
	Json jsonData = FileManager::JsonLoad(m_ScenePath);

	// ��Ȃ珉��UI��1�ǉ�
	if (jsonData.is_null() || jsonData.empty()) {
		auto sprite = new Sprite2D();
		auto ui = new UIObject();

		sprite->Init("Data\\Texture\\Other\\Black.png"); // ���摜�������ŏo��
		ui->AttachSprite(sprite);
		ui->SetPos(D3DXVECTOR3(0.f, 0.f, 0.f));

		m_pSprite2Ds.push_back(sprite);
		m_pUIs.push_back(ui);
		m_SpritePosList.push_back(ui->GetPos());

		// ���ł�SaveScene()�ŏ�������ł���
		SaveScene();
	}

	// �ۑ����ꂽUI�f�[�^��ǂݍ��݁A�W�J
	for (auto& [imageName, spriteArray] : jsonData.items()) {
		// �g���q�� .json �Ȃ�X�L�b�v
		std::string::size_type dotPos = imageName.find_last_of('.');
		if (dotPos != std::string::npos) {
			std::string ext = imageName.substr(dotPos);
			if (ext == ".json" || ext == ".JSON") continue;
		}

		// �X�v���C�g�擾
		Sprite2D* pSprite = CSpriteManager::GetInstance()->Get2DSprite(GetBaseName(imageName));
		if (!pSprite) {
			MessageBoxA(NULL, ("�X�v���C�g��������܂���: " + imageName).c_str(), "Error", MB_OK);
			continue;
		}

		// �eUI�C���X�^���X��W�J
		for (auto& value : spriteArray) {
			auto ui = new UIObject();

			// �e�����ݒ�
			pSprite->SetPos(D3DXVECTOR3(value["Pos"]["x"], value["Pos"]["y"], value["Pos"]["z"]));
			pSprite->SetColor(D3DXVECTOR3(value["Color"]["x"], value["Color"]["y"], value["Color"]["z"]));
			pSprite->SetAlpha(value["Alpha"]);
			pSprite->SetScale(D3DXVECTOR3(value["Scale"]["x"], value["Scale"]["y"], value["Scale"]["z"]));
			pSprite->SetPivot(D3DXVECTOR3(value["Pivot"]["x"], value["Pivot"]["y"], value["Pivot"]["z"]));
			pSprite->SetRot(D3DXVECTOR3(value["Rotate"]["x"], value["Rotate"]["y"], value["Rotate"]["z"]));

			// SpriteData�̈ꕔ���㏑��
			pSprite->SetBase(D3DXVECTOR2(value["Base"]["w"], value["Base"]["h"]));
			pSprite->SetDisp(D3DXVECTOR2(value["Disp"]["w"], value["Disp"]["h"]));
			pSprite->SetStride(D3DXVECTOR2(value["Stride"]["w"], value["Stride"]["h"]));

			// ���X�g�ɒǉ�
			m_pSprite2Ds.push_back(pSprite);
			ui->AttachSprite(pSprite);
			m_pUIs.push_back(ui);
			m_SpritePosList.push_back(ui->GetPos());
		}
	}
	// Z���W����Ƀ\�[�g����
	std::sort(m_pUIs.begin(), m_pUIs.end(), [](const UIObject* a, const UIObject* b) {
		return a->GetPos().z < b->GetPos().z;
		});
}


//-----------------------------------------------------------------------------
//		���݃V�[����UI����ۑ�
//-----------------------------------------------------------------------------
HRESULT CUIEditor::SaveScene()
{
	Json jsonData;
	for (size_t i = 0; i < m_pUIs.size(); ++i)
	{
		std::string imageName = m_pUIs[i]->GetSpriteData().Name;

		// �摜�����Ƃ̃��X�g��UI����ǉ�
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

		// jsonData[�摜��] �ɔz��Ƃ��Ēǉ�
		jsonData[imageName].push_back(SpriteState);
	}
	std::string outPath = "Data\\Texture\\UIData\\" + m_CurrentSceneName + ".json";
	if (!SUCCEEDED(FileManager::JsonSave(outPath, jsonData))) return E_FAIL;

	return S_OK;
}


//-----------------------------------------------------------------------------
//		Z���W�����Ƀ\�[�g����֐�
//-----------------------------------------------------------------------------
void CUIEditor::SortBySpritePosZ(UIObject* object)
{
	// �������W�Ɋւ��ĕύX���Ȃ��ꍇ�������^�[��
	if (!m_MovedSpritePos) { return; }

	// �\�[�g�O�̑I������Ă��� UI �̃|�C���^��ۑ�
	UIObject* pPreviousSelectedUI = object;

	// Z���W����Ƀ\�[�g����
	std::sort(m_pUIs.begin(), m_pUIs.end(), [](const UIObject* a, const UIObject* b) {
		if (a && a->GetPos() && b && b->GetPos()) {
			return a->GetPos().z < b->GetPos().z;
		}
		return false;
		});

	// �\�[�g��ɁA�ȑO�I������Ă��� UI ���܂����X�g�ɑ��݂��邩�m�F���A�đI��
	if (pPreviousSelectedUI != nullptr)
	{
		auto it = std::find(m_pUIs.begin(), m_pUIs.end(), pPreviousSelectedUI);
		if (it != m_pUIs.end()) {
			// �đI��
			m_SelectedUIIndex = static_cast<int>(std::distance(m_pUIs.begin(), it));
		}
		else {
			// �ȑO�I������Ă���UI�����X�g�ɂȂ��ꍇ
			object = nullptr;
			m_SelectedUIIndex = 0; // �܂��͓K�؂ȃf�t�H���g�l
		}
	}

	m_MovedSpritePos = false;
}


//-----------------------------------------------------------------------------
//		ImGui��p�����V�[���I���֐�
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiSelectScene()
{
	ImGui::Begin(IMGUI_JP("�V�[���Ǘ�"));

	// �V�K�V�[���̍쐬
	ImGui::InputText(IMGUI_JP("�V�K�V�[����"), m_NewSceneName, IM_ARRAYSIZE(m_NewSceneName));
	if (ImGui::Button(IMGUI_JP("�V�K�V�[���쐬"))) {
		std::string newPath = "Data\\Texture\\UIData\\" + std::string(m_NewSceneName) + ".json";
		if (!std::filesystem::exists(newPath)) {
			std::ofstream ofs(newPath);
			ofs << "{}"; // ���JSON����������
			ofs.close();
		}
		m_CurrentSceneName = m_NewSceneName;
		SelectSceneLoad(m_CurrentSceneName);
		UIEDIOR_MOVEANY = false;
	}

	ImGui::Separator();
	ImGui::Text(IMGUI_JP("�����̃V�[��"));

	static std::string sceneToDelete; // �폜���̃V�[����
	static bool showDeleteConfirm = false; // �폜�m�F�_�C�A���O�\���t���O

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

			if (ImGui::Button(IMGUI_JP("�폜"))) {
				sceneToDelete = sceneName;
				showDeleteConfirm = true;
			}

			ImGui::PopID();
		}
	}

	// �폜�m�F���[�_��
	if (showDeleteConfirm) {
		ImGui::OpenPopup(IMGUI_JP("�폜�m�F"));
	}
	if (ImGui::BeginPopupModal(IMGUI_JP("�폜�m�F"), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("%s\n%s", sceneToDelete.c_str(), IMGUI_JP("���폜���܂����H"));
		ImGui::Separator();

		if (ImGui::Button(IMGUI_JP("�͂�"), ImVec2(120, 0))) {
			std::string deletePath = "Data\\Texture\\UIData\\" + sceneToDelete + ".json";
			if (std::filesystem::exists(deletePath)) {
				try {
					std::filesystem::remove(deletePath);
				}
				catch (...) {}
			}

			if (m_CurrentSceneName == sceneToDelete) {
				// �폜�Ώۂ̃V�[�������݂̃V�[������O��
				m_CurrentSceneName.clear();

				// UI�Ȃǂ̃f�[�^���N���A
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
		if (ImGui::Button(IMGUI_JP("������"), ImVec2(120, 0))) {
			sceneToDelete.clear();
			showDeleteConfirm = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}


//-----------------------------------------------------------------------------
//		UI���X�g�����֐�
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiSearchUI()
{
	if (!ISDEBUG) { return; }
	if (ImGui::TreeNodeEx(IMGUI_JP("UI���X�g"), ImGuiTreeNodeFlags_DefaultOpen)) {
		// �����o�[
		ImGui::InputText(IMGUI_JP("����"), m_SearchBuffer, IM_ARRAYSIZE(m_SearchBuffer));
		// �X�N���[���\�ȃ��X�g
		ImGui::BeginChild(IMGUI_JP("���X�g"), ImVec2(315, 100), true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < m_pUIs.size(); ++i) {
			// �����t�B���^�����O
			if (strlen(m_SearchBuffer) > 0
				&& m_pUIs[i]->GetSpriteData().Name.find(m_SearchBuffer) == std::string::npos) {
				continue;
			}

			bool isSelected = (m_SelectedUIIndex == i);
			if (ImGui::Selectable(m_pUIs[i]->GetSpriteData().Name.c_str(), isSelected)) {
				m_SelectedUIIndex = i; // �I���X�V
				SelectInit();
			}
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}
}


//-----------------------------------------------------------------------------
//		SpriteManager����UI��ǉ�
//-----------------------------------------------------------------------------
void CUIEditor::AddDeleteSprite()
{
	if (!ISDEBUG) { return; }
	std::vector<std::string> spriteNames = CSpriteManager::GetInstance()->GetSpriteNames();
	if (ImGui::TreeNodeEx(IMGUI_JP("�ǉ��\UI���X�g"), ImGuiTreeNodeFlags_DefaultOpen)) {
		// �����o�[
		ImGui::InputText(IMGUI_JP("����"), m_SpriteSearchBuffer,
			IM_ARRAYSIZE(m_SpriteSearchBuffer));
		// �X�N���[���\�ȃ��X�g
		ImGui::BeginChild(IMGUI_JP("���X�g"),
			ImVec2(315, 100), true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < spriteNames.size(); ++i) {
			const std::string& name = spriteNames[i];

			// �����t�B���^
			if (strlen(m_SpriteSearchBuffer) > 0 &&
				name.find(m_SpriteSearchBuffer) == std::string::npos) {
				continue;
			}

			bool isSelected = (m_SelectedSpriteName == name);
			if (ImGui::Selectable(name.c_str(), isSelected)) {
				m_SelectedSpriteName = name; // �I���X�V
			}
		}
		ImGui::EndChild();

		// �I�����ꂽ�X�v���C�g��UI�Ƃ��Ēǉ�
		if (ImGui::Button(IMGUI_JP("UI�ǉ�"))) {
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

		// �I�����ꂽUI���폜
		if (ImGui::Button(IMGUI_JP("UI�폜")) && !m_pUIs.empty()) {
			SAFE_DELETE(m_pUIs[m_SelectedUIIndex]);

			m_pUIs.erase(m_pUIs.begin() + m_SelectedUIIndex);
			m_pSprite2Ds.erase(m_pSprite2Ds.begin() + m_SelectedUIIndex);
			m_SpritePosList.erase(m_SpritePosList.begin() + m_SelectedUIIndex);

			// �C���f�b�N�X�����Z�b�g
			m_SelectedUIIndex = 0;
		}

		ImGui::TreePop();
	}
}


//-----------------------------------------------------------------------------
//		�\�[�g�Ɩ��O�ύX�𓯎��ɍs��
//-----------------------------------------------------------------------------
void CUIEditor::RenameUIObjects()
{
	if (m_pUIs.empty()) { return; }
	std::vector<std::pair<std::string, UIObject*>> nameUIList;

	// UI����UI�I�u�W�F�N�g�̃y�A�����W
	for (auto* ui : m_pUIs) {
		std::string baseName = GetBaseName(ui->GetSpriteData().Name);
		nameUIList.emplace_back(baseName, ui);
	}

	// �i���o�����O���Ė��O���Đݒ�
	std::unordered_map<std::string, int> nameCount;

	for (auto& [baseName, ui] : nameUIList) {
		int index = nameCount[baseName]++;
		std::string newName = baseName + "_" + std::to_string(index);
		ui->SetSpriteName(newName);
	}
}


//-----------------------------------------------------------------------------
//		�I�����ꂽUI���n�C���C�g����
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiSetShader(UIObject* object)
{
	if (!ISDEBUG) { return; }
	auto deviceContext = CDirectX11::GetInstance()->GetContext();

	// �p�����p�̔{��
	int offsetcorner = 180;

	ImGui::Begin(IMGUI_JP("�n�C���C�g��"));
	ImGui::DragFloat("Thickness", &m_LineThickness, 0.001f, 0.001f, 50.0f);
	ImGui::ColorEdit4("Line Color##", (float*)&m_LineColor);
	ImGui::End();

	float centerOffsetX = NOWFWND_W * 0.5f;
	float centerOffsetY = NOWFWND_H * 0.5f;
	// ��ƂȂ钸�_���W�i�����j
	D3DXVECTOR3 basePos = {
		object->GetPos().x - centerOffsetX,
		(NOWFWND_H - object->GetPos().y) - centerOffsetY,  // Y���W�𔽓]
		object->GetPos().z
	};
	// ��ӂ̉E���_���W
	D3DXVECTOR3 toplineright = {
		basePos.x + object->GetSpriteData().Disp.w,
		basePos.y,
		basePos.z
	};

	// �E�ӂ̏㉺���W
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

	// ���ӂ̍��E���W
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
	// ���ӂ̏㉺���W
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

	// ���
	lineVertices[0] = { basePos,		m_LineColor };
	lineVertices[1] = { toplineright,	m_LineColor };
	// �E��
	lineVertices[2] = { rightlinetop,	m_LineColor };
	lineVertices[3] = { rightlinebottom,m_LineColor };
	// ����
	lineVertices[4] = { bottomlineright,m_LineColor };
	lineVertices[5] = { bottomlineleft,	m_LineColor };
	// ����
	lineVertices[6] = { leftlinebottom,	m_LineColor };
	lineVertices[7] = { leftlinetop,	m_LineColor };


	// �V�F�[�_�[�̃o�b�t�@�ɒ��_�����Z�b�g����
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(deviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		memcpy(mapped.pData, lineVertices, sizeof(Vertex) * 8); // 8���_��
		deviceContext->Unmap(m_pVertexBuffer, 0);
	}

	// �R���X�^���g�o�b�t�@�X�V�iLineThickness�p�j
	D3D11_MAPPED_SUBRESOURCE mappedCB;
	if (SUCCEEDED(deviceContext->Map(m_pCBufferMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCB)))
	{
		CBUFFER_MATRIX* cbData = (CBUFFER_MATRIX*)mappedCB.pData;
		cbData->LineThickness = m_LineThickness;

		deviceContext->Unmap(m_pCBufferMatrix, 0);
	}
}


//-----------------------------------------------------------------------------
//		���W�����֐�(�I�����ꂽUIObect)
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiPosEdit(UIObject* object)
{
	if (!ISDEBUG) { return; }
	if (ImGui::TreeNode(IMGUI_JP("���W"))) 
	{
		// �h���b�O�p�Ƀ}�E�X�����DirectInpu��p��
		CMouse* Mouse = CInput::GetInstance()->CDMouse();
		D3DXVECTOR3 pos = object->GetPos();
		bool posdrag = ImGui::DragFloat3("##Position", pos, m_DragValue);

		// �}�E�X�ʒu���擾
		POINT MousePos;
		GetCursorPos(&MousePos);
		// �摜�͈͓��ō��N���b�N���͒��̏ꍇ�A�h���b�O������J�n
		if (object->PointInSquare(MousePos, CLIENTRECT) && !m_DoDrag) {
			if (Mouse->IsLAction()) {
				m_DoDrag = true;
				m_OffsetPos = D3DXVECTOR2(pos.x - MousePos.x, pos.y - MousePos.y);
			}
		}
		if (m_DoDrag) {
			posdrag = true;
			// �␳�l+�}�E�X���W�������W������
			pos = D3DXVECTOR3(MousePos.x + m_OffsetPos.x, MousePos.y + m_OffsetPos.y, pos.z);
			// �}�E�X�̍��N���b�N��b�����ꍇ�A�h���b�O������~
			if (!Mouse->IsLDown()) { m_DoDrag = false; }
		}

		// �ύX���������ꍇ�ۑ�����
		if (posdrag) {
			object->SetPos(pos);
			UIEDIOR_MOVEANY = true;
			m_MovedSpritePos = true;
		}
		ImGui::TreePop();
	}
}


//-----------------------------------------------------------------------------
//		��񒲐��֐�(�I�����ꂽUIObect)
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiInfoEdit(UIObject* object)
{
	if (!ISDEBUG) { return; }
	if (ImGui::TreeNode(IMGUI_JP("�摜���")))
	{
		// ���A�\���A�������ꂼ��̃T�C�Y����
		D3DXVECTOR2 base = D3DXVECTOR2(
			object->GetSpriteData().Base.w, 
			object->GetSpriteData().Base.h);
		D3DXVECTOR2 disp = D3DXVECTOR2(
			object->GetSpriteData().Disp.w, 
			object->GetSpriteData().Disp.h);
		D3DXVECTOR2 stride = D3DXVECTOR2(
			object->GetSpriteData().Stride.w, 
			object->GetSpriteData().Stride.h);

		ImGui::Text(IMGUI_JP("���̃T�C�Y(x,y)"));
		bool basedrag	= ImGui::DragFloat2("##BaseDrag", base, m_DragValue);

		ImGui::Text(IMGUI_JP("�\���T�C�Y(x,y)"));
		bool dispdrag	= ImGui::DragFloat2("##DispDrag", disp, m_DragValue);

		ImGui::Text(IMGUI_JP("�����T�C�Y(x,y)"));
		bool stridedrag	= ImGui::DragFloat2("##StrideDrag", stride, m_DragValue);

		// �ύX���������ꍇ�ۑ�����
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
//		�摜�p�^�[���������֐�(�I�����ꂽUIObect)
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiPatternTest(UIObject* object)
{
	if (!ISDEBUG) { return; }
	if (ImGui::TreeNode(IMGUI_JP("�摜�p�^�[��������"))) 
	{
		m_PatternNo = object->GetPatternNo();
		int pattern[2] = { m_PatternNo.x,m_PatternNo.y };
		int patternmax[2] = { m_PatternMax.x,m_PatternMax.y };

		// �p�^�[���̍ő吔�����߂�
		ImGui::Text(IMGUI_JP("�p�^�[���̏��"));
		ImGui::PushItemWidth(100.0f);
		ImGui::InputInt("##x", &patternmax[0]); ImGui::SameLine(); ImGui::InputInt("##y", &patternmax[1]);
		ImGui::PopItemWidth();

		// ������1�Œ�
		if (patternmax[0] < 1) { patternmax[0] = 1; }
		if (patternmax[1] < 1) { patternmax[1] = 1; }
		m_PatternMax = POINTS(patternmax[0], patternmax[1]);

		// �p�^�[���̃N���b�N����
		if (ImGui::TreeNode(IMGUI_JP("�N���b�N����"))) {
			ImGui::PushItemWidth(100.0f);
			ImGui::InputInt("##xclickpattern", &pattern[0]); ImGui::SameLine(); ImGui::InputInt("##yclickpattern", &pattern[1]);
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}

		// �p�^�[���̃I�[�g��������
		if (ImGui::TreeNode(IMGUI_JP("�I�[�g��������"))) {
			// ���s���̏���
			if (m_PatternAuto) {
				ImGui::Text("On");
				m_AnimationSpeed -= CTime::GetInstance()->GetDeltaTime();
				if (m_AnimationSpeed < 0) {
					m_AnimationSpeed = m_AnimationSpeedMax * CTime::GetInstance()->GetDeltaTime();
					pattern[0]++;

					// x���ő�l�𒴂��Ay���ő�l�̏ꍇ�A�j���[�V�������ŏ����瑗����悤�ɂ���
					if (m_PatternMax.x < pattern[0] && m_PatternMax.y == pattern[1]) {
						// y��0�ȉ��ɂȂ����ꍇ�͏�����Ԃɂ���
						pattern[0] = 0; pattern[1] = 0;
					}
				}
			}
			else {
				ImGui::Text("Off");
				m_AnimationSpeed = m_AnimationSpeedMax * CTime::GetInstance()->GetDeltaTime();
			}
			ImGui::PushItemWidth(100.0f);
			// ���s�̐؂�ւ�
			if (ImGui::Button(IMGUI_JP("�ؑ�"))) { m_PatternAuto = !m_PatternAuto; }
			// ���葬�x�̐ݒ�
			ImGui::DragFloat(IMGUI_JP("���葬�x�ݒ�(�t���[��)"), &m_AnimationSpeedMax, m_DragValue);
			ImGui::PopItemWidth();

			ImGui::TreePop();
		}

		// X���ő�l�𒴂����ꍇ
		if (m_PatternMax.x < pattern[0]) {
			// Y���ő�l�ȏ�̏ꍇ�AX���ő�l�ɂ���
			if (m_PatternMax.y <= pattern[1]) {
				pattern[0] = m_PatternMax.x;
			}
			else {
				pattern[0] = 0; pattern[1]++;
			}
		}
		else if (pattern[0] < 0) {
			// �Œ�l��0�ɌŒ肵�Ay�̒l���J�艺����
			pattern[0] = 0; pattern[1]--;
		}

		// Y���ő�l�𒴂����ꍇ
		if (m_PatternMax.y < pattern[1]) {
			pattern[0] = m_PatternMax.x;
			pattern[1] = m_PatternMax.y;
		}
		else if (pattern[1] < 0) {
			// y��0�ȉ��ɂȂ����ꍇ�͏�����Ԃɂ���
			pattern[0] = 0; pattern[1] = 0;
		}

		// ���f����
		m_PatternNo = POINTS(pattern[0], pattern[1]);
		object->SetPatternNo(m_PatternNo.x, m_PatternNo.y);
		ImGui::TreePop();
	}
}


//-----------------------------------------------------------------------------
//		���̑��̏�񒲐��֐�(�I�����ꂽUIObect)
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiEtcInfoEdit(UIObject* object)
{
	if (!ISDEBUG) { return; }
	if (ImGui::TreeNode(IMGUI_JP("���̑�")))
	{
		D3DXVECTOR4 color = D3DXVECTOR4(object->GetColor(), object->GetAlpha());
		D3DXVECTOR3 scale = object->GetScale();
		D3DXVECTOR3 rot = object->GetRot();
		D3DXVECTOR3 pivot = object->GetPivot();

		ImGui::Text(IMGUI_JP("�J���["));
		bool colorslider = ImGui::ColorEdit4("##Color", color);

		ImGui::Text(IMGUI_JP("�X�P�[��"));
		bool scaledrag = ImGui::DragFloat3("##ScaleDrag", scale, m_DragValue);

		ImGui::Text(IMGUI_JP("��]��"));
		bool Pivotdrag = ImGui::DragFloat3("##PivotDrag", pivot, m_DragValue);
		ImGui::Text(IMGUI_JP("��]"));
		bool rotdrag = ImGui::DragFloat3("##RotDrag", rot, m_DragValue);

		// �ύX���������ꍇ�ۑ�����
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
