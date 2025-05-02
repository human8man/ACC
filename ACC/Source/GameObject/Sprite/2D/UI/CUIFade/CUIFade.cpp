#include "CUIFade.h"
#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	// json �Ǎ��ɂ����p�X�̐ݒ�.
	constexpr char FadeImagePath[] = "Data\\Texture\\other";

	// UI���X�g.
	std::string FadeImage = "Black";

}


//=============================================================================
//		�t�F�[�h�N���X.
//=============================================================================
CUIFade::CUIFade()
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
CUIFade::~CUIFade()
{
}


//=================================================================================================
//		�쐬����.
//=================================================================================================
void CUIFade::Create()
{
	// �C���X�^���X����
	m_pSprite2Ds.push_back(new CSprite2D);
	m_pUIs.push_back(new CUIObject());
	m_pSprite2Ds.back() = CSpriteManager::GetInstance()->GetSprite(FadeImage);

	// �摜�f�[�^�̓ǂݍ���
	m_pUIs.back()->AttachSprite(m_pSprite2Ds.back());
	m_pUIs.back()->SetPos(m_pSprite2Ds.back()->GetSpriteData().Pos);
	m_SpritePosList.push_back(m_pUIs.back()->GetPos());
}


//=================================================================================================
//		�X�V����.
//=================================================================================================
void CUIFade::Update()
{
	// �t�F�[�h�J�n.
	if ( m_FadeStart ) {
		m_FadeStart = false;
		m_Fading = true;
		m_FadePeak = false;
		m_FadeEnd = false;
		m_Peaking = false;

		m_AddAlpha = m_AAVMAXVAL;
	}

	// �t�F�[�h��.
	if ( m_Fading ) {
		// �s�[�N���Ԓ�.
		if ( m_Peaking ) {
			m_FadeAlpha = 1.f;	// �A���t�@��1�ŌŒ�.
			m_PeakCnt--;

			if ( m_PeakCnt <= 0 ) {
				m_AddAlpha = -m_AAVMAXVAL;
				m_Peaking = false;
			}
		}
		else {
			m_FadeAlpha += m_AddAlpha;
		}

		// �t�F�[�h�̏���ɂ��Ă��Ȃ��ꍇ.
		if ( !m_FadePeak ) {
			// �A���t�@�̍ő�l�𒴂����ꍇ.
			if ( 1.f < m_FadeAlpha )
			{
				m_FadePeak = true;
				m_Peaking = true;
			}
		}
		else {
			m_FadePeak = false;
		}

		// �t�F�[�h�I��.
		if ( m_FadeAlpha <= 0 ) {
			m_FadeAlpha = 0.f;
			m_Fading = false;
			m_FadeEnd = true;
		}
	}

	// �X�v���C�g�̕��̃A���t�@�ɃZ�b�g.
	m_pUIs[0]->SetAlpha( m_FadeAlpha );

	// �X�V.
	m_pUIs[0]->Update();
}


//=================================================================================================
//		�`�揈��.
//=================================================================================================
void CUIFade::Draw()
{
	m_pUIs[0]->Draw();
}