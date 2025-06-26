#include "TitleUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectSound/CSoundManager.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	std::string TitleJson = "Data\\Texture\\UIData\\Title.json";

	// �񏈗���UI���X�g
	std::vector<std::string> ignoreList = {
		"Black",
		"Title"
	};
}


TitleUI::TitleUI()
	: m_Start			( false )
{
}
TitleUI::~TitleUI()
{
	Release();
}


//=================================================================================================
//		�쐬����
//=================================================================================================
void TitleUI::Create()
{
	LoadFromJson(TitleJson, m_pUIs);
}


//=================================================================================================
//		�Ǎ�����
//=================================================================================================
HRESULT TitleUI::LoadData()
{
	return S_OK;
}


//=================================================================================================
//		����������
//=================================================================================================
void TitleUI::Init()
{
}


//=================================================================================================
//		�X�V����
//=================================================================================================
void TitleUI::Update()
{
	CMouse*	Mouse	= CInput::GetInstance()->CDMouse();
	CKey*	Key		= CInput::GetInstance()->CDKeyboard();
	
	// �}�E�X�ʒu���擾
	POINT MousePos;
	GetCursorPos(&MousePos);
	
	//----------------------------------------------------------------------------
	//		���ꂼ���UI�̍X�V
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		// �����̂���Ȃ�UI�𑁊��ɐ؂�
		if (std::find(
			ignoreList.begin(),
			ignoreList.end(), 
			m_pUIs[i]->GetSpriteData().Name) != ignoreList.end()) { continue; }

		// �_�Ǝl�p�̓����蔻��
		if ( m_pUIs[i]->PointInSquare( MousePos, CLIENTRECT ) )
		{
			//	�O��I������Ă��Ȃ������ꍇSE��炷
			if ( m_pUIs[i]->GetPatternNo().x == 0 ) {
				CSoundManager::GetInstance()->Play(CSoundManager::SE_SelectMove);
			}
			m_pUIs[i]->SetPatternNo(1, 0);
		}
		else {
			m_pUIs[i]->SetPatternNo(0, 0);
		}

		// ����̖��O�̏ꍇ
		if ( m_pUIs[i]->GetSpriteData().Name == "StartButton" )
		{
			// ���łɃJ�[�\���őI������Ă���ꍇ
			if ( m_pUIs[i]->GetPatternNo().x ) {
				if (Mouse->IsLAction()) { m_Start = true; }
			}
			else {
				// SPACE�L�[�ŃQ�[���J�n
				if (Key->IsKeyAction(DIK_SPACE)) {
					// �I����Ԃɂ���
					m_pUIs[i]->SetPatternNo(1, 0);
					m_Start = true;
				}
			}
		}
	}
}


//=================================================================================================
//		�`�揈��
//=================================================================================================
void TitleUI::Draw()
{
	// UI���ꂼ��̕`�揈��
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
}


//=================================================================================================
//		�������
//=================================================================================================
void TitleUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}
