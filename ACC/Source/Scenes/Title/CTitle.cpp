#include "CTitle.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "GameObject/Sprite/2D/UI/CUIObject.h"
#include "Common/DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"

CTitle::CTitle(HWND hWnd)
	: m_pUIs			()
	, m_pSprite2Ds		()

	, m_ScaleDef		(0.f)
	, m_StartScale		(0.f)
	, m_EndScale		(0.f)
	, m_OptionScale		(0.f)
	, m_OpTextDefScale	(0.8f)
	, m_SelectButton	(0)
	, m_SelectOther		(false)
{
	m_hWnd = hWnd;
}

CTitle::~CTitle()
{
	Release();
}

void CTitle::Create()
{
	// UI���ꂼ��̍\����.
	st_TitleText	= { 492,216, 492,216, 492,216,	D3DXVECTOR3(50.f,  150.f, 1.f) };
	st_BestTimeText = { 314,47,  314,47,  314,47,	D3DXVECTOR3(100.f, 500.f, 1.f) };
	st_Button		= { 338,270, 338,270, 169,270,	D3DXVECTOR3(850.f, 160.f, 0.f) }; // Option.
	st_Button2		= { 338,270, 338,270, 169,270,	D3DXVECTOR3(575.f, 400.f, 0.f) }; // Start.
	st_Button3		= { 256,64,  512,64,  256,64,	D3DXVECTOR3(875.f, 500.f, 0.f) }; // End.

	st_ButtonText	= { 154,45,308,45,154,45,		D3DXVECTOR3(665.f, 200.f, 1.f) }; // Start.
	st_ButtonText2	= { 123,43,246,43,123,43,		D3DXVECTOR3(945.f, 510.f, 1.f) }; // End.
	st_ButtonText3	= { 256,64,512,64,256,64,		D3DXVECTOR3(920.f, 300.f, 1.f) }; // Option.
	st_FullScreen	= { WND_W,WND_H,WND_W,WND_W };


	// �t�@�C���p�X�ƃX�v���C�g�f�[�^���܂Ƃ߂ď���.
	SpriteDataList = {
		{_T("Data\\Texture\\other\\Black.png"),			st_FullScreen		},
		{_T("Data\\Texture\\title\\Button.png"),		st_Button3			},
		{_T("Data\\Texture\\title\\TitleButtons.png"),	st_Button2			},
		{_T("Data\\Texture\\title\\TitleButtons.png"),	st_Button			},
		{_T("Data\\Texture\\title\\OptionButton.png"),	st_ButtonText3		},
		{_T("Data\\Texture\\title\\EndButton.png"),		st_ButtonText2		},
		{_T("Data\\Texture\\title\\StartButton.png"),	st_ButtonText		},
		{_T("Data\\Texture\\title\\BestTime.png"),		st_BestTimeText		},
		{_T("Data\\Texture\\title\\TitleFont.png"),		st_TitleText		}
	};
}

HRESULT CTitle::LoadData()
{
	// �e2DSprite�I�u�W�F�N�g�̏����ݒ������.
	for (size_t i = 0; i < SpriteDataList.size(); ++i) {
		m_pSprite2Ds.push_back(new CSprite2D());
		//m_pSprite2Ds[i]->Init(SpriteDataList[i].first.c_str(), SpriteDataList[i].second);
	}

	// �eUI�I�u�W�F�N�g�̏����ݒ������.
	for (size_t i = 0; i < SpriteDataList.size(); ++i) {
		m_pUIs.push_back(new CUIObject());
		m_pUIs[i]->AttachSprite(*m_pSprite2Ds[i]);

		m_pUIs[i]->SetPosition(SpriteDataList[i].second.Pos);
	}

	return S_OK;
}

void CTitle::Release()
{
	for (size_t i = 0; i < SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pUIs[i]);
	}
	for (size_t i = 0; i < SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pSprite2Ds[i]);
	}
}

void CTitle::Init()
{
}

