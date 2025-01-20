#include "CGameUI.h"
#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"

namespace {
	constexpr char FadeImagePath[] = "Data\\Texture\\Game";
}

//======================================================================================================================
//		�Q�[��UI�N���X.
//======================================================================================================================
CGameUI::CGameUI()
	: m_FadeStart	( false )
	, m_Fading		( false )
	, m_FadePeak	( false )
	, m_FadeEnd		( false )
	, m_Peaking		( false )

	, m_FadeAlpha	( 0.f )
	, m_AddAlpha	( 0.f )
	, m_AAVMAXVAL	( 0.02f )
	, m_PeakCnt		( 0 )
{
}

CGameUI::~CGameUI()
{
	Release();
}

//======================================================================================================================
//		�\�z�֐�.
//======================================================================================================================
void CGameUI::Create()
{
	int index = 0;

	// �w�肵���f�B���N�g�����𑖍�.
	for (const auto& entry : std::filesystem::directory_iterator(FadeImagePath)) {
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


//======================================================================================================================
//		�X�V.
//======================================================================================================================
void CGameUI::Update()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i ) 
	{
	}
}


//======================================================================================================================
//		�`��.
//======================================================================================================================
void CGameUI::Draw()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i )
	{
		m_pUIs[i]->Draw();
	}
}


//======================================================================================================================
//		�J��.
//======================================================================================================================
void CGameUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pUIs[i]);
	}
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pSprite2Ds[i]);
	}
}
