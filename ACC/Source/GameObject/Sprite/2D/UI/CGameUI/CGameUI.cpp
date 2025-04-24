#include "CGameUI.h"

#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"


namespace {
	// UICSV�̃p�X.
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	// �摜�̃p�X.
	constexpr char GameImagePath[] = "Data\\Texture\\Game";
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
	int index = 0;
	
	// �w�肵���f�B���N�g�����𑖍�.
	for (const auto& entry : std::filesystem::directory_iterator(GameImagePath)) {
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
		// �G�t���[���͕ʂŏ���.
		if (i == GameSprite::EnemyFrame) { continue; }

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

		// HPUI�̕`��ݒ�.
		if (i == GameSprite::HP) {
			if (m_HP < 0) { m_HP = 0; }
			m_pUIs[i]->SetPatternNo(m_HP, 0);
		}

		// LOWHPUI�̕`��ݒ�.
		if (i == GameSprite::LowHP) {
			// HP�������ȏ�̏ꍇ�͕`�悵�Ȃ�.
			if (m_HP > m_HPMax / 2) { continue; }
		}

		// �����[�hUI�̕`��ݒ�.
		if (i == GameSprite::Reload) {
			// �����[�h���Ă��Ȃ��ꍇ�͕`�悵�Ȃ�.
			if (m_ReloadTime <= 0.f) { continue; }
		}

		// �`�[�g�n��UI�`��ݒ�.
		if (i == GameSprite::AutoAim) {m_pUIs[i]->SetPatternNo(m_AutoAim, 0);}
		if (i == GameSprite::Homing)  {m_pUIs[i]->SetPatternNo(m_Homing, 0);}
		if (i == GameSprite::WallHack)  {m_pUIs[i]->SetPatternNo(m_WallHack, 0);}

		m_pUIs[i]->Draw();
	}
}


//=================================================================================================
//		�������.
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