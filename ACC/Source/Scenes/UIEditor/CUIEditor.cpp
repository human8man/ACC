#include "CUIEditor.h"
#include "DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
#include "FileManager/FileManager.h"
#include "FileManager/LoadImage/LoadImage.h"
#include "nlohmann/json.hpp"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "Time/CTime.h"

#ifdef _DEBUG
#include "ImGui/CImGui.h"
#endif


// json�^���쐬.
using Json = nlohmann::json;

namespace {
	// �V�F�[�_�[�̃p�X.
	const TCHAR ShaderPath[]		= _T("Data\\Shader\\UIEditorLineShader.hlsl");
	// TitleUI�̃p�X.
	constexpr char TitleImagePath[]	= "Data\\Texture\\Title";
	// GameUI�̃p�X.
	constexpr char GameImagePath[]	= "Data\\Texture\\Game";
	// WinUI�̃p�X.
	constexpr char WinImagePath[]	= "Data\\Texture\\Win";
	// LoseUI�̃p�X.
	constexpr char LoseImagePath[]	= "Data\\Texture\\Lose";

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
}


//============================================================================
//		UI�G�f�B�^�[�N���X.
//============================================================================
CUIEditor::CUIEditor(HWND hWnd)
{
	m_hWnd = hWnd;
}
CUIEditor::~CUIEditor()
{
}


//=============================================================================
//		�쐬����.
//=============================================================================
void CUIEditor::Create()
{
	CSoundManager::GetInstance()->AllStop();
	SelectSceneLoad(UISceneList::Title);
	SelectInit();
	LoadLineShader();
}


