#include "CSoundManager.h"
#include "Json/CJson.h"

//�ʂɉ��ʂ𒲐��ł���T�E���h�ǂݍ���.
static CSoundManager::SoundList s_SoundList[] =
{
	//10000��MAX�̉���.
	//�����Őݒ肷�鉹�ʂ̐��l��MAX���̉��ʂ̐��l.
	//�����傫�����̂���������������.
		{ CSoundManager::enList::SE_Jump		,"Data\\Sound\\SE\\Jump.wav"		, {"SE_Jump"}		,10000 },
		{ CSoundManager::enList::SE_JumpEnd		,"Data\\Sound\\SE\\JumpEnd.wav"		, {"SE_JumpEnd"}	,10000 },
		{ CSoundManager::enList::SE_JumpPad		,"Data\\Sound\\SE\\JumpPad.wav"		, {"SE_JumpPad"}	,10000 },
		{ CSoundManager::enList::SE_Wire		,"Data\\Sound\\SE\\Wire.wav"		, {"SE_Wire"}		,10000 },
		{ CSoundManager::enList::SE_CanSliding	,"Data\\Sound\\SE\\CanSliding.wav"	, {"SE_CanSliding"}	,10000 },
		{ CSoundManager::enList::SE_Sliding		,"Data\\Sound\\SE\\Sliding.wav"		, {"SE_Sliding"}	,10000 },
		{ CSoundManager::enList::SE_Move		,"Data\\Sound\\SE\\Run.wav"			, {"SE_Move"}		,10000 },
		{ CSoundManager::enList::SE_WallJump	,"Data\\Sound\\SE\\WallJump.wav"	, {"SE_WallJump"}	, 9000 },
		{ CSoundManager::enList::SE_Pause		,"Data\\Sound\\SE\\Pause.wav"		, {"SE_Pause"}		,10000 },
		{ CSoundManager::enList::SE_Decision	,"Data\\Sound\\SE\\Decision.wav"	, {"SE_Dection"}	,10000 },
		{ CSoundManager::enList::SE_GameStart	,"Data\\Sound\\SE\\GameStart.wav"	, {"SE_GameStart"}	,10000 },
		{ CSoundManager::enList::SE_SelectMove	,"Data\\Sound\\SE\\SelectMove.wav"	, {"SE_SelectMove"}	,10000 },
		{ CSoundManager::enList::SE_GameOver	,"Data\\Sound\\SE\\GameOver.wav"	, {"SE_GameOver"}	,10000 },
		{ CSoundManager::enList::SE_GameClear	,"Data\\Sound\\SE\\GameClear.wav"	, {"SE_GameClear"}	,10000 },
		{ CSoundManager::enList::BGM_GameMain	,"Data\\Sound\\BGM\\MainBGM.wav"	, {"BGM_Main"}		, 8500 },
		{ CSoundManager::enList::BGM_Title		,"Data\\Sound\\BGM\\TitleBGM.wav"	, {"BGM_Title"}		, 8500 },
		
#ifdef _DEBUG
		{ CSoundManager::enList::ImGui_Add		,"Data\\Sound\\IMGUI\\Add.wav"		,{"ImGui_Add"}		,10000 },
		{ CSoundManager::enList::ImGui_Delete	,"Data\\Sound\\IMGUI\\Delete.wav"	,{"ImGui_Delete"}	,10000 },
		{ CSoundManager::enList::ImGui_Ground	,"Data\\Sound\\IMGUI\\Ground.wav"	,{"ImGui_Ground"}	,10000 },
		{ CSoundManager::enList::ImGui_JumpPad	,"Data\\Sound\\IMGUI\\JumpPad.wav"	,{"ImGui_JumpPad"}	,10000 },
		{ CSoundManager::enList::ImGui_Laser	,"Data\\Sound\\IMGUI\\Laser.wav"	,{"ImGui_Laser"}	,10000 },
		{ CSoundManager::enList::ImGui_Save		,"Data\\Sound\\IMGUI\\Save.wav"		,{"ImGui_Save"}		,10000 },
		{ CSoundManager::enList::ImGui_Select	,"Data\\Sound\\IMGUI\\Select.wav"	,{"ImGui_Select"}	,10000 },
		{ CSoundManager::enList::ImGui_Wall		,"Data\\Sound\\IMGUI\\Wall.wav"		,{"ImGui_Wall"}		,10000 },
#endif
};

CSoundManager::CSoundManager()
	: m_pSound		()
	, m_MasterVolume(0.8f)
	, m_BGMVolume	(1.0f)
	, m_SEVolume	(1.0f)
{
}

CSoundManager::~CSoundManager()
{
	Release();
}

//�T�E���h�f�[�^�Ǎ��֐�.
bool CSoundManager::Load( HWND hWnd )
{
	m_SoundList.assign(std::begin(s_SoundList), std::end(s_SoundList));

	for (const auto& sound : m_SoundList)
	{
		CSound* pSound = nullptr;
		if (FAILED(CSoundManager::GetInstance()->Create(&pSound, sound.path.c_str(), hWnd)))
		{
			Release();
			return false;
		}
		m_pSound.push_back(pSound);
	}

	ApplyVolumeSetting();
	return true;
}

//�T�E���h�f�[�^����֐�.
void CSoundManager::Release()
{
	for (auto& sound : m_pSound)
	{
		delete sound;
		sound = nullptr;
	}
	m_pSound.clear();
}

HRESULT CSoundManager::Create(CSound** ppSound, std::string filePath, HWND hWnd)
{
	*ppSound = new CSound();

	if (!*ppSound)
	{
		return E_FAIL;	//�������m�ێ��s���̃G���[����.
	}
	(*ppSound)->CreateDevice(hWnd);
	return (*ppSound)->LoadData(ConvertStringToLPSTR(filePath));
}

void CSoundManager::ApplyVolumeSetting()
{
	for (size_t i = 0; i < m_SoundList.size(); i++)
	{
		if (i < m_pSound.size())
		{
			LONG adjustedVolume;	//�K�p�����邽�߂̕ϐ�.

			
			if (m_MasterVolume <= 0.6f)
			{
				m_MasterVolume = 0.0f;
			}
			else if (m_BGMVolume <= 0.6f)
			{
				m_BGMVolume = 0.0f;
			}
			else if (m_SEVolume <= 0.6f)
			{
				m_SEVolume = 0.6f;
			}

			if (m_SoundList[i].alias->find("BGM") != std::string::npos)
			{
				//�G�C���A�X����BGM�����Ă��镨���������o.
				//BGM�p�̉��̒���.
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume * m_BGMVolume);
			}
			else if(m_SoundList[i].alias->find("SE") != std::string::npos)
			{
				//�G�C���A�X����SE�����Ă��镨���������o.
				//SE�p�̉��̒���.
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume * m_SEVolume);
			}
			else
			{
				//����ȊO�����o.
				//ImGui�p�̉��̒����p.
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume);
			}

			//�ݒ肵������Sound�ɓK��.
			m_pSound[i]->SetVolume(adjustedVolume);
		}
	}
}

//--------------------------------------------------------------------
//		string����LPSTR�֕ϊ�.
//--------------------------------------------------------------------
LPSTR CSoundManager::ConvertStringToLPSTR(const std::string& str)
{
    return const_cast<LPSTR>(str.c_str());
}
