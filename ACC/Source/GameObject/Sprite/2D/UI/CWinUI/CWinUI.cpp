#include "CWinUI.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "GameObject/Sprite/2D/UI/CUIObject.h"
#include "Common/DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"

#include "Common/Time/CTime.h"
#include "Common/Easing/Easing.h"


namespace {
	// WinUI�̃p�X.
	constexpr char WinImagePath[] = "Data\\Texture\\Win";
}


//=================================================================================================
//		�^�C�g���N���X.
//=================================================================================================
CWinUI::CWinUI()
	: m_Light			()
	, m_mView			()
	, m_mProj			()
	, m_SpriteDataList	()
	, m_SpritePosList	()
	, m_pUIs			()
	, m_pSprite2Ds		()

	, m_SpawnTimeMax	( CTime::GetInstance()->GetDeltaTime() * 300.f )
	, m_SpawnTime		( m_SpawnTimeMax )
{
	m_Light.vDirection = D3DXVECTOR3(1.f, 5.f, 0.f);	//���C�g����.
}

CWinUI::~CWinUI()
{
	Release();
}


//=================================================================================================
//		�\�z�֐�.
//=================================================================================================
void CWinUI::Create()
{
	int index = 0;

	// �w�肵���f�B���N�g�����𑖍�.
	for (const auto& entry : std::filesystem::directory_iterator(WinImagePath)) {
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
HRESULT CWinUI::LoadData()
{
	return S_OK;
}


//=================================================================================================
//		�J��.
//=================================================================================================
void CWinUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}


//=================================================================================================
//		������.
//=================================================================================================
void CWinUI::Init()
{
}


//=================================================================================================
//		�X�V.
//=================================================================================================
void CWinUI::Update()
{
	m_SpawnTime -= CTime::GetInstance()->GetDeltaTime();

	//----------------------------------------------------------------------------
	//		���ꂼ���UI�̍X�V.
	//----------------------------------------------------------------------------
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		// �w�i�𓧉߂�����.
		if (i == WinSprite::Black) { 
			m_pUIs[i]->SetAlpha(0.4f);
		}

		if(m_SpawnTime < 0.f){
			// �Q�[�����J�n����.
			CSceneManager::GetInstance()->LoadScene(SceneList::Title);
			CSoundManager::GetInstance()->Stop(CSoundManager::enList::BGM_Game);
		}
	}
}


//=================================================================================================
//		�`��.
//=================================================================================================
void CWinUI::Draw()
{	
	// UI���ꂼ��̕`�揈��.
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		m_pUIs[i]->Draw();
	}
}