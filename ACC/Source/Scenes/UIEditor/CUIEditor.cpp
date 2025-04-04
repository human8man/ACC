#include "CUIEditor.h"
#include "nlohmann/json.hpp"
#include "FileManager/FileManager.h"
#include "FileManager/LoadImage/LoadImage.h"


#ifdef _DEBUG
#include "ImGui/CImGui.h"
#endif


// json�^���쐬.
using Json = nlohmann::json;

namespace {
	// TitleUI�̃p�X.
	constexpr char TitleImagePath[] = "Data\\Texture\\Title";
	// GameUI�̃p�X.
	constexpr char GameImagePath[] = "Data\\Texture\\Game";
	// WinUI�̃p�X.
	constexpr char WinImagePath[] = "Data\\Texture\\Win";
	// LoseUI�̃p�X.
	constexpr char LoseImagePath[] = "Data\\Texture\\Lose";
}


//============================================================================
//		UI�G�f�B�^�[�N���X.
//============================================================================
CUIEditor::CUIEditor(HWND hWnd)
{
}
CUIEditor::~CUIEditor()
{
}


//=============================================================================
//		�쐬����.
//=============================================================================
void CUIEditor::Create()
{
	SelectSceneLoad(UISceneList::Title);
}


//=============================================================================
//		�f�[�^�Ǎ�.
//=============================================================================
HRESULT CUIEditor::LoadData()
{
	return E_NOTIMPL;
}


//=============================================================================
//		����������.
//=============================================================================
void CUIEditor::Init()
{
}


