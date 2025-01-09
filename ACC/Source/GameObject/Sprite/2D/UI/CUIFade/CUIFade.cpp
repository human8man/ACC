#include "CUIFade.h"
#include "DirectX/CDirectX11.h"	// Depth�Ŏg�p.
#include "DirectSound/CSoundManager.h"


namespace {
	constexpr char FadeImagePath[] = "Data\\Texture\\other";
}

//======================================================================================================================
//		�t�F�[�h�N���X.
//======================================================================================================================
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
	Release();
}

//======================================================================================================================
//		�\�z�֐�.
//======================================================================================================================
void CUIFade::Create()
{
	int index = 0;

	// �w�肵���f�B���N�g�����𑖍�.
	for (const auto& entry : std::filesystem::directory_iterator(FadeImagePath)) {
		// json�̏ꍇ��蒼��.
		std::string Extension = entry.path().string();
		Extension.erase(0, entry.path().string().rfind("."));
		if (Extension == ".json") continue;

		m_pSprite2Ds.push_back(new CSprite2D());
		m_pUIs.push_back(new CUIObject());

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
void CUIFade::Update()
{
	// �����ǉ�����邩������Ȃ��摜�p��for.
	for ( size_t i = 0; i < m_pUIs.size(); ++i ) 
	{
		//------------------------------------------
		//		���摜�̏ꍇ.
		//------------------------------------------
		if ( i == FadeSprite::Black )
		{
			// �t�F�[�h�J�n.
			if ( m_FadeStart ) {
				m_FadeStart = false;
				m_Fading	= true;
				m_FadePeak	= false;
				m_FadeEnd	= false;
				m_Peaking = false;

				m_AddAlpha	= m_AAVMAXVAL;
			}

			// �t�F�[�h��.
			if( m_Fading ) {
				CSoundManager::Stop(CSoundManager::SE_Move);

				// �s�[�N���Ԓ�.
				if ( m_Peaking ) {
					m_FadeAlpha = 1.f;	// �A���t�@��1�ŌŒ�.
					m_PeakCnt--;

					if (m_PeakCnt <= 0) {
						m_AddAlpha = -m_AAVMAXVAL;
						m_Peaking  = false;
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
						m_Peaking  = true;
					}
				}
				else {
					m_FadePeak = false;
				}

				// �t�F�[�h�I��.
				if ( m_FadeAlpha <= 0 ) {
					m_FadeAlpha = 0.f;
					m_Fading	= false;
					m_FadeEnd	= true;
				}
			}
		}

		// �X�v���C�g�̕��̃A���t�@�ɃZ�b�g.
		m_pUIs[i]->SetAlpha( m_FadeAlpha );

		// �X�V.
		m_pUIs[i]->Update();
	}
}


//======================================================================================================================
//		�`��.
//======================================================================================================================
void CUIFade::Draw()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i )
	{
		// ���摜�ȊO�̕`������Ȃ�
		if (i != FadeSprite::Black) { continue; }
		m_pUIs[i]->Draw();
	}
}


//======================================================================================================================
//		�J��.
//======================================================================================================================
void CUIFade::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pUIs[i]);
	}
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pSprite2Ds[i]);
	}
}
