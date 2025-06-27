#include "DefeatUI.h"

#include "Scenes/SceneManager/SceneManager.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectSound/SoundManager.h"
#include "Time/Time.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// UICSV�̃p�X
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	std::string LoseJson = "Data\\Texture\\UIData\\Defeat.json";
}


DefeatUI::DefeatUI()
	: m_SpawnTimeMax	( Time::GetDeltaTime() * 180.f )
	, m_SpawnTime		( m_SpawnTimeMax )
{
	// �L�����N�^�[CSV�̏��ۑ��p
	std::unordered_map<std::string, std::string> m_StateList;
	// �L�����N�^�[CSV�̏��擾
	m_StateList = FileManager::CSVLoad(UICSVPath);

	// ��łȂ��ꍇ�́A�O���Œ�������ׂ��ϐ��̒l�����Ă���
	if (!m_StateList.empty())
	{
		m_SpawnTimeMax = StrToFloat(m_StateList["Lose_SpawnTimeMax"]) * Time::GetDeltaTime();
	}
}
DefeatUI::~DefeatUI()
{
	Release();
}


//=================================================================================================
//		�쐬����
//=================================================================================================
void DefeatUI::Create()
{
	LoadFromJson(LoseJson, m_pUIs);
}


//=================================================================================================
//		�Ǎ�����
//=================================================================================================
HRESULT DefeatUI::LoadData()
{
	return S_OK;
}


//=================================================================================================
//		����������
//=================================================================================================
void DefeatUI::Init()
{
}


//=================================================================================================
//		�X�V����
//=================================================================================================
void DefeatUI::Update()
{
	if (m_SpawnTime >= 0) { m_SpawnTime -= Time::GetDeltaTime(); }

	//----------------------------------------------------------------------------
	//		���ꂼ���UI�̍X�V
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) 
	{
		// �w�i�𓧉߂�����
		if ( m_pUIs[i]->GetSpriteData().Name == "Black") { m_pUIs[i]->SetAlpha(0.6f); }

		// �o�����Ԃ��I�������ꍇ
		if (m_SpawnTime < 0.f) {
			// �^�C�g���ɑJ��
			SceneManager::GetInstance()->LoadScene(SceneList::eTitle);
			SoundManager::GetInstance()->AllStop();
		}
	}
}


//=================================================================================================
//		�`�揈��
//=================================================================================================
void DefeatUI::Draw()
{	
	// UI���ꂼ��̕`�揈��
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
}


//=================================================================================================
//		�������
//=================================================================================================
void DefeatUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}