void CTitle::Update()
{
	CKey* Key = CDInput::GetInstance()->CDKeyboard();
	CGamePad* GamePad = CDInput::GetInstance()->CDGamePad();

	CSoundManager::PlayLoop(CSoundManager::BGM_Title);


	//-----------------------------------------------------------------------------------
	// Start�{�^���̔���.
	//-----------------------------------------------------------------------------------
	if (Key->IsKeyDown(DIK_A) || GamePad->GetPadpos().lX <= -STIC_LOW
		|| m_SelectButton == 0 && Key->IsKeyAction(DIK_W)
		|| m_SelectButton == 0 && GamePad->GetPadpos().lY >= STIC_LOW)
	{
		//�I�����ړ�SE���Đ�
		if (m_SelectButton != 1)
		{
			CSoundManager::PlaySE(CSoundManager::enList::SE_SelectMove);
		}

		m_SelectButton = 1;
	}


	//-----------------------------------------------------------------------------------
	// Option�{�^���̔���.
	//-----------------------------------------------------------------------------------
	else if (Key->IsKeyAction(DIK_D) || GamePad->GetPadpos().lX >= STIC_LOW
		|| m_SelectButton == 3 && Key->IsKeyAction(DIK_W)
		|| m_SelectButton == 3 && GamePad->GetPadpos().lY <= -STIC_LOW)
	{

		//�I�����ړ�SE���Đ�
		if (m_SelectButton != 2)
		{
			CSoundManager::PlaySE(CSoundManager::enList::SE_SelectMove);
		}

		m_SelectButton = 2;
	}

	//-----------------------------------------------------------------------------------
	// End�{�^���̔���.
	//-----------------------------------------------------------------------------------
	else if (Key->IsKeyAction(DIK_S) || GamePad->GetPadpos().lY >= STIC_LOW)
	{
		//�I�����ړ�SE���Đ�
		if (m_SelectButton != 3)
		{
			CSoundManager::PlaySE(CSoundManager::enList::SE_SelectMove);
		}

		m_SelectButton = 3;
	}

	// �{�^���n���̍X�V.
	m_SelectOther = false;
	if (!m_SelectOther)
	{
		StartButtonUpdate(Key, GamePad);
	}
	if (!m_SelectOther)
	{
		EndButtonUpdate(Key, GamePad);
	}
	if (!m_SelectOther)
	{
		OptionButtonUpdate(Key, GamePad);
	}
}

void CTitle::Draw()
{
	//UI���ꂼ��̕`�揈��.
	for (size_t i = 0; i < m_pUIs.size(); ++i) {
		m_pUIs[i]->Draw();
	}
}

void CTitle::StartButtonUpdate(CKey* key, CGamePad* pad)
{
	float scale;

	if (m_SelectButton == 1)
	{
		if (m_StartScale < 0.2f)
		{
			m_StartScale += 0.06f;
		}
		scale = 1.f + m_StartScale;

		// �摜�̎�ޕύX.
		m_pUIs[TitleSprite::Button2]->SetPatternNo(1, 0);
		m_pUIs[TitleSprite::ButtonText]->SetPatternNo(1, 0);

		// �X�P�[���l��ύX(Button2�͏㉺���]�̂��߃}�C�i�X).
		m_pUIs[TitleSprite::Button2]->SetScale(scale, -scale, 1.f);
		m_pUIs[TitleSprite::ButtonText]->SetScale(scale, scale, 1.f);

		// �g�厞�ɍ��W�����炵�Z���^�����O����.
		m_pUIs[TitleSprite::Button2]
			->SetPosition(
				st_Button2.Pos.x - (st_Button2.Disp.w * m_StartScale / 2), 
				st_Button2.Pos.y + (st_Button2.Disp.h * m_StartScale / 2), 0.f);

		m_pUIs[TitleSprite::ButtonText]
			->SetPosition(
				st_ButtonText.Pos.x - (st_ButtonText.Disp.w * m_StartScale / 2), 
				st_ButtonText.Pos.y - (st_ButtonText.Disp.h * m_StartScale / 2), 0.f);

		//�Q�[�����C���֑J��.
		if (key->IsKeyAction(DIK_SPACE) || pad->GetPadButtonDown(2))
		{
			// �f�o�b�O�̂��߂̃L�[���͂̋�ł�.
			EmptyInput();
			CSceneManager::GetInstance()->LoadScene(SceneList::Game);

			// �Q�[���X�^�[�gSE���Đ�.
			CSoundManager::PlaySE(CSoundManager::enList::SE_GameStart);
			m_SelectOther = true;
		}
	}
	else
	{
		if (m_StartScale > 0.f)
		{
			m_StartScale -= 0.06f;
		}
		scale = 1.f + m_StartScale;

		// �摜�̎�ޕύX.
		m_pUIs[TitleSprite::ButtonText]->SetPatternNo(0, 0);
		m_pUIs[TitleSprite::Button2]->SetPatternNo(0, 0);

		// �X�P�[���l��ύX(Button2�͏㉺���]�̂��߃}�C�i�X).
		m_pUIs[TitleSprite::Button2]->SetScale(scale, -scale, 1.f);
		m_pUIs[TitleSprite::ButtonText]->SetScale(scale, scale, 1.f);
	}

	// �g�厞�ɍ��W�����炵�Z���^�����O����.
	m_pUIs[TitleSprite::Button2]
		->SetPosition(
		st_Button2.Pos.x - (st_Button2.Disp.w * m_StartScale / 2),
		st_Button2.Pos.y + (st_Button2.Disp.h * m_StartScale / 2), 0.f);

	m_pUIs[TitleSprite::ButtonText]
		->SetPosition(
		st_ButtonText.Pos.x - (st_ButtonText.Disp.w * m_StartScale / 2),
		st_ButtonText.Pos.y - (st_ButtonText.Disp.h * m_StartScale / 2), 0.f);
}

