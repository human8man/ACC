#include "COptionUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "Common/DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"


namespace {
	// �I�v�V����UI�̃p�X.
	constexpr char OptionImagePath[] = "Data\\Texture\\Option";
}


//=================================================================================================
//		�I�v�V�����N���X.
//=================================================================================================
COptionUI::COptionUI(HWND hWnd)
	: m_Light			()
	, m_mView			()
	, m_mProj			()
	, m_SpriteDataList	()
	, m_SpritePosList	()
	, m_pUIs			()
	, m_pSprite2Ds		()
{
	m_hWnd = hWnd;
	m_Light.vDirection = D3DXVECTOR3(1.f, 5.f, 0.f);
}

COptionUI::~COptionUI()
{
	Release();
}


//=================================================================================================
//		�\�z�֐�.
//=================================================================================================
void COptionUI::Create()
{
	int index = 0;

	// �w�肵���f�B���N�g�����𑖍�.
	for (const auto& entry : std::filesystem::directory_iterator(OptionImagePath)) {
		// ������json�̏ꍇ��蒼��.
		std::string Extension = entry.path().string();
		Extension.erase(0, entry.path().string().rfind("."));
		if (Extension == ".json") continue;

		// �C���X�^���X����.
		m_pSprite2Ds.push_back(new CSprite2D());
		m_pUIs.push_back(new CUIObject());

		// �摜�f�[�^�̓ǂݍ���.
		m_pSprite2Ds[index]->Init(entry.path().string());
		m_pUIs[index]->AttachSprite(*m_pSprite2Ds[index]);
		m_pUIs[index]->SetPos(m_pSprite2Ds[index]->GetSpriteData().Pos);
		m_SpritePosList.push_back(m_pUIs[index]->GetPos());
		index++;
	}
}


//=================================================================================================
//		�ǂݍ���.
//=================================================================================================
HRESULT COptionUI::LoadData()
{
	return S_OK;
}


//=================================================================================================
//		������.
//=================================================================================================
void COptionUI::Init()
{

}


//=================================================================================================
//		�X�V.
//=================================================================================================
void COptionUI::Update()
{
	CMouse* Mouse = CInput::GetInstance()->CDMouse();

	// �}�E�X�ʒu���擾.
	POINT MousePos;
	GetCursorPos(&MousePos);

	// �E�B���h�E�S�̂̈ʒu�ƃT�C�Y���擾�i�E�B���h�E�^�u��g���܂ށj.
	RECT WindowRect;
	GetWindowRect(m_hWnd, &WindowRect);

	// �N���C�A���g�̈�̈ʒu�ƃT�C�Y���擾�i�E�B���h�E���̕`��͈́j.
	RECT clientRect;
	GetClientRect(m_hWnd, &clientRect);

	// �N���C�A���g�̈�̍���ƉE���̍��W��������.
	POINT topLeft = { clientRect.left, clientRect.top };
	POINT bottomRight = { clientRect.right, clientRect.bottom };

	// �N���C�A���g�̈�̍��W���X�N���[�����W�n�ɕϊ�.
	ClientToScreen(m_hWnd, &topLeft);
	ClientToScreen(m_hWnd, &bottomRight);

	// �E�B���h�E�S�̂̍�����W�ƃN���C�A���g�̈�̍�����W�̍������v�Z.
	int borderLeft = topLeft.x - WindowRect.left;
	int borderTop = topLeft.y - WindowRect.top;

	// �t���[�������܂񂾃E�B���h�E�̈ʒu���Z�o.
	D3DXVECTOR2 windowrect = D3DXVECTOR2(
		static_cast<float>(borderLeft + WindowRect.left),
		static_cast<float>(borderTop + WindowRect.top));

	//----------------------------------------------------------------------------
	//		���ꂼ���UI�̍X�V.
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		// �w�i�̏����͂���Ȃ��̂ő����ɐ؂�.
		if (i == OptionSprite::FullScreen) { continue; }

		// UI�̃T�C�Y�ƍ��W��ϊ�����.
		D3DXVECTOR2 SquarePos = D3DXVECTOR2(m_pUIs[i]->GetPos().x, m_pUIs[i]->GetPos().y);
		D3DXVECTOR2 SquareDisp = D3DXVECTOR2(m_pUIs[i]->GetSpriteData().Disp.w, m_pUIs[i]->GetSpriteData().Disp.h);


		// �_�Ǝl�p�̓����蔻��.
		if (PointInSquare(MousePos, SquarePos + windowrect, SquareDisp))
		{
			//	�O��I������Ă��Ȃ������ꍇSE��炷.
			if (m_pUIs[i]->GetPatternNo().x == 0) {
				CSoundManager::GetInstance()->PlaySE(CSoundManager::SE_SelectMove);
			}
			m_pUIs[i]->SetPatternNo(1, 0);
		}
		else {
			m_pUIs[i]->SetPatternNo(0, 0);
		}


		// �X�^�[�g�{�^���ɃJ�[�\�����d�Ȃ��Ă��鎞.
		if (i == OptionSprite::StartButton && m_pUIs[i]->GetPatternNo().x) {
			if (Mouse->IsLAction()) {
				// �Q�[�����J�n����.
				CSceneManager::GetInstance()->LoadScene(SceneList::Game);
			}
		}
	}
}


//=================================================================================================
//		�`��.
//=================================================================================================
void COptionUI::Draw()
{
	// UI���ꂼ��̕`�揈��.
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		m_pUIs[i]->Draw();
	}
}


//=================================================================================================
//		�������.
//=================================================================================================
void COptionUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}