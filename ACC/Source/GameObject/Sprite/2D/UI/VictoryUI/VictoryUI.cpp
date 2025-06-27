#include "VictoryUI.h"

#include "Scenes/SceneManager/SceneManager.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectSound/SoundManager.h"
#include "Time/Time.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// UICSV�̃p�X
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	std::string WinJson = "Data\\Texture\\UIData\\Victory.json";
}


VictoryUI::VictoryUI()
	: m_SpawnTimeMax	( Time::GetInstance()->GetDeltaTime() * 300.f )
	, m_SpawnTime		( m_SpawnTimeMax )
{
	// �L�����N�^�[CSV�̏��ۑ��p
	std::unordered_map<std::string, std::string> m_StateList;
	// �L�����N�^�[CSV�̏��擾
	m_StateList = FileManager::CSVLoad(UICSVPath);

	// ��łȂ��ꍇ�́A�O���Œ�������ׂ��ϐ��̒l�����Ă���
	if (!m_StateList.empty())
	{
		m_SpawnTimeMax = StrToFloat(m_StateList["Win_SpawnTimeMax"]) * Time::GetInstance()->GetDeltaTime();
	}
}
VictoryUI::~VictoryUI()
{
	Release();
}


//=============================================================================
//		�쐬����
//=============================================================================
void VictoryUI::Create()
{
	LoadFromJson(WinJson, m_pUIs);
}


//=============================================================================
//		�Ǎ�����
//=============================================================================
HRESULT VictoryUI::LoadData()
{
	return S_OK;
}


//=============================================================================
//		����������
//=============================================================================
void VictoryUI::Init()
{
}


//=============================================================================
//		�X�V����
//=============================================================================
void VictoryUI::Update()
{
	if (m_SpawnTime >= 0) { m_SpawnTime -= Time::GetInstance()->GetDeltaTime(); }

	//----------------------------------------------------------------------------
	//		���ꂼ���UI�̍X�V
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) 
	{
		// �w�i�𓧉߂�����
		if ( m_pUIs[i]->GetSpriteData().Name == "Black" ) { m_pUIs[i]->SetAlpha(0.6f); }

		// �o�����Ԃ��I�������ꍇ
		if( m_SpawnTime < 0.f ) {
			// �^�C�g����ʂɑJ��
			SceneManager::GetInstance()->LoadScene(SceneList::eTitle);
			SoundManager::GetInstance()->AllStop();
		}
	}
}


//=============================================================================
//		�`�揈��
//=============================================================================
void VictoryUI::Draw()
{	
	// UI���ꂼ��̕`�揈��
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		m_pUIs[i]->Draw();
	}
}


//=============================================================================
//		�����������
//=============================================================================
void VictoryUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}