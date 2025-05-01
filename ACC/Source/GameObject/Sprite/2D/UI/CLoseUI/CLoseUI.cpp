#include "CLoseUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectSound/CSoundManager.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"


namespace {
	// UICSV�̃p�X.
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	// LoseUI�̃p�X.
	constexpr char LoseImagePath[] = "Data\\Texture\\Lose";
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
	int index = 0;

	// �w�肵���f�B���N�g�����𑖍�.
	for (const auto& entry : std::filesystem::directory_iterator(LoseImagePath)) {
		// ������json�̏ꍇ��蒼��.
		std::string Extension = entry.path().string();
		Extension.erase(0, entry.path().string().rfind("."));
		if (Extension == ".json") continue;

		// �C���X�^���X����.
		m_pSprite2Ds.push_back(new CSprite2D());
		m_pUIs.push_back(new CUIObject());

		// �摜�f�[�^�̓ǂݍ���.
		m_pSprite2Ds[index]->Init(entry.path().string());
		m_pUIs[index]->AttachSprite(m_pSprite2Ds[index]);
		m_pUIs[index]->SetPos(m_pSprite2Ds[index]->GetSpriteData().Pos);
		m_SpritePosList.push_back(m_pUIs[index]->GetPos());
		index++;
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
		if (i == LoseSprite::Black) { m_pUIs[i]->SetAlpha(0.6f); }

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