//-----------------------------------------------------------------------------------
//		�V�F�[�_�Ǎ��֐��i���̂݁j.
//-----------------------------------------------------------------------------------
HRESULT CUIEditor::LoadLineShader()
{
	auto directx11 = CDirectX11::GetInstance();
	ID3D11Device* device = directx11->GetDevice();

	// �V�F�[�_�ǂݍ���.
	ID3D10Blob* vsBlob = nullptr;
	ID3D10Blob* psBlob = nullptr;
	ID3D10Blob* gsBlob = nullptr;

	D3DX11CompileFromFile(ShaderPath, nullptr, nullptr, "VS_Main", "vs_5_0", 0, 0, nullptr, &vsBlob, nullptr, nullptr);
	D3DX11CompileFromFile(ShaderPath, nullptr, nullptr, "PS_Main", "ps_5_0", 0, 0, nullptr, &psBlob, nullptr, nullptr);
	D3DX11CompileFromFile(ShaderPath, nullptr, nullptr, "GS_Main", "gs_5_0", 0, 0, nullptr, &gsBlob, nullptr, nullptr);

	// �V�F�[�_�쐬.
	device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	device->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &m_pGeometryShader);

	// ���̓��C�A�E�g.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_pInputLayout);

	// ���_�o�b�t�@.
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bd, nullptr, &m_pVertexBuffer);

	// �萔�o�b�t�@.
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
//		�V�F�[�_�X�V�֐�.
//-----------------------------------------------------------------------------------
void CUIEditor::UpdateShader()
{
	auto directx11 = CDirectX11::GetInstance();
	ID3D11DeviceContext* context = directx11->GetContext();

	// ---- �萔�o�b�t�@�X�V ----
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

	// ---- ���_�o�b�t�@�X�V ----
	if (lineVertices) {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(context->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			memcpy(mappedResource.pData, lineVertices, sizeof(Vertex) * 8);
			context->Unmap(m_pVertexBuffer, 0);
		}
	}

	// ---- �p�C�v���C���ݒ� ----
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetInputLayout(m_pInputLayout); // ��������ǉ��I
	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	context->VSSetShader(m_pVertexShader, nullptr, 0);
	context->GSSetShader(m_pGeometryShader, nullptr, 0);
	context->PSSetShader(m_pPixelShader, nullptr, 0);

	// ---- �`�� ----
	context->Draw(8, 0);
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


//-----------------------------------------------------------------------------
//		UI�I�����ɉ��ϐ���������������.
//-----------------------------------------------------------------------------
void CUIEditor::SelectInit()
{
	m_PatternNo		= POINTS(0, 0);
	m_PatternMax	= POINTS(1, 1);
	m_PatternAuto	= false;
}


//=============================================================================
//		�X�V����.
//=============================================================================
void CUIEditor::Update()
{
#ifdef _DEBUG
	// �V�[����I������.
	ImGuiSelectScene();

	//--------------------------------------------------------------
	//		UI��������.
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI�G�f�B�^�[�E�B���h�E"));

	// UI���X�g�̌����֐�.
	ImGuiSearchUI();

	//-----------------------------------------------------------
	//		�I�𒆂̃I�u�W�F�N�g�̕ҏW.
	//-----------------------------------------------------------
	if (m_SelectedUIIndex >= 0 && m_SelectedUIIndex < m_pUIs.size()) {
		// �I�����ꂽUI���Ăт₷������.
		CUIObject* selectedUI = m_pUIs[m_SelectedUIIndex];
		// �I������Ă���UI��\��.
		ImGui::Text(IMGUI_JP("�I������Ă���UI: %s"), selectedUI->GetSpriteData().Name.c_str());

		// �I�����ꂽUI���n�C���C�g����.
		ImGuiSetShader(selectedUI);

		// ���W�̒���.
		ImGuiPosEdit(selectedUI);
		// Z���W����Ƀ\�[�g.
		SortBySpritePosZ(selectedUI);


		// �摜���̒���.
		ImGuiInfoEdit(selectedUI);


		// �摜�p�^�[��������.
		ImGuiPatternTest(selectedUI);


		// ���̑��̏��̒���.
		ImGuiEtcInfoEdit(selectedUI);
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
	UpdateShader();
}


//=============================================================================
//		�������.
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
		ui->AttachSprite(sprite);
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
		if ( FAILED( FileManager::JsonSave( TextPath, SpriteState ))) return E_FAIL;
	}
	return S_OK;
}


//-----------------------------------------------------------------------------
//		Z���W�����Ƀ\�[�g����֐�.
//-----------------------------------------------------------------------------
void CUIEditor::SortBySpritePosZ(CUIObject* object)
{
	// �������W�Ɋւ��ĕύX���Ȃ��ꍇ�������^�[��.
	if (!m_MovedSpritePos) { return; }

	// �\�[�g�O�̑I������Ă��� UI �̃|�C���^��ۑ�
	CUIObject* pPreviousSelectedUI = object;

	// Z���W����Ƀ\�[�g����.
	std::sort(m_pUIs.begin(), m_pUIs.end(), [](const CUIObject* a, const CUIObject* b) {
		if (a && a->GetPos() && b && b->GetPos()) {
			return a->GetPos().z < b->GetPos().z;
		}
		return false;
		});

	// �\�[�g��ɁA�ȑO�I������Ă��� UI ���܂����X�g�ɑ��݂��邩�m�F���A�đI��.
	if (pPreviousSelectedUI != nullptr)
	{
		auto it = std::find(m_pUIs.begin(), m_pUIs.end(), pPreviousSelectedUI);
		if (it != m_pUIs.end()) {
			// �đI��.
			m_SelectedUIIndex = std::distance(m_pUIs.begin(), it);
		}
		else {
			// �ȑO�I������Ă���UI�����X�g�ɂȂ��ꍇ.
			object = nullptr;
			m_SelectedUIIndex = 0; // �܂��͓K�؂ȃf�t�H���g�l
		}
	}

	m_MovedSpritePos = false;
}


//-----------------------------------------------------------------------------
//		ImGui��p�����V�[���I���֐�.
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiSelectScene()
{
#ifdef _DEBUG
	ImGui::Begin(IMGUI_JP("UI�G�f�B�^�[�p�V�[���I��"));
	UISceneList scene;
	bool click = false;
	if (ImGui::Button("Title")) { scene = UISceneList::Title;	click = true; }
	if (ImGui::Button("Game")) { scene = UISceneList::Game;	click = true; }
	if (ImGui::Button("Lose")) { scene = UISceneList::Lose;	click = true; }
	if (ImGui::Button("Win")) { scene = UISceneList::Win;		click = true; }
	if (m_MovedSomething && click && (MessageBox(NULL,
		L"�{���ɐ؂�ւ��܂����H\n(�ۑ����Ă��Ȃ��ꍇ���Z�b�g����܂�)", L"�m�F",
		MB_YESNO | MB_ICONQUESTION)) == IDYES) {
		SelectSceneLoad(scene);
		m_MovedSomething = false;
	}
	else if (!m_MovedSomething && click) {
		SelectSceneLoad(scene);
	}

	ImGui::End();
#endif
}


//-----------------------------------------------------------------------------
//		UI���X�g�����֐�.
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiSearchUI()
{
#ifdef _DEBUG
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
				SelectInit();
			}
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}
#endif
}


