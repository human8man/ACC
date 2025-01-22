#include "CSoundManager.h"

//個別に音量を調整できるサウンド読み込み.
static CSoundManager::SoundList s_SoundList[] =
{
	//10000がMAXの音量.
	//ここで設定する音量の数値はMAX時の音量の数値.
	//音が大きいものだけを小さくする.
	{ CSoundManager::enList::SE_CritHit		,"Data\\Sound\\SE\\CritHit.wav"		, {"SE_CritHit"}	,10000 },
	{ CSoundManager::enList::SE_Hit			,"Data\\Sound\\SE\\Hit.wav"			, {"SE_Hit"}		,10000 },
	{ CSoundManager::enList::SE_NoAmmo		,"Data\\Sound\\SE\\NoAmmo.wav"		, {"SE_NoAmmo"}		, 9000 },
	{ CSoundManager::enList::SE_Reload		,"Data\\Sound\\SE\\Reload.wav"		, {"SE_Reload"}		, 9000 },
	{ CSoundManager::enList::SE_ReloadEnd	,"Data\\Sound\\SE\\ReloadEnd.wav"	, {"SE_ReloadEnd"}	, 9000 },
	{ CSoundManager::enList::SE_SelectMove	,"Data\\Sound\\SE\\SelectMove.wav"	, {"SE_SelectMove"}	, 9000 },
	{ CSoundManager::enList::SE_Shot		,"Data\\Sound\\SE\\Shot.wav"		, {"SE_Shot"}		,10000 },
	{ CSoundManager::enList::SE_Dash		,"Data\\Sound\\SE\\Dash.wav"		, {"SE_Dash"}		, 9000 },
	{ CSoundManager::enList::SE_Jump		,"Data\\Sound\\SE\\Jump.wav"		, {"SE_Jump"}		, 9000 },
	{ CSoundManager::enList::SE_Damage		,"Data\\Sound\\SE\\Damage.wav"		, {"SE_Damage"}		,10000 },
	{ CSoundManager::enList::SE_DamageCrit	,"Data\\Sound\\SE\\DamageCrit.wav"	, {"SE_DamageCrit"}	,10000 },
	{ CSoundManager::enList::SE_Lose		,"Data\\Sound\\SE\\Lose.wav"		, {"SE_Lose"}		,10000 },
	{ CSoundManager::enList::SE_Win			,"Data\\Sound\\SE\\Win.wav"			, {"SE_Win"}		,10000 },
																									  
	{ CSoundManager::enList::BGM_Game		,"Data\\Sound\\BGM\\Game.wav"		, {"BGM_Game"}		, 9000 },
	{ CSoundManager::enList::BGM_Title		,"Data\\Sound\\BGM\\Title.wav"		, {"BGM_Title"}		, 9000 },
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

//サウンドデータ読込関数.
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

//サウンドデータ解放関数.
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
		return E_FAIL;	//メモリ確保失敗時のエラー処理.
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
			LONG adjustedVolume;	//適用させるための変数.

			
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
				//エイリアス名にBGMがついている物だけを検出.
				//BGM用の音の調整.
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume * m_BGMVolume);
			}
			else if(m_SoundList[i].alias->find("SE") != std::string::npos)
			{
				//エイリアス名にSEがついている物だけを検出.
				//SE用の音の調整.
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume * m_SEVolume);
			}
			else
			{
				//それ以外を検出.
				//ImGui用の音の調整用.
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume);
			}

			//設定した音をSoundに適応.
			m_pSound[i]->SetVolume(adjustedVolume);
		}
	}
}

//--------------------------------------------------------------------
//		stringからLPSTRへ変換.
//--------------------------------------------------------------------
LPSTR CSoundManager::ConvertStringToLPSTR(const std::string& str)
{
    return const_cast<LPSTR>(str.c_str());
}
