#include "CTitle.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "GameObject/Sprite/2D/UI/CUIObject.h"
#include "Common/DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
#include "GameObject/Mesh/Static/CStaticMesh.h"
#include "../GameObject/Camera/CCamera.h"

#if _DEBUG
	#include "ImGui/CImGui.h"
#endif


namespace {
	constexpr char TitleImagePath[] = "Data\\Texture\\Title";
}


//======================================================================================================================
//		�^�C�g���N���X.
//======================================================================================================================
CTitle::CTitle(HWND hWnd)
	: m_Light			()
	, m_mView			()
	, m_mProj			()
	, m_SpriteDataList	()
	, m_SpritePosList	()
	, m_pUIs			()
	, m_pSprite2Ds		()
	, m_pEgg			()
{
	m_hWnd = hWnd;
	m_Light.vDirection = D3DXVECTOR3(1.f, 5.f, 0.f);	//���C�g����.
}

CTitle::~CTitle()
{
	Release();
}


//======================================================================================================================
//		�\�z�֐�.
//======================================================================================================================
void CTitle::Create()
{
	int index = 0;

	// �w�肵���f�B���N�g�����𑖍�.
	for (const auto& entry : std::filesystem::directory_iterator(TitleImagePath)) {
		// json�̏ꍇ��蒼��.
		std::string Extension = entry.path().string();
		Extension.erase(0, entry.path().string().rfind("."));
		if (Extension == ".json") continue;

		m_pSprite2Ds.push_back(new CSprite2D());
		m_pUIs.push_back(new CUIObject());

		m_pSprite2Ds[index]->Init(entry.path().string());
		m_pUIs[index]->AttachSprite(*m_pSprite2Ds[index]);
		m_pUIs[index]->SetPos(m_pSprite2Ds[index]->GetSpriteData().Pos);
		m_SpritePosList.push_back(m_pUIs[index]->GetPos());
		index++;
	}
	m_pEgg = new CStaticMesh();

}


//======================================================================================================================
//		�ǂݍ���.
//======================================================================================================================
HRESULT CTitle::LoadData()
{
	m_pEgg->Init(_T("Data\\Mesh\\Static\\Player\\egg.x"));
	m_pEgg->SetPos(D3DXVECTOR3(5.f, 1.f, 2.5f));


	return S_OK;
}


//======================================================================================================================
//		�J��.
//======================================================================================================================
void CTitle::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pUIs[i]);
	}
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pSprite2Ds[i]);
	}
	SAFE_DELETE(m_pEgg);
}


//======================================================================================================================
//		������.
//======================================================================================================================
void CTitle::Init()
{
	CCamera::GetInstance()->Init();	// �J�����̏�����.
}


//======================================================================================================================
//		�X�V.
//======================================================================================================================
void CTitle::Update()
{
	CMouse* Mouse = CDInput::GetInstance()->CDMouse();

	// �}�E�X�ʒu���擾.
	POINT MousePos;
	GetCursorPos(&MousePos);

	// �E�B���h�E�ʒu���擾(�E�B���h�E�^�u���܂�).
	RECT WindowRect;
	GetWindowRect(m_hWnd, &WindowRect);
	RECT clientRect;
	GetClientRect(m_hWnd, &clientRect);

	POINT topLeft = { clientRect.left, clientRect.top };
	POINT bottomRight = { clientRect.right, clientRect.bottom };
	ClientToScreen(m_hWnd, &topLeft);
	ClientToScreen(m_hWnd, &bottomRight);

	int borderLeft = topLeft.x - WindowRect.left;
	int borderTop = topLeft.y - WindowRect.top;

	D3DXVECTOR2 windowrect = D3DXVECTOR2(
		static_cast<float>(borderLeft + WindowRect.left),
		static_cast<float>(borderTop + WindowRect.top));


	// ���ꂼ���UI�̍X�V������.
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		// �w�i�̏����͂���Ȃ��̂ő����ɐ؂�.
		if (i == TitleSprite::FullScreen) { continue; }

		// �l�p�̍��W.
		D3DXVECTOR2 SquarePos = D3DXVECTOR2(
			m_pUIs[i]->GetPos().x, m_pUIs[i]->GetPos().y);

		// �l�p�̕\���͈�.
		D3DXVECTOR2 SquareDisp = D3DXVECTOR2(
			m_pUIs[i]->GetSpriteData().Disp.w, m_pUIs[i]->GetSpriteData().Disp.h);

		// �_�Ǝl�p�̓����蔻��.
		if (PointInSquare(MousePos, SquarePos + windowrect, SquareDisp))
		{
			//	�O��I������Ă��Ȃ������ꍇ().
			if (m_pUIs[i]->GetPatternNo().x == 0) {
				CSoundManager::GetInstance()->PlaySE(CSoundManager::SE_SelectMove);
			}
			m_pUIs[i]->SetPatternNo(1, 0);
		}
		else {
			m_pUIs[i]->SetPatternNo(0, 0);
		}

		// �X�^�[�g�{�^���ɃJ�[�\�����d�Ȃ��Ă��鎞.
		if (i == TitleSprite::StartButton && m_pUIs[i]->GetPatternNo().x) {
			if (Mouse->IsLAction()) {
				CSceneManager::GetInstance()->LoadScene(SceneList::Game);
			}
		}


	}
	

#if _DEBUG
	ImGui::Begin("MousePos");
	POINT pos;
	if (GetCursorPos(&pos)) 
	{
		ImGui::Text("%d,%d", pos.x, pos.y);
	}
	ImGui::End();
#endif
}


//======================================================================================================================
//		�`��.
//======================================================================================================================
void CTitle::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);
	
	// UI���ꂼ��̕`�揈��.
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		m_pUIs[i]->Draw();
		if (i == TitleSprite::FullScreen)
		{
			m_pEgg->Render(m_mView, m_mProj, m_Light);
		}
	}
}


//------------------------------------------------------------------------------------------------
//		������.
//------------------------------------------------------------------------------------------------
bool CTitle::PointInSquare( POINT ppos, D3DXVECTOR2 spos, D3DXVECTOR2 sposs)
{
	if (spos.x < ppos.x
	&&  spos.y < ppos.y
	&&  ppos.x < spos.x + sposs.x
	&&  ppos.y < spos.y + sposs.y )
	{
		return true;
	}

	return false;
}