//-----------------------------------------------------------------------------
//		�I�����ꂽUI���n�C���C�g����.
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiSetShader(CUIObject* object)
{
#ifdef _DEBUG
	auto deviceContext = CDirectX11::GetInstance()->GetContext();

	// �p�����p�̔{��.
	int offsetcorner = 180;

	ImGui::Begin(IMGUI_JP("�n�C���C�g��"));
	ImGui::DragFloat("Thickness", &m_LineThickness, 0.001f, 0.001f, 50.0f);
	ImGui::ColorEdit4("Line Color##", (float*)&m_LineColor);
	ImGui::End();

	float centerOffsetX = NOWFWND_W * 0.5f;
	float centerOffsetY = NOWFWND_H * 0.5f;
	// ��ƂȂ钸�_���W�i�����j.
	D3DXVECTOR3 basePos = {
		object->GetPos().x - centerOffsetX,
		(NOWFWND_H - object->GetPos().y) - centerOffsetY,  // Y���W�𔽓]
		object->GetPos().z
	};
	// ��ӂ̉E���_���W.
	D3DXVECTOR3 toplineright = {
		basePos.x + object->GetSpriteData().Disp.w,
		basePos.y,
		basePos.z
	};

	// �E�ӂ̏㉺���W.
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

	// ���ӂ̍��E���W.
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
	// ���ӂ̏㉺���W.
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

	// ���.
	lineVertices[0] = { basePos,		m_LineColor };
	lineVertices[1] = { toplineright,	m_LineColor };
	// �E��.
	lineVertices[2] = { rightlinetop,	m_LineColor };
	lineVertices[3] = { rightlinebottom,m_LineColor };
	// ����.
	lineVertices[4] = { bottomlineright,m_LineColor };
	lineVertices[5] = { bottomlineleft,	m_LineColor };
	// ����.
	lineVertices[6] = { leftlinebottom,	m_LineColor };
	lineVertices[7] = { leftlinetop,	m_LineColor };


	// �V�F�[�_�[�̃o�b�t�@�ɒ��_�����Z�b�g����.
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
#endif
}


//-----------------------------------------------------------------------------
//		���W�����֐�(�I�����ꂽUIObect).
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiPosEdit(CUIObject* object)
{
#ifdef _DEBUG
	if (ImGui::TreeNode(IMGUI_JP("���W"))) 
	{
		// �h���b�O�p�Ƀ}�E�X�����DirectInpu��p��.
		CMouse* Mouse = CInput::GetInstance()->CDMouse();
		D3DXVECTOR3 pos = object->GetPos();
		bool posdrag = ImGui::DragFloat3("##Position", pos, 1.f);
		bool posinput = ImGui::InputFloat3("##InputPos", pos);

		// �}�E�X�ʒu���擾.
		POINT MousePos;
		GetCursorPos(&MousePos);
		// �摜�͈͓��ō��N���b�N���͒��̏ꍇ�A�h���b�O������J�n.
		if (object->PointInSquare(MousePos, CLIENTRECT) && !m_DoDrag) {
			if (Mouse->IsLAction()) {
				m_DoDrag = true;
				m_OffsetPos = D3DXVECTOR2(pos.x - MousePos.x, pos.y - MousePos.y);
			}
		}
		if (m_DoDrag) {
			posdrag = true;
			// �␳�l+�}�E�X���W�������W������.
			pos = D3DXVECTOR3(MousePos.x + m_OffsetPos.x, MousePos.y + m_OffsetPos.y, pos.z);
			// �}�E�X�̍��N���b�N��b�����ꍇ�A�h���b�O������~.
			if (!Mouse->IsLDown()) { m_DoDrag = false; }
		}

		// �ύX���������ꍇ�ۑ�����.
		if (posdrag || posinput) {
			object->SetPos(pos);
			m_MovedSomething = true;
			m_MovedSpritePos = true;
		}
		ImGui::TreePop();
	}
#endif
}


