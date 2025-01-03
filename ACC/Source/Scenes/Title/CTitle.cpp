#include "CTitle.h"

#include "Scenes/SceneManager/CSceneManager.h"
#include "GameObject/Sprite/2D/UI/CUIObject.h"
#include "Common/DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"

namespace {
	constexpr char TitleImagePath[] = "Data\\Texture\\title";
	using State_map = std::unordered_map<std::string, CUIObject*>;
}

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
	// UI���ꂼ��̍\����(a~z�̏��ɐ錾).
	// m_SpritePosList = {
	//	D3DXVECTOR3(100.f, 500.f, 1.f),	// BestTime.
	//	D3DXVECTOR3( ZEROVEC3 ),		// Black.

	//	D3DXVECTOR3(875.f, 500.f, 0.f),	// Button3(End).
	//	D3DXVECTOR3(945.f, 510.f, 1.f),	// ButtonText2(End).

	//	D3DXVECTOR3(920.f, 300.f, 1.f),	// ButtonText3(Option).
	//	D3DXVECTOR3(665.f, 200.f, 1.f),	// ButtonText(Start).

	//	D3DXVECTOR3(575.f, 400.f, 0.f),	// Button2(Start).
	//	D3DXVECTOR3(850.f, 160.f, 0.f),	// Button(Option).

	//	D3DXVECTOR3(50.f,  150.f, 1.f),	// Title.

	//};
}

HRESULT CTitle::LoadData()
{
	int index = 0;


	try 
	{
		// �w�肵���f�B���N�g�����𑖍�.
		for (const auto& entry : std::filesystem::directory_iterator(TitleImagePath)) {
			// json�̏ꍇ��蒼��.
			std::string Extension = entry.path().string();
			Extension.erase(0, entry.path().string().rfind("."));
			if (Extension == ".json") continue;

			m_pSprite2Ds.push_back(new CSprite2D());
			m_pUIs.push_back(new CUIObject());

			m_pSprite2Ds[index]->Init(entry.path().string());
			m_pUIs[index]->AttachSprite(*m_pSprite2Ds[index]);
			m_pUIs[index]->SetPosition(m_pSprite2Ds[index]->GetSpriteData().Pos);
			m_SpritePosList.push_back(m_pUIs[index]->GetPosition());
			index++;
		}
	}
	catch (const std::exception& e) { return E_FAIL; }


	return S_OK;
}

void CTitle::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
		SAFE_DELETE(m_pUIs[i]);
	}
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) {
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
	int sp1 = TitleSprite::Button2;
	int sp2 = TitleSprite::ButtonText;

	D3DXVECTOR2 sp1_data = { m_pSprite2Ds[sp1]->GetSpriteData().Disp.w,m_pSprite2Ds[sp1]->GetSpriteData().Disp.h };
	D3DXVECTOR2 sp2_data = { m_pSprite2Ds[sp2]->GetSpriteData().Disp.w,m_pSprite2Ds[sp2]->GetSpriteData().Disp.h };

	if (m_SelectButton == 1)
	{
		if (m_StartScale < 0.2f)
		{
			m_StartScale += 0.06f;
		}
		scale = 1.f + m_StartScale;

		// �摜�̎�ޕύX.
		m_pUIs[sp1]->SetPatternNo(1, 0);
		m_pUIs[sp2]->SetPatternNo(1, 0);

		// �X�P�[���l��ύX(Button2�͏㉺���]�̂��߃}�C�i�X).
		m_pUIs[sp1]->SetScale(scale, -scale, 1.f);
		m_pUIs[sp2]->SetScale(scale,  scale, 1.f);

		// �g�厞�ɍ��W�����炵�Z���^�����O����.
		m_pUIs[sp1]
			->SetPosition(
				m_SpritePosList[sp1].x - (sp1_data.x * m_StartScale / 2),
				m_SpritePosList[sp1].y + (sp1_data.y * m_StartScale / 2), 0.f);

		m_pUIs[sp2]
			->SetPosition(
				m_SpritePosList[sp2].x - (sp2_data.x * m_StartScale / 2),
				m_SpritePosList[sp2].y - (sp2_data.y * m_StartScale / 2), 0.f);

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
		m_pUIs[sp2]->SetPatternNo(0, 0);
		m_pUIs[sp1]->SetPatternNo(0, 0);

		// �X�P�[���l��ύX(Button2�͏㉺���]�̂��߃}�C�i�X).
		m_pUIs[sp1]->SetScale(scale, -scale, 1.f);
		m_pUIs[sp2]->SetScale(scale, scale, 1.f);
	}

	// �g�厞�ɍ��W�����炵�Z���^�����O����.
	m_pUIs[sp1]
		->SetPosition(
			m_SpritePosList[sp1].x - (sp1_data.x * m_StartScale / 2),
			m_SpritePosList[sp1].y + (sp1_data.y * m_StartScale / 2), 0.f);

	m_pUIs[sp2]
		->SetPosition(
			m_SpritePosList[sp2].x - (sp2_data.x * m_StartScale / 2),
			m_SpritePosList[sp2].y - (sp2_data.y * m_StartScale / 2), 0.f);
}

