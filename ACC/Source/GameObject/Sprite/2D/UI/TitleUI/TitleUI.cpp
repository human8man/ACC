#include "TitleUI.h"

#include "Scenes/SceneManager/SceneManager.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectSound/SoundManager.h"
#include "Time/Time.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "Sprite/2D/UI/NumberUI/NumberUI.h"


namespace {
	std::string TitleJson = "Data\\Texture\\UIData\\Title.json";

	// �񏈗���UI���X�g
	std::vector<std::string> ignoreList = {
		"Black",
		"Title"
	};
}


TitleUI::TitleUI()
	: m_pNumberUI	( nullptr )
	, m_NumPos		( ZEROVEC3 )
	, m_Size		( ZEROVEC3 )
	, m_Start		( false )
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
	m_pNumberUI = std::make_unique<NumberUI>();
	m_pNumberUI->Create();
	LoadFromJson(TitleJson, m_pUIs);

	for (auto it = m_pUIs.begin(); it != m_pUIs.end(); )
	{
		// UIEditor���Őݒ肵�������̒�������W���擾
		if ((*it)->GetSpriteData().Name == "Number") {
			m_NumPos = (*it)->GetPos();
			m_NumPos.x += (*it)->GetSpriteData().Disp.w/2;
			m_Size = D3DXVECTOR3((*it)->GetSpriteData().Disp.w, (*it)->GetSpriteData().Disp.h,0.f);
			delete* it;
			it = m_pUIs.erase(it);
		}
		else {
			++it;
		}
	}
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
	Mouse*	Mouse	= DirectInput::GetInstance()->CDMouse();
	Key*	Key		= DirectInput::GetInstance()->CDKeyboard();
	
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
			m_pUIs[i]->GetSpriteData().Name) != ignoreList.end()) {
			continue;
		}

		// �_�Ǝl�p�̓����蔻��
		if (m_pUIs[i]->PointInSquare(MousePos, CLIENTRECT)
			&& !(m_pUIs[i]->GetSpriteData().Name == "Number"))
		{
			//	�O��I������Ă��Ȃ������ꍇSE��炷
			if (m_pUIs[i]->GetPatternNo().x == 0) {
				SoundManager::GetInstance()->Play(SoundManager::SE_SelectMove);
			}
			m_pUIs[i]->SetPatternNo(1, m_pUIs[i]->GetPatternNo().y);
		}
		else {
			m_pUIs[i]->SetPatternNo(0, m_pUIs[i]->GetPatternNo().y);
		}

		// ����̖��O�̏ꍇ
		if (m_pUIs[i]->GetSpriteData().Name == "StartButton")
		{
			// ���łɃJ�[�\���őI������Ă���ꍇ
			if (m_pUIs[i]->GetPatternNo().x) {
				if (Mouse->IsLAction()) { m_Start = true; }
			}
			// SPACE�L�[�ŃQ�[���J�n
			if (Key->IsKeyAction(DIK_SPACE)) {
				// �I����Ԃɂ���
				m_pUIs[i]->SetPatternNo(1, 0);
				m_Start = true;
			}
		}


		if (m_pUIs[i]->GetSpriteData().Name == "LRButton"){
			// �����̂��ߒ������W����ɔ��f����
			if (m_NumPos.x > m_pUIs[i]->GetPos().x) {
				if (m_pUIs[i]->GetPatternNo().x) {
					if (Mouse->IsLDown()) { ENEMY_COUNT--; }
				}
				if (Key->IsKeyAction(DIK_LEFT)
					|| Key->IsKeyDown(DIK_A)) {
					ENEMY_COUNT--;
					m_pUIs[i]->SetPatternNo(1, m_pUIs[i]->GetPatternNo().y);
				}
				
			}
			else {
				m_pUIs[i]->SetPatternNo(m_pUIs[i]->GetPatternNo().x, 1);
				if (m_pUIs[i]->GetPatternNo().x) {
					if (Mouse->IsLDown()) { ENEMY_COUNT++; }
				}
				if (Key->IsKeyAction(DIK_RIGHT)
					|| Key->IsKeyDown(DIK_D)) {
					ENEMY_COUNT++;
					m_pUIs[i]->SetPatternNo(1, m_pUIs[i]->GetPatternNo().y);
				}
			}
		}
	}


	// �����ɂ���č��W��ς���
	D3DXVECTOR3 numposition = m_NumPos;
	if (ENEMY_COUNT / 10 > 0) {
		numposition = D3DXVECTOR3(m_NumPos.x - m_Size.x, m_NumPos.y, m_NumPos.z);
	}
	else {
		numposition = D3DXVECTOR3(m_NumPos.x - m_Size.x / 2, m_NumPos.y, m_NumPos.z);
	}
	ENEMY_COUNT = std::clamp(ENEMY_COUNT, 1, ENEMY_COUNTMAX);
	m_pNumberUI->SetNumber(numposition, ENEMY_COUNT, 1.f);
}


//=================================================================================================
//		�`�揈��
//=================================================================================================
void TitleUI::Draw()
{
	// UI���ꂼ��̕`�揈��
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
	m_pNumberUI->Draw();
}


//=================================================================================================
//		�������
//=================================================================================================
void TitleUI::Release()
{
	m_pNumberUI.reset();
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}