//-----------------------------------------------------------------------------
//		��񒲐��֐�(�I�����ꂽUIObect).
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiInfoEdit(CUIObject* object)
{
#ifdef _DEBUG
	if (ImGui::TreeNode(IMGUI_JP("�摜���")))
	{
		// ���A�\���A�������ꂼ��̃T�C�Y����.
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
		bool basedrag	= ImGui::DragFloat2("##BaseDrag", base, 1.f);
		bool baseinput	= ImGui::InputFloat2("##BaseInput", base);

		ImGui::Text(IMGUI_JP("�\���T�C�Y(x,y)"));
		bool dispdrag	= ImGui::DragFloat2("##DispDrag", disp, 1.f);
		bool dispinput	= ImGui::InputFloat2("##DispInput", disp);

		ImGui::Text(IMGUI_JP("�����T�C�Y(x,y)"));
		bool stridedrag	= ImGui::DragFloat2("##StrideDrag", stride, 1.f);
		bool strideinput= ImGui::InputFloat2("##StrideInput", stride);

		// �ύX���������ꍇ�ۑ�����.
		if (basedrag	|| baseinput
		||	dispdrag	|| dispinput
		||	stridedrag	|| strideinput)
		{
			object->SetBase(base);
			object->SetDisp(disp);
			object->SetStride(stride);
			m_MovedSomething = true;
		}
		ImGui::TreePop();
	}
#endif
}