void CTitle::OptionButtonUpdate(CKey* key, CGamePad* pad)
{
	float scale;
	float butscale;
	int sp1 = TitleSprite::Button;
	int sp2 = TitleSprite::ButtonText3;

	D3DXVECTOR2 sp1_data = { m_pSprite2Ds[sp1]->GetSpriteData().Disp.w,m_pSprite2Ds[sp1]->GetSpriteData().Disp.h };
	D3DXVECTOR2 sp2_data = { m_pSprite2Ds[sp2]->GetSpriteData().Disp.w,m_pSprite2Ds[sp2]->GetSpriteData().Disp.h };

	if (m_SelectButton == 2)
	{
		if (m_OptionScale < 0.2f)
		{
			m_OptionScale += 0.06f;
		}
		scale = m_OpTextDefScale + m_OptionScale;
		butscale = m_OptionScale + 1.f;

		// �摜�̎�ޕύX.
		m_pUIs[sp1]->SetPatternNo(1, 0);
		m_pUIs[sp2]->SetPatternNo(1, 0);

		// �X�P�[���l��ύX.
		m_pUIs[sp1]->SetScale(butscale, butscale, 1.f);
		m_pUIs[sp2]->SetScale(scale, scale, 1.f);
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
		m_pUIs[sp1]->SetPatternNo(0, 0);
		m_pUIs[sp2]->SetPatternNo(0, 0);

		// �X�P�[���l��ύX.
		m_pUIs[sp1]->SetScale(butscale, butscale, 1.f);
		m_pUIs[sp2]->SetScale(scale, scale, 1.f);
	}

	// �g�厞�ɍ��W�����炵�Z���^�����O����.
	m_pUIs[sp1]
		->SetPosition(
			m_SpritePosList[sp1].x - (sp1_data.x * m_OptionScale / 2),
			m_SpritePosList[sp1].y - (sp1_data.y * m_OptionScale / 2), 0.f);

	m_pUIs[sp2]
		->SetPosition(
			m_SpritePosList[sp2].x - (sp2_data.x * m_OptionScale / 2),
			m_SpritePosList[sp2].y - (sp2_data.y * m_OptionScale / 2), 0.f);
}

void CTitle::EndButtonUpdate(CKey* key, CGamePad* pad)
{
	float scale;
	int sp1 = TitleSprite::Button3;
	int sp2 = TitleSprite::ButtonText2;

	D3DXVECTOR2 sp1_data = { m_pSprite2Ds[sp1]->GetSpriteData().Disp.w,m_pSprite2Ds[sp1]->GetSpriteData().Disp.h };
	D3DXVECTOR2 sp2_data = { m_pSprite2Ds[sp2]->GetSpriteData().Disp.w,m_pSprite2Ds[sp2]->GetSpriteData().Disp.h };

	if (m_SelectButton == 3)
	{
		if (m_EndScale < 0.1f)
		{
			m_EndScale += 0.08f;
		}
		scale = 1.f + m_EndScale;

		// �摜�̎�ޕύX.
		m_pUIs[sp1]->SetPatternNo(1, 0);
		m_pUIs[sp2]->SetPatternNo(1, 0);

		// �X�P�[���l��ύX.
		m_pUIs[sp1]->SetScale(scale, scale, 1.f);
		m_pUIs[sp2]->SetScale(scale, scale, 1.f);

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
		m_pUIs[sp1]->SetPatternNo(0, 0);
		m_pUIs[sp2]->SetPatternNo(0, 0);

		// �X�P�[���l��ύX.
		m_pUIs[sp1]->SetScale(scale, scale, 1.f);
		m_pUIs[sp2]->SetScale(scale, scale, 1.f);
	}

	// �g�厞�ɍ��W�����炵�Z���^�����O����.
	m_pUIs[sp1]
		->SetPosition(
			m_SpritePosList[sp1].x - (sp1_data.x * m_EndScale / 2),
			m_SpritePosList[sp1].y - (sp1_data.y * m_EndScale / 2), 0.f);
	m_pUIs[sp2]
		->SetPosition(
			m_SpritePosList[sp2].x  - (sp2_data.x * m_EndScale / 2),
			m_SpritePosList[sp2].y  - (sp2_data.y * m_EndScale / 2), 0.f);
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