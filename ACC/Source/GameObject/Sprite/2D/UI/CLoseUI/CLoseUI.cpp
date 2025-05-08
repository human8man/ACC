#include "CLoseUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectSound/CSoundManager.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// UICSV�̃p�X.
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	// UI���X�g.
	std::vector<std::string> ImageList = {
		"Black",
		"Lose"
	};
}


//=================================================================================================
//		LoseUI�N���X.
//=================================================================================================
CLoseUI::CLoseUI()
	: m_SpriteDataList	()
	, m_SpritePosList	()
	, m_pUIs			()
	, m_pSprite2Ds		()

	, m_SpawnTimeMax	( CTime::GetInstance()->GetDeltaTime() * 300.f )
	, m_SpawnTime		( m_SpawnTimeMax )
{
	// �L�����N�^�[CSV�̏��ۑ��p.
	std::unordered_map<std::string, std::string> m_StateList;
	// �L�����N�^�[CSV�̏��擾.
	m_StateList = FileManager::CSVLoad(UICSVPath);

	// ��łȂ��ꍇ�́A�O���Œ�������ׂ��ϐ��̒l�����Ă���.
	if (!m_StateList.empty())
	{
		m_SpawnTimeMax = StrToFloat(m_StateList["Lose_SpawnTimeMax"]) * CTime::GetInstance()->GetDeltaTime();
	}
}
CLoseUI::~CLoseUI()
{
	Release();
}


//=================================================================================================
//		�쐬����.
//=================================================================================================
void CLoseUI::Create()
{
	std::unordered_map<std::string, int> nameCounts; // ���O���Ƃ̏o���񐔂��L�^.

	for (size_t i = 0; i < ImageList.size(); ++i)
	{
		// ���O��肪����ꍇ�̏���.
		std::string baseName = ImageList[i];
		std::string numberedName;

		if (nameCounts.count(baseName) == 0) {
			numberedName = baseName;	// 1�ڂ͂��̂܂�.
			nameCounts[baseName] = 1;	// �������1�X�^�[�g.
		}
		else {
			numberedName = baseName + "_" + std::to_string(nameCounts[baseName]);
			nameCounts[baseName]++;
		}

		// �C���X�^���X����.
		m_pSprite2Ds.push_back(CSpriteManager::GetInstance()->GetSprite(baseName));
		m_pUIs.push_back(new CUIObject());
		CSprite2D* pSprite = CSpriteManager::GetInstance()->GetSprite(ImageList[i]);

		// �摜�f�[�^�̓ǂݍ���.
		m_pUIs.back()->AttachSprite(pSprite);
		m_pUIs.back()->SetPos(m_pSprite2Ds.back()->GetSpriteData().Pos);
		m_SpritePosList.push_back(m_pUIs.back()->GetPos());

		// �ύX��̖��O�ɂ��Ȃ���.
		m_pUIs.back()->SetSpriteName(numberedName);
	}
}


//=================================================================================================
//		�Ǎ�����.
//=================================================================================================
HRESULT CLoseUI::LoadData()
{
	return S_OK;
}


//=================================================================================================
//		����������.
//=================================================================================================
void CLoseUI::Init()
{
}


//=================================================================================================
//		�X�V����.
//=================================================================================================
void CLoseUI::Update()
{
	if (m_SpawnTime >= 0) { m_SpawnTime -= CTime::GetInstance()->GetDeltaTime(); }

	//----------------------------------------------------------------------------
	//		���ꂼ���UI�̍X�V.
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) 
	{
		// �w�i�𓧉߂�����.
		if ( m_pUIs[i]->GetSpriteData().Name == "Black") { m_pUIs[i]->SetAlpha(0.6f); }

		// �o�����Ԃ��I�������ꍇ.
		if (m_SpawnTime < 0.f) {
			// �^�C�g���ɑJ��.
			CSceneManager::GetInstance()->LoadScene(SceneList::Title);
			CSoundManager::GetInstance()->AllStop();
		}
	}
}


//=================================================================================================
//		�`�揈��.
//=================================================================================================
void CLoseUI::Draw()
{	
	// UI���ꂼ��̕`�揈��.
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
}


//=================================================================================================
//		�������.
//=================================================================================================
void CLoseUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}