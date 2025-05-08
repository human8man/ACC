#include "CGameUI.h"

#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// UICSV�̃p�X.
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	// UI���X�g.
	std::vector<std::string> ImageList = {
		"Bullets",
		"Crosshair",
		"HP",
		"LowHP",
		"Reload",
		"AutoAim",
		"Homing",
		"WallHack"
	};
}


//=================================================================================================
//		�Q�[��UI�N���X.
//=================================================================================================
CGameUI::CGameUI()
	: m_HP				( 0 )
	, m_HPMax			( 0 )
	, m_Ammo			( 0 )
	, m_HitKind			( 0 )
	, m_ReloadTime		( 0.f )
	, m_ViewHitTime		( 0.f )
	, m_ViewHitTimeMax	( CTime::GetInstance()->GetDeltaTime() * 60.f )
	, m_AutoAim			( false )
	, m_Homing			( false )
	, m_WallHack		( false )
{
	// �L�����N�^�[CSV�̏��ۑ��p.
	std::unordered_map<std::string, std::string> m_StateList;
	// �L�����N�^�[CSV�̏��擾.
	m_StateList = FileManager::CSVLoad(UICSVPath);

	// ��łȂ��ꍇ�́A�O���Œ�������ׂ��ϐ��̒l�����Ă���.
	if (!m_StateList.empty())
	{
		m_ViewHitTimeMax = StrToFloat(m_StateList["Game_HitViewTime"]) * CTime::GetInstance()->GetDeltaTime();
	}
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
//		�X�V.
//=================================================================================================
void CGameUI::Update(std::unique_ptr<CPlayer>& chara)
{
	// �J�E���g����.
	if (m_ViewHitTime >= 0.f) { m_ViewHitTime -= CTime::GetInstance()->GetDeltaTime(); }

	// �v���C���[�̍U�����������Ă����ꍇ.
	if (chara->GetHit()) {
		// �����̎�ނ�ݒ�.
		m_HitKind = chara->GetHitKind();
		m_ViewHitTime = m_ViewHitTimeMax;
	}

	// �Q�[�����UI�ɕK�v�ȏ��̐ݒ�.
	m_Homing		= chara->GetHoming();
	m_WallHack		= chara->GetWallHack();
	m_AutoAim		= chara->GetAutoAim();
	m_ReloadTime	= chara->GetReloadTime();
	m_HP			= chara->GetCharaInfo().HP;
	m_Ammo			= chara->GetCharaInfo().Ammo;
	m_HPMax			= chara->GetCharaInfo().MaxHP;
}


//=================================================================================================
//		�`��.
//=================================================================================================
void CGameUI::Draw()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i ) {
		std::string spritename = m_pUIs[i]->GetSpriteData().Name;
		
		// �eUI�̕`��ݒ�.
		if( spritename == "Bullets" ) {
			// �����[�h���Ă����ꍇ�͕`�悵�Ȃ�.
			if (m_ReloadTime >= 0.f) { continue; }
			m_pUIs[i]->SetPatternNo(m_Ammo, 0);
		}

		// �N���X�w�A�̕`��ݒ�.
		if ( spritename == "Crosshair" ) {
			if (m_ViewHitTime > 0.f) {
				m_pUIs[i]->SetPatternNo(m_HitKind, 0);
			}
			else {
				m_pUIs[i]->SetPatternNo(0, 0);
			}
		}

		// HPUI�̕`��ݒ�.
		if ( spritename == "HP" ) {
			if (m_HP < 0) { m_HP = 0; }
			m_pUIs[i]->SetPatternNo(m_HP, 0);
		}

		// LOWHPUI�̕`��ݒ�.
		if ( spritename == "LowHP" ) {
			// HP�������ȏ�̏ꍇ�͕`�悵�Ȃ�.
			if (m_HP > m_HPMax / 2) { continue; }
		}

		// �����[�hUI�̕`��ݒ�.
		if ( spritename == "Reload" ) {
			// �����[�h���Ă��Ȃ��ꍇ�͕`�悵�Ȃ�.
			if (m_ReloadTime <= 0.f) { continue; }
		}

		// �`�[�g�n��UI�`��ݒ�.
		if (spritename == "AutoAim")	{ m_pUIs[i]->SetPatternNo(m_AutoAim, 0); }
		if (spritename == "Homing")		{ m_pUIs[i]->SetPatternNo(m_Homing, 0); }
		if (spritename == "WallHack")	{ m_pUIs[i]->SetPatternNo(m_WallHack, 0); }

		m_pUIs[i]->Draw();
	}
}


//=================================================================================================
//		�������.
//=================================================================================================
void CGameUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}