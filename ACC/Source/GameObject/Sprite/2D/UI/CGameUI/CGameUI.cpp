#include "CGameUI.h"
#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"
#include "Common/Time/CTime.h"
#include "Common/Easing/Easing.h"

namespace {
	constexpr char FadeImagePath[] = "Data\\Texture\\Game";
}

//=================================================================================================
//		�Q�[��UI�N���X.
//=================================================================================================
CGameUI::CGameUI()
	: m_Ammo			( 0 )
	, m_ReloadTime		( 0.f )
	, m_ViewHitTime		( 0.f )
	, m_ViewHitTimeMax	( CTime::GetInstance()->GetDeltaTime() * 60.f )
{
}

CGameUI::~CGameUI()
{
	Release();
}

//=================================================================================================
//		�\�z�֐�.
//=================================================================================================
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


//=================================================================================================
//		�X�V.
//=================================================================================================
void CGameUI::Update()
{
	// �J�E���g����.
	if (m_ViewHitTime >= 0.f) { m_ViewHitTime -= CTime::GetInstance()->GetDeltaTime(); }

}


//=================================================================================================
//		�`��.
//=================================================================================================
void CGameUI::Draw()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i ) {

		// �eUI�̕`��ݒ�.
		if(i == GameSprite::Bullets) {
			// �����[�h���Ă����ꍇ�͕`�悵�Ȃ�.
			if (m_ReloadTime >= 0.f) { continue; }
			m_pUIs[i]->SetPatternNo(m_Ammo, 0);
		}

		// �N���X�w�A�̕`��ݒ�.
		if (i == GameSprite::Crosshair) {
			if (m_ViewHitTime > 0.f) {
				m_pUIs[i]->SetPatternNo(m_HitKind, 0);
			}
			else {
				m_pUIs[i]->SetPatternNo(0, 0);
			}
		}

		// �����[�hUI�̕`��ݒ�.
		if (i == GameSprite::Reload) {
			// �����[�h���Ă��Ȃ��ꍇ�͕`�悵�Ȃ�.
			if (m_ReloadTime <= 0.f) { continue; }
		}


		m_pUIs[i]->Draw();
	}
}


//=================================================================================================
//		�J��.
//=================================================================================================
void CGameUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pUIs[i]);
	}
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pSprite2Ds[i]);
	}
}


//=================================================================================================
//		Hit���̐ݒ�.
//=================================================================================================
void CGameUI::SetHit(int hit)
{
	m_HitKind = hit; 
	m_ViewHitTime = m_ViewHitTimeMax;
}
