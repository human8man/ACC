#include "CUIFade.h"
#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// UI���X�g.
	std::string FadeImage = "Black";
}


//=============================================================================
//		�t�F�[�h�N���X.
//=============================================================================
CUIFade::CUIFade()
	: m_pSprite2D	( nullptr )
	, m_Fading		( false )
	, m_FirstPeak	( false )
	, m_Peak		( false )
	, m_BeforePeak	( false )
	, m_End			( false )
	, m_Peaking		( false )

	, m_FadeAlpha	( 0.f )
	, m_AddAlpha	( 0.f )
	, m_InAddAlpha	( 0.f )
	, m_OutAddAlpha	( 0.f )
	
	, m_PeakCnt		( 0 )
{
}
CUIFade::~CUIFade()
{
}


//=================================================================================================
//		�쐬����.
//=================================================================================================
void CUIFade::Create()
{
	// �C���X�^���X����
	m_pSprite2D = CSpriteManager::GetInstance()->GetSprite(FadeImage);
	m_pUI = std::make_unique<CUIObject>();

	// �摜�f�[�^�̓ǂݍ���
	m_pUI->AttachSprite(m_pSprite2D);
	m_pUI->SetPos(m_pSprite2D->GetSpriteData().Pos);

	// �A���t�@���Z�b�g.
	m_pUI->SetAlpha(m_FadeAlpha);
}


//=================================================================================================
//		�X�V����.
//=================================================================================================
void CUIFade::Update()
{
	m_EndFrame = false;

	// �t�F�[�h���łȂ��ꍇ�؂�.
	if (!m_Fading) { return; }

	// ��x�ł��ߋ��ƌ��݂̃s�[�N��񂪐��̏ꍇ�͕s�ɂ���.
	if ( m_BeforePeak && m_BeforePeak == m_Peak ) { m_FirstPeak = false; }

	// �s�[�N���Ԓ�.
	if (m_Peaking) {
		if (m_PeakCnt > 0.f) {
			m_FadeAlpha = 1.f; // �s�[�N���̓A���t�@�Œ�
			m_PeakCnt -= CTime::GetDeltaTime();
		}
		else {
			m_FadeAlpha -= m_OutAddAlpha;

			// �����ǉ�: �A���t�@��0�ȉ��ɂȂ�����t�F�[�h�I��
			if (m_FadeAlpha <= 0.f) {
				m_FadeAlpha = 0.f;
				m_Fading = false;
				m_Peaking = false;
				if (!m_End) { m_EndFrame = true; }
				m_End = true;
			}
		}
	}
	// �s�[�N�ɑ΂��Ă��Ȃ��ꍇ.
	else {
		m_FadeAlpha += m_InAddAlpha;	//�@�A���t�@���Z.
		// �s�[�N�ɒB�����Ƃ�.
		if ( 1.f < m_FadeAlpha && !m_Peak) {
			m_Peak = m_Peaking = m_FirstPeak = true;
		}
	}

	// �t�F�[�h�I��.
	if (m_FadeAlpha <= 0.f) {
		m_FadeAlpha = 0.f;
		m_Fading = false;
		m_End = true;
	}

	// �O�t���[���̃s�[�N����ۑ�.
	m_BeforePeak = m_Peak;

	// �X�v���C�g�̕��̃A���t�@�ɃZ�b�g.
	m_pUI->SetAlpha( m_FadeAlpha );

	// �X�V.
	m_pUI->Update();
}


//=================================================================================================
//		�`�揈��.
//=================================================================================================
void CUIFade::Draw() 
{
	m_pUI->Draw(); 
}


//=================================================================================================
//		�t�F�[�h�J�n����.
//=================================================================================================
void CUIFade::DoFade(int in, int peak, int out)
{
	m_Peak = m_Peaking = m_End = m_FirstPeak = m_BeforePeak = false;
	m_Fading = true;
	m_FadeAlpha = 0.f;

	m_PeakCnt = peak * CTime::GetDeltaTime();
	
	// �C��/�A�E�g���ɉ��Z����l���Z�o.
	m_InAddAlpha = 1.f / in;
	m_OutAddAlpha = 1.f / out;
}