//=============================================================================
//		�X�V����.
//=============================================================================
void CUIEditor::Update()
{
#ifdef _DEBUG
	//--------------------------------------------------------------
	//		�V�[����I������.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UIEditor�p�V�[���I��"));
	UISceneList scene;
	bool click = false;
	if (ImGui::Button("Title"))	{ scene = UISceneList::Title;	click = true; }
	if (ImGui::Button("Game"))	{ scene = UISceneList::Game;	click = true; }
	if (ImGui::Button("Lose"))	{ scene = UISceneList::Lose;	click = true; }
	if (ImGui::Button("Win"))	{ scene = UISceneList::Win;		click = true; }
	if (m_MovedSomething && click && (MessageBox(NULL,
		L"�{���ɐ؂�ւ��܂����H\n(�ۑ����Ă��Ȃ��ꍇ���Z�b�g����܂�)",  L"�m�F",
		MB_YESNO | MB_ICONQUESTION)) == IDYES) {
		SelectSceneLoad(scene);
		m_MovedSomething = false;
	}
	else if(!m_MovedSomething && click){
		SelectSceneLoad(scene);
	}

	ImGui::End();


	//--------------------------------------------------------------
	//		UI��������.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI�����E�B���h�E"));

	if (ImGui::TreeNodeEx(IMGUI_JP("UI���X�g"), ImGuiTreeNodeFlags_DefaultOpen)) {
		// �����o�[.
		ImGui::InputText(IMGUI_JP("����"), m_SearchBuffer, IM_ARRAYSIZE(m_SearchBuffer));
		// �X�N���[���\�ȃ��X�g.
		ImGui::BeginChild(IMGUI_JP("���X�g"), ImVec2(315, 100), true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < m_pUIs.size(); ++i) {
			// �����t�B���^�����O.
			if (strlen(m_SearchBuffer) > 0 
			&& m_pUIs[i]->GetSpriteData().Name.find(m_SearchBuffer) == std::string::npos) {
				continue;
			}

			bool isSelected = (m_SelectedUIIndex == i);
			if (ImGui::Selectable(m_pUIs[i]->GetSpriteData().Name.c_str(), isSelected)) {
				m_SelectedUIIndex = i; // �I���X�V.
			}
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}

	// �I�𒆂̃I�u�W�F�N�g��ҏW.
	if (m_SelectedUIIndex >= 0 && m_SelectedUIIndex < m_pUIs.size()) {
		// �I�����ꂽUI���Ăт₷������.
		CUIObject* selectedUI = m_pUIs[m_SelectedUIIndex];
		// �I������Ă���UI��\��.
		ImGui::Text(IMGUI_JP("�I������Ă���UI: %s"), selectedUI->GetSpriteData().Name.c_str());

		// ���W�̒���.
		if (ImGui::TreeNode(IMGUI_JP("���W"))) {
			D3DXVECTOR3 pos	= selectedUI->GetPos();
			bool posdrag	= ImGui::DragFloat3("##Position", pos, 1.f);
			bool posinput	= ImGui::InputFloat3("##InputPos", pos);

			// �ύX���������ꍇ�ۑ�����.
			if (posdrag || posinput) {
				selectedUI->SetPos(pos); 
				m_MovedSomething = true;
			}
			ImGui::TreePop();
		}

		// �摜���̒���.
		if (ImGui::TreeNode(IMGUI_JP("�摜���"))) {
			D3DXVECTOR2 base	= D3DXVECTOR2(selectedUI->GetSpriteData().Base.w, selectedUI->GetSpriteData().Base.h);
			D3DXVECTOR2 disp	= D3DXVECTOR2(selectedUI->GetSpriteData().Disp.w, selectedUI->GetSpriteData().Disp.h);
			D3DXVECTOR2 stride	= D3DXVECTOR2(selectedUI->GetSpriteData().Stride.w, selectedUI->GetSpriteData().Stride.h);

			ImGui::Text(IMGUI_JP("���̃T�C�Y(x,y)"));
			bool basedrag	= ImGui::DragFloat2("##BaseDrag", base, 1.f);
			bool baseinput	= ImGui::InputFloat2("##BaseInput", base);

			ImGui::Text(IMGUI_JP("�\���T�C�Y(x,y)"));
			bool dispdrag	= ImGui::DragFloat2("##DispDrag", disp, 1.f);
			bool dispinput	= ImGui::InputFloat2("##DispInput", disp);

			ImGui::Text(IMGUI_JP("�����T�C�Y(x,y)"));
			bool stridedrag		= ImGui::DragFloat2("##StrideDrag", stride, 1.f);
			bool strideinput	= ImGui::InputFloat2("##StrideInput", stride);

			// �ύX���������ꍇ�ۑ�����.
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

		// ���̑��̏��̒���.
		if (ImGui::TreeNode(IMGUI_JP("���̑�"))) {
			float alpha = selectedUI->GetAlpha();
			D3DXVECTOR3 scale	= selectedUI->GetScale();
			D3DXVECTOR3 rot		= selectedUI->GetRot();

			ImGui::Text(IMGUI_JP("�A���t�@"));
			bool alphaslider	= ImGui::SliderFloat("##AlphaSlider", &alpha, 0.f, 1.f);
			bool alphainput		= ImGui::InputFloat("##AlphaInput", &alpha);

			ImGui::Text(IMGUI_JP("�X�P�[��"));
			bool scaledrag	= ImGui::DragFloat3("##ScaleDrag", scale, 0.1f);
			bool scaleinput	= ImGui::InputFloat3("##ScaleInput", scale);

			ImGui::Text(IMGUI_JP("��]"));
			bool rotdrag	= ImGui::DragFloat3("##RotDrag", rot, 0.1f);
			bool rotinput	= ImGui::InputFloat3("##RotInput", rot);

			// �ύX���������ꍇ�ۑ�����.
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
	//		�ۑ�����.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI�ۑ��E�B���h�E"));
	if (ImGui::Button(IMGUI_JP("UI��ۑ�"))) {
		SaveScene(); 
		m_MovedSomething = false;
	}
	ImGui::End();
#endif
}


//=============================================================================
//		�`�揈��.
//=============================================================================
void CUIEditor::Draw()
{
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
}


//=============================================================================
//		�������.
//=============================================================================
void CUIEditor::Release()
{
}


//-----------------------------------------------------------------------------
//		�I�������V�[����UI��ǂݍ���.
//-----------------------------------------------------------------------------
void CUIEditor::SelectSceneLoad(UISceneList scene)
{
	// ���݂�UI���N���A.
	for (auto sprite : m_pSprite2Ds) { SAFE_DELETE(sprite); }
	m_pSprite2Ds.clear(); 
	for (auto ui : m_pUIs) { SAFE_DELETE(ui); }
	m_pUIs.clear(); 
	m_SpritePosList.clear();

	// �V�[�����Ƃ̃p�X��ݒ�.
	switch (scene) {
	case Title:	m_ScenePath = TitleImagePath;	break;
	case Game:	m_ScenePath = GameImagePath;	break;
	case Win:	m_ScenePath = WinImagePath;		break;
	case Lose:	m_ScenePath = LoseImagePath;	break;
	default: return;
	}

	// �w�肵���p�X�𑖍�.
	for (const auto& entry : std::filesystem::directory_iterator(m_ScenePath)) {
		std::string extension = entry.path().extension().string();
		if (extension == ".json") continue;

		// �C���X�^���X����.
		auto sprite = new CSprite2D();
		auto ui = new CUIObject();

		// �摜�f�[�^�̓ǂݍ���.
		sprite->Init(entry.path().string());
		ui->AttachSprite(*sprite);
		ui->SetPos(sprite->GetSpriteData().Pos);

		// ���X�g�ɒǉ�.
		m_pSprite2Ds.push_back(sprite);
		m_pUIs.push_back(ui);
		m_SpritePosList.push_back(ui->GetPos());
	}
}


//-----------------------------------------------------------------------------
//		���݃V�[����UI����ۑ�.
//-----------------------------------------------------------------------------
HRESULT CUIEditor::SaveScene()
{
	for (size_t i = 0; i < m_pUIs.size(); ++i) 
	{
		// �쐬����t�@�C���p�X.
		std::string TextPath = m_pUIs[i]->GetSpriteData().Path;

		// ����ǉ����Ă���.
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

		// �X�v���C�g���̍쐬.
		if (FAILED(FileManager::JsonSave(TextPath, SpriteState))) return E_FAIL;
	}
	return S_OK;
}