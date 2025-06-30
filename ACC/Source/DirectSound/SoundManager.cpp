#include "SoundManager.h"


// �T�E���h�ǂݍ��ݗp�̃��X�g�ݒ�
static SoundManager::SoundList s_SoundList[] =
{
	// 10000��MAX�̉���
	// �����Őݒ肷�鉹�ʂ̐��l��MAX���̉��ʂ̐��l
	{ SoundManager::enList::SE_CritHit		,"Data\\Sound\\SE\\CritHit.wav"		, {"SE_CritHit"}	, 9000 },
	{ SoundManager::enList::SE_Hit			,"Data\\Sound\\SE\\Hit.wav"			, {"SE_Hit"}		,10000 },
	{ SoundManager::enList::SE_NoAmmo		,"Data\\Sound\\SE\\NoAmmo.wav"		, {"SE_NoAmmo"}		, 9500 },
	{ SoundManager::enList::SE_Reload		,"Data\\Sound\\SE\\Reload.wav"		, {"SE_Reload"}		, 9500 },
	{ SoundManager::enList::SE_ReloadEnd	,"Data\\Sound\\SE\\ReloadEnd.wav"	, {"SE_ReloadEnd"}	, 9500 },
	{ SoundManager::enList::SE_SelectMove	,"Data\\Sound\\SE\\SelectMove.wav"	, {"SE_SelectMove"}	, 9000 },
	{ SoundManager::enList::SE_Shot			,"Data\\Sound\\SE\\Shot.wav"		, {"SE_Shot"}		, 8250 },
	{ SoundManager::enList::SE_Dash			,"Data\\Sound\\SE\\Dash.wav"		, {"SE_Dash"}		, 9000 },
	{ SoundManager::enList::SE_Jump			,"Data\\Sound\\SE\\Jump.wav"		, {"SE_Jump"}		, 9000 },
	{ SoundManager::enList::SE_Damage		,"Data\\Sound\\SE\\Damage.wav"		, {"SE_Damage"}		, 9000 },
	{ SoundManager::enList::SE_DamageCrit	,"Data\\Sound\\SE\\DamageCrit.wav"	, {"SE_DamageCrit"}	, 9000 },
	{ SoundManager::enList::SE_Lose			,"Data\\Sound\\SE\\Lose.wav"		, {"SE_Lose"}		, 9000 },
	{ SoundManager::enList::SE_Win			,"Data\\Sound\\SE\\Win.wav"			, {"SE_Win"}		, 9000 },
																									  
	{ SoundManager::enList::BGM_Game		,"Data\\Sound\\BGM\\Game.wav"		, {"BGM_Game"}		, 8500 },
	{ SoundManager::enList::BGM_Title		,"Data\\Sound\\BGM\\Title.wav"		, {"BGM_Title"}		, 9000 },
};


SoundManager::SoundManager()
	: m_pSound		()
	, m_MasterVolume(0.8f)
	, m_BGMVolume	(1.f)
	, m_SEVolume	(1.f)
{
}

SoundManager::~SoundManager()
{
	Release();
}


//==============================================================================
//		�쐬����
//==============================================================================
HRESULT SoundManager::Create(Sound** ppSound, std::string filePath, HWND hWnd)
{
	*ppSound = new Sound();

	if (!*ppSound)
	{
		return E_FAIL;	// �������m�ێ��s���̃G���[����
	}
	(*ppSound)->CreateDevice(hWnd);
	return (*ppSound)->LoadData(ConvertStringToLPSTR(filePath));
}


//==============================================================================
//		�f�[�^�Ǎ�����
//==============================================================================
HRESULT SoundManager::Load( HWND hWnd )
{
	m_SoundList.assign(std::begin(s_SoundList), std::end(s_SoundList));

	for (const auto& sound : m_SoundList)
	{
		Sound* pSound = nullptr;
		if (FAILED(SoundManager::GetInstance()->Create(&pSound, sound.path.c_str(), hWnd)))
		{
			Release();
			return E_FAIL;
		}
		m_pSound.push_back(pSound);
	}

	ApplyVolumeSetting();
	return S_OK;
}


//==============================================================================
//		�������
//==============================================================================
void SoundManager::Release()
{
	for (auto& sound : m_pSound)
	{
		delete sound;
		sound = nullptr;
	}
	m_pSound.clear();
}


