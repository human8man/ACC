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
	//--------------------------------------------------------------
	//		�V�[����I������.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("�V�[���I���E�B���h�E"));
	UISceneList scene;
	bool click = false;
	if (ImGui::Button("Title"))	{ scene = UISceneList::Title;	click = true; }
	if (ImGui::Button("Game"))	{ scene = UISceneList::Game;	click = true; }
	if (ImGui::Button("Lose"))	{ scene = UISceneList::Lose;	click = true; }
	if (ImGui::Button("Win"))	{ scene = UISceneList::Win;		click = true; }
	if (click && (MessageBox( NULL,
		L"�{���ɐ؂�ւ��܂����H\n(�ۑ����Ă��Ȃ��ꍇ���Z�b�g����܂�)",  L"�m�F",
		MB_YESNO | MB_ICONQUESTION)) == IDYES) {
		SelectSceneLoad(scene);
	}
	ImGui::End();


	//--------------------------------------------------------------
	//		UI��������.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI�����E�B���h�E"));

	ImGui::End();


	//--------------------------------------------------------------
	//		�ۑ�����.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI�ۑ��E�B���h�E"));
	if (ImGui::Button(IMGUI_JP("UI��ۑ�"))) { SaveScene(); }
	ImGui::End();
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