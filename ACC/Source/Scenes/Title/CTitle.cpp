#include "CTitle.h"

#include "Camera/CCamera.h"
#include "Mesh/Static/CStaticMesh.h"
#include "DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"

#ifdef _DEBUG
#include "ImGui/CImGui.h"
#endif

namespace {
	// �^�C�g�����UI�̃p�X.
	constexpr char TitleImagePath[] = "Data\\Texture\\Title";
	
	// UI���X�g.
	std::vector<std::string> TitleImageList = {
		"Black",
		"StartButton",
		"Title"
	};

	// �񏈗�UI���X�g.
	std::vector<std::string> ignoreList = {
		"Black",
		"Title"
	};
}


//=================================================================================================
//		�^�C�g���N���X.
//=================================================================================================
CTitle::CTitle(HWND hWnd)
	: m_Light			()
	, m_mView			()
	, m_mProj			()
	, m_SpriteDataList	()
	, m_SpritePosList	()
	, m_pUIs			()
	, m_pSprite2Ds		()
	, m_pEgg			()
	, m_Start			( false )
{
	m_hWnd = hWnd;
	m_Light.vDirection = D3DXVECTOR3(1.f, 5.f, 0.f);	//���C�g����.
}
CTitle::~CTitle()
{
	Release();
}


//=================================================================================================
//		�쐬����.
//=================================================================================================
void CTitle::Create()
{
	for (size_t i = 0;i < TitleImageList.size();++i)
	{
		// �C���X�^���X����.
		m_pSprite2Ds.push_back(CSpriteManager::GetInstance()->GetSprite(TitleImageList[i]));
		m_pUIs.push_back(new CUIObject());


		// �摜�f�[�^�̓ǂݍ���.
		m_pUIs[i]->AttachSprite(*m_pSprite2Ds[i]);
		m_pUIs[i]->SetPos(m_pSprite2Ds[i]->GetSpriteData().Pos);
		m_SpritePosList.push_back(m_pUIs[i]->GetPos());
	}

	m_pEgg = new CStaticMesh();
}


//=================================================================================================
//		�Ǎ�����.
//=================================================================================================
HRESULT CTitle::LoadData()
{
	m_pEgg->Init(_T("Data\\Mesh\\Static\\Player\\egg.x"));
	m_pEgg->SetPos(D3DXVECTOR3(10.f, -1.f, 4.f));

	return S_OK;
}


//=================================================================================================
//		����������.
//=================================================================================================
void CTitle::Init()
{
	m_Start = false;
	CCamera::GetInstance()->Init();	// �J�����̏�����.
}


//=================================================================================================
//		�X�V����.
//=================================================================================================
void CTitle::Update()
{
	// BGM�Đ�.
	CSoundManager::GetInstance()->PlayLoop(CSoundManager::enList::BGM_Title);

	CMouse*	Mouse	= CInput::GetInstance()->CDMouse();
	CKey*	Key		= CInput::GetInstance()->CDKeyboard();

	m_pEgg->SetRotY(m_pEgg->GetRot().y + 0.01f);

	// �}�E�X�ʒu���擾.
	POINT MousePos;
	GetCursorPos(&MousePos);

	//----------------------------------------------------------------------------
	//		���ꂼ���UI�̍X�V.
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		// �����̂���Ȃ�UI�𑁊��ɐ؂�.
		if (std::find(ignoreList.begin(), ignoreList.end(), 
			m_pUIs[i]->GetSpriteData().Name) != ignoreList.end()) { continue; }

		// �_�Ǝl�p�̓����蔻��.
		if (m_pUIs[i]->PointInSquare(MousePos, CLIENTRECT))
		{
			//	�O��I������Ă��Ȃ������ꍇSE��炷.
			if ( m_pUIs[i]->GetPatternNo().x == 0 ) {
				CSoundManager::GetInstance()->PlaySE(CSoundManager::SE_SelectMove);
			}
			m_pUIs[i]->SetPatternNo(1, 0);
		}
		else {
			m_pUIs[i]->SetPatternNo(0, 0);
		}

		// �X�^�[�g�{�^���ɃJ�[�\�����d�Ȃ��Ă��鎞.
		if (m_pUIs[i]->GetSpriteData().Name == TitleImageList[1])
		{
			// ���łɃJ�[�\���őI������Ă���ꍇ.
			if ( m_pUIs[i]->GetPatternNo().x ) {
				if (Mouse->IsLAction()) { m_Start = true; }
			}
			else {
				// SPACE�L�[�ŃQ�[���J�n.
				if (Key->IsKeyAction(DIK_SPACE)) {
					// �I����Ԃɂ���.
					m_pUIs[i]->SetPatternNo(1, 0);
					m_Start = true;
				}
			}
		}
	}

	if (m_Start) {
		// �Q�[�����J�n����.
		CSceneManager::GetInstance()->LoadScene(SceneList::Game);
		CSoundManager::GetInstance()->Stop(CSoundManager::enList::BGM_Title);
	}
}


//=================================================================================================
//		�`�揈��.
//=================================================================================================
void CTitle::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);
	
	// UI���ꂼ��̕`�揈��.
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		m_pUIs[i]->Draw();
		if (m_pUIs[i]->GetSpriteData().Name == TitleImageList[0])
		{
			m_pEgg->Render(m_mView, m_mProj, m_Light);
		}
	}
}


//=================================================================================================
//		�������.
//=================================================================================================
void CTitle::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
	SAFE_DELETE(m_pEgg);
}