//==============================================================================
//		���ʂ�K�p������
//==============================================================================
void SoundManager::ApplyVolumeSetting()
{
	for (size_t i = 0; i < m_SoundList.size(); i++)
	{
		if (i < m_pSound.size())
		{
			LONG adjustedVolume;	// �K�p�����邽�߂̕ϐ�

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
				// �G�C���A�X����BGM�����Ă��镨�̉��̒���
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume * m_BGMVolume);
			}
			else if(m_SoundList[i].alias->find("SE") != std::string::npos)
			{
				// �G�C���A�X����SE�����Ă��镨�̉��̒���
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume * m_SEVolume);
			}
			else
			{
				// ����ȊO�̉��̒���
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume);
			}

			//�ݒ肵������Sound�ɓK��
			m_pSound[i]->SetVolume(adjustedVolume);
		}
	}
}


//=============================================================================
//		��x�����Đ�����
//=============================================================================
void SoundManager::Play(enList list)
{
	SoundManager::GetInstance()->m_pSound[list]->Play();
}


//=============================================================================
//		���[�v�Đ�����
//=============================================================================
void SoundManager::PlayLoop(enList list)
{
	SoundManager::GetInstance()->m_pSound[list]->PlayLoop();
}


//=============================================================================
//		��~����
//=============================================================================
void SoundManager::Stop(enList list)
{
	SoundManager::GetInstance()->m_pSound[list]->Stop();
}


//=============================================================================
//		�Đ��ʒu��߂����ɒ�~
//=============================================================================
void SoundManager::MiniStop(enList list)
{
	SoundManager::GetInstance()->m_pSound[list]->MiniStop();
}


//=============================================================================
//		���ׂẲ����~
//=============================================================================
void SoundManager::AllStop()
{
	for (int i = 0;i < enList::max;++i) {
		SoundManager::GetInstance()->m_pSound[i]->Stop();
	}
}


//=============================================================================
//		�Đ��������m�F
//=============================================================================
bool SoundManager::IsPlaying(enList list)
{
	return SoundManager::GetInstance()->m_pSound[list]->IsPlaying();
}


//=============================================================================
//		��ɍŏ�����Đ�����
//=============================================================================
void SoundManager::PlayEx(enList list, I3DL2_ENV_PRESET preset)
{
	SoundManager::GetInstance()->m_pSound[list]->playLoopEx(preset);
}


//=============================================================================
//		���[�v�Đ�
//=============================================================================
void SoundManager::playLoopEx(enList list, I3DL2_ENV_PRESET preset)
{
	SoundManager::GetInstance()->m_pSound[list]->PlayEx(preset);
}


//=============================================================================
//		�p���j���O��ݒ肷��
//=============================================================================
void SoundManager::SetPan(enList list, LONG pan)
{
	SoundManager::GetInstance()->m_pSound[list]->SetPan(pan);
}


//=============================================================================
//		���g����ݒ肷��
//=============================================================================
void SoundManager::SetFrequency(enList list, DWORD freq)
{
	SoundManager::GetInstance()->m_pSound[list]->SetFrequency(freq);
}


//=============================================================================
//		���ʂ�ݒ肷��
//=============================================================================
void SoundManager::SetVolume(enList list, LONG volume)
{
	SoundManager::GetInstance()->m_pSound[list]->SetVolume(volume);
}


//=============================================================================
//		�p���j���O��ݒ肷��
//=============================================================================
LONG SoundManager::GetPan(enList list)
{
	return SoundManager::GetInstance()->m_pSound[list]->GetPan();
}


//=============================================================================
//		���g�����擾����
//=============================================================================
DWORD SoundManager::GetFrequency(enList list)
{
	return SoundManager::GetInstance()->m_pSound[list]->GetFrequency();
}


//=============================================================================
//		���ʂ��擾����
//=============================================================================
LONG SoundManager::GetVolume(enList list)
{
	return SoundManager::GetInstance()->m_pSound[list]->GetVolume();
}


//=============================================================================
//		�G�t�F�N�g�p�����[�^�̎擾
//=============================================================================
void SoundManager::GetPreset(enList list, I3DL2_ENV_PRESET presetNo)
{
	SoundManager::GetInstance()->m_pSound[list]->GetPreset(presetNo);
}


//-----------------------------------------------------------------------------
//		string����LPSTR�֕ϊ�
//-----------------------------------------------------------------------------
LPSTR SoundManager::ConvertStringToLPSTR(const std::string& str)
{
	return const_cast<LPSTR>(str.c_str());
}