void CTitle::OptionButtonUpdate(CKey* key, CGamePad* pad)
{
	float scale;
	float butscale;

	if (m_SelectButton == 2)
	{
		if (m_OptionScale < 0.2f)
		{
			m_OptionScale += 0.06f;
		}
		scale = m_OpTextDefScale + m_OptionScale;
		butscale = m_OptionScale + 1.f;

		// �摜�̎�ޕύX.
		m_pUIs[TitleSprite::Button]->SetPatternNo(1, 0);
		m_pUIs[TitleSprite::ButtonText3]->SetPatternNo(1, 0);

		// �X�P�[���l��ύX.
		m_pUIs[TitleSprite::Button]->SetScale(butscale, butscale, 1.f);
		m_pUIs[TitleSprite::ButtonText3]->SetScale(scale, scale, 1.f);
	}
	else
	{
		if (m_OptionScale > 0.f)
		{
			m_OptionScale -= 0.06f;
		}
		scale = m_OpTextDefScale + m_OptionScale;
		butscale = m_OptionScale + 1.f;

		// �摜�̎�ޕύX.
		m_pUIs[TitleSprite::Button]->SetPatternNo(0, 0);
		m_pUIs[TitleSprite::ButtonText3]->SetPatternNo(0, 0);

		// �X�P�[���l��ύX.
		m_pUIs[TitleSprite::Button]->SetScale(butscale, butscale, 1.f);
		m_pUIs[TitleSprite::ButtonText3]->SetScale(scale, scale, 1.f);
	}

	// �g�厞�ɍ��W�����炵�Z���^�����O����.
	m_pUIs[TitleSprite::Button]
		->SetPosition(
			st_Button.Pos.x - (st_Button.Disp.w * m_OptionScale / 2),
			st_Button.Pos.y - (st_Button.Disp.h * m_OptionScale / 2), 0.f);

	m_pUIs[TitleSprite::ButtonText3]
		->SetPosition(
			st_ButtonText3.Pos.x - (st_ButtonText3.Disp.w * m_OptionScale / 2),
			st_ButtonText3.Pos.y - (st_ButtonText3.Disp.h * m_OptionScale / 2), 0.f);
}

void CTitle::EndButtonUpdate(CKey* key, CGamePad* pad)
{
	float scale;

	if (m_SelectButton == 3)
	{
		if (m_EndScale < 0.1f)
		{
			m_EndScale += 0.08f;
		}
		scale = 1.f + m_EndScale;

		// �摜�̎�ޕύX.
		m_pUIs[TitleSprite::Button3]->SetPatternNo(1, 0);
		m_pUIs[TitleSprite::ButtonText2]->SetPatternNo(1, 0);

		// �X�P�[���l��ύX.
		m_pUIs[TitleSprite::Button3]->SetScale(scale, scale, 1.f);
		m_pUIs[TitleSprite::ButtonText2]->SetScale(scale, scale, 1.f);

		// �Q�[�����I��������.
		if (key->IsKeyAction(DIK_SPACE) || pad->GetPadButtonDown(2))
		{
			// �A�v���P�[�V�������I������.
			PostMessage(m_hWnd, WM_CLOSE, 0, 0);
			m_SelectOther = true;
		}
	}
	else
	{
		if (m_EndScale > 0.f)
		{
			m_EndScale -= 0.08f;
		}
		scale = 1.f + m_EndScale;

		// �摜�̎�ޕύX.
		m_pUIs[TitleSprite::Button3]->SetPatternNo(0, 0);
		m_pUIs[TitleSprite::ButtonText2]->SetPatternNo(0, 0);

		// �X�P�[���l��ύX.
		m_pUIs[TitleSprite::Button3]->SetScale(scale, scale, 1.f);
		m_pUIs[TitleSprite::ButtonText2]->SetScale(scale, scale, 1.f);
	}

	// �g�厞�ɍ��W�����炵�Z���^�����O����.
	m_pUIs[TitleSprite::Button3]
		->SetPosition(
			st_Button3.Pos.x - (st_Button3.Disp.w * m_EndScale / 2),
			st_Button3.Pos.y - (st_Button3.Disp.h * m_EndScale / 2), 0.f);
	m_pUIs[TitleSprite::ButtonText2]
		->SetPosition(
			st_ButtonText2.Pos.x - (st_ButtonText2.Disp.w * m_EndScale / 2),
			st_ButtonText2.Pos.y - (st_ButtonText2.Disp.h * m_EndScale / 2), 0.f);
}

void CTitle::EmptyInput()
{
	if (GetAsyncKeyState('Q') & 0x0001
		|| GetAsyncKeyState('R') & 0x0001
		|| GetAsyncKeyState('M') & 0x0001
		|| GetAsyncKeyState('C') & 0x0001)
	{
	}
}