//-----------------------------------------------------------------------------
//		�摜�p�^�[���������֐�(�I�����ꂽUIObect).
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiPatternTest(CUIObject* object)
{
#ifdef _DEBUG
	if (ImGui::TreeNode(IMGUI_JP("�摜�p�^�[��������"))) 
	{
		m_PatternNo = object->GetPatternNo();
		int pattern[2] = { m_PatternNo.x,m_PatternNo.y };
		int patternmax[2] = { m_PatternMax.x,m_PatternMax.y };

		// �p�^�[���̍ő吔�����߂�.
		ImGui::Text(IMGUI_JP("�p�^�[���̏��"));
		ImGui::PushItemWidth(100.0f);
		ImGui::InputInt("##x", &patternmax[0]); ImGui::SameLine(); ImGui::InputInt("##y", &patternmax[1]);
		ImGui::PopItemWidth();

		// ������1�Œ�.
		if (patternmax[0] < 1) { patternmax[0] = 1; }
		if (patternmax[1] < 1) { patternmax[1] = 1; }
		m_PatternMax = POINTS(patternmax[0], patternmax[1]);

		// �p�^�[���̃N���b�N����.
		if (ImGui::TreeNode(IMGUI_JP("�N���b�N����"))) {
			ImGui::PushItemWidth(100.0f);
			ImGui::InputInt("##xclickpattern", &pattern[0]); ImGui::SameLine(); ImGui::InputInt("##yclickpattern", &pattern[1]);
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}

		// �p�^�[���̃I�[�g��������.
		if (ImGui::TreeNode(IMGUI_JP("�I�[�g��������"))) {
			// ���s���̏���.
			if (m_PatternAuto) {
				ImGui::Text("On");
				m_AnimationSpeed -= CTime::GetInstance()->GetDeltaTime();
				if (m_AnimationSpeed < 0) {
					m_AnimationSpeed = m_AnimationSpeedMax * CTime::GetInstance()->GetDeltaTime();
					pattern[0]++;

					// x���ő�l�𒴂��Ay���ő�l�̏ꍇ�A�j���[�V�������ŏ����瑗����悤�ɂ���.
					if (m_PatternMax.x < pattern[0] && m_PatternMax.y == pattern[1]) {
						// y��0�ȉ��ɂȂ����ꍇ�͏�����Ԃɂ���.
						pattern[0] = 0; pattern[1] = 0;
					}
				}
			}
			else {
				ImGui::Text("Off");
				m_AnimationSpeed = m_AnimationSpeedMax * CTime::GetInstance()->GetDeltaTime();
			}
			ImGui::PushItemWidth(100.0f);
			// ���s�̐؂�ւ�.
			if (ImGui::Button(IMGUI_JP("�ؑ�"))) { m_PatternAuto = !m_PatternAuto; }
			// ���葬�x�̐ݒ�.
			ImGui::InputFloat(IMGUI_JP("���葬�x�ݒ�(�t���[��)"), &m_AnimationSpeedMax);
			ImGui::PopItemWidth();

			ImGui::TreePop();
		}

		// X���ő�l�𒴂����ꍇ.
		if (m_PatternMax.x < pattern[0]) {
			// Y���ő�l�ȏ�̏ꍇ�AX���ő�l�ɂ���.
			if (m_PatternMax.y <= pattern[1]) {
				pattern[0] = m_PatternMax.x;
			}
			else {
				pattern[0] = 0; pattern[1]++;
			}
		}
		else if (pattern[0] < 0) {
			// �Œ�l��0�ɌŒ肵�Ay�̒l���J�艺����.
			pattern[0] = 0; pattern[1]--;
		}

		// Y���ő�l�𒴂����ꍇ.
		if (m_PatternMax.y < pattern[1]) {
			pattern[0] = m_PatternMax.x;
			pattern[1] = m_PatternMax.y;
		}
		else if (pattern[1] < 0) {
			// y��0�ȉ��ɂȂ����ꍇ�͏�����Ԃɂ���.
			pattern[0] = 0; pattern[1] = 0;
		}

		// ���f����.
		m_PatternNo = POINTS(pattern[0], pattern[1]);
		object->SetPatternNo(m_PatternNo.x, m_PatternNo.y);
		ImGui::TreePop();
	}
#endif
}


//-----------------------------------------------------------------------------
//		���̑��̏�񒲐��֐�(�I�����ꂽUIObect).
//-----------------------------------------------------------------------------
void CUIEditor::ImGuiEtcInfoEdit(CUIObject* object)
{
#ifdef _DEBUG
	if (ImGui::TreeNode(IMGUI_JP("���̑�")))
	{
		float alpha = object->GetAlpha();
		D3DXVECTOR3 scale = object->GetScale();
		D3DXVECTOR3 rot = object->GetRot();

		ImGui::Text(IMGUI_JP("�A���t�@"));
		bool alphaslider = ImGui::SliderFloat("##AlphaSlider", &alpha, 0.f, 1.f);
		bool alphainput = ImGui::InputFloat("##AlphaInput", &alpha);

		ImGui::Text(IMGUI_JP("�X�P�[��"));
		bool scaledrag = ImGui::DragFloat3("##ScaleDrag", scale, 0.1f);
		bool scaleinput = ImGui::InputFloat3("##ScaleInput", scale);

		ImGui::Text(IMGUI_JP("��]"));
		bool rotdrag = ImGui::DragFloat3("##RotDrag", rot, 0.1f);
		bool rotinput = ImGui::InputFloat3("##RotInput", rot);

		// �ύX���������ꍇ�ۑ�����.
		if (alphaslider	|| alphainput
		||	scaledrag	|| scaleinput
		||	rotdrag		|| rotinput)
		{
			object->SetAlpha(alpha);
			object->SetScale(scale);
			object->SetRot(rot);
			m_MovedSomething = true;
		}
		ImGui::TreePop();
	}
#endif
}
