#include "CSoundManager.h"


// サウンド読み込み用のリスト設定.
static CSoundManager::SoundList s_SoundList[] =
{
	// 10000がMAXの音量.
	// ここで設定する音量の数値はMAX時の音量の数値.
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


//==============================================================================
//		サウンドマネージャークラス.
//==============================================================================
CSoundManager::CSoundManager()
	: m_pSound		()
	, m_MasterVolume(0.8f)
	, m_BGMVolume	(1.f)
	, m_SEVolume	(1.f)
{
}

CSoundManager::~CSoundManager()
{
	Release();
}


//==============================================================================
//		作成処理.
//==============================================================================
HRESULT CSoundManager::Create(CSound** ppSound, std::string filePath, HWND hWnd)
{
	*ppSound = new CSound();

	if (!*ppSound)
	{
		return E_FAIL;	// メモリ確保失敗時のエラー処理.
	}
	(*ppSound)->CreateDevice(hWnd);
	return (*ppSound)->LoadData(ConvertStringToLPSTR(filePath));
}


//==============================================================================
//		データ読込処理.
//==============================================================================
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


//==============================================================================
//		解放処理.
//==============================================================================
void CSoundManager::Release()
{
	for (auto& sound : m_pSound)
	{
		delete sound;
		sound = nullptr;
	}
	m_pSound.clear();
}


//==============================================================================
//		音量を適用させる.
//==============================================================================
void CSoundManager::ApplyVolumeSetting()
{
	for (size_t i = 0; i < m_SoundList.size(); i++)
	{
		if (i < m_pSound.size())
		{
			LONG adjustedVolume;	// 適用させるための変数.

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
				// エイリアス名にBGMがついている物の音の調整.
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume * m_BGMVolume);
			}
			else if(m_SoundList[i].alias->find("SE") != std::string::npos)
			{
				// エイリアス名にSEがついている物の音の調整.
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume * m_SEVolume);
			}
			else
			{
				// それ以外の音の調整.
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume);
			}

			//設定した音をSoundに適応.
			m_pSound[i]->SetVolume(adjustedVolume);
		}
	}
}


//=============================================================================
//		一度だけ再生する.
//=============================================================================
void CSoundManager::Play(enList list)
{
	CSoundManager::GetInstance()->m_pSound[list]->Play();
}


//=============================================================================
//		ループ再生する.
//=============================================================================
void CSoundManager::PlayLoop(enList list)
{
	CSoundManager::GetInstance()->m_pSound[list]->PlayLoop();
}


//=============================================================================
//		停止する.
//=============================================================================
void CSoundManager::Stop(enList list)
{
	CSoundManager::GetInstance()->m_pSound[list]->Stop();
}


//=============================================================================
//		再生位置を戻さずに停止.
//=============================================================================
void CSoundManager::MiniStop(enList list)
{
	CSoundManager::GetInstance()->m_pSound[list]->MiniStop();
}


//=============================================================================
//		すべての音を停止.
//=============================================================================
void CSoundManager::AllStop()
{
	for (int i = 0;i < enList::max;++i) {
		CSoundManager::GetInstance()->m_pSound[i]->Stop();
	}
}


//=============================================================================
//		再生中かを確認.
//=============================================================================
bool CSoundManager::IsPlaying(enList list)
{
	return CSoundManager::GetInstance()->m_pSound[list]->IsPlaying();
}


//=============================================================================
//		常に最初から再生する.
//=============================================================================
void CSoundManager::PlayEx(enList list, I3DL2_ENV_PRESET preset)
{
	CSoundManager::GetInstance()->m_pSound[list]->playLoopEx(preset);
}


//=============================================================================
//		ループ再生.
//=============================================================================
void CSoundManager::playLoopEx(enList list, I3DL2_ENV_PRESET preset)
{
	CSoundManager::GetInstance()->m_pSound[list]->PlayEx(preset);
}


//=============================================================================
//		パンニングを設定する.
//=============================================================================
void CSoundManager::SetPan(enList list, LONG pan)
{
	CSoundManager::GetInstance()->m_pSound[list]->SetPan(pan);
}


//=============================================================================
//		周波数を設定する.
//=============================================================================
void CSoundManager::SetFrequency(enList list, DWORD freq)
{
	CSoundManager::GetInstance()->m_pSound[list]->SetFrequency(freq);
}


//=============================================================================
//		音量を設定する.
//=============================================================================
void CSoundManager::SetVolume(enList list, LONG volume)
{
	CSoundManager::GetInstance()->m_pSound[list]->SetVolume(volume);
}


//=============================================================================
//		パンニングを設定する.
//=============================================================================
LONG CSoundManager::GetPan(enList list)
{
	return CSoundManager::GetInstance()->m_pSound[list]->GetPan();
}


//=============================================================================
//		周波数を取得する.
//=============================================================================
DWORD CSoundManager::GetFrequency(enList list)
{
	return CSoundManager::GetInstance()->m_pSound[list]->GetFrequency();
}


//=============================================================================
//		音量を取得する.
//=============================================================================
LONG CSoundManager::GetVolume(enList list)
{
	return CSoundManager::GetInstance()->m_pSound[list]->GetVolume();
}


//=============================================================================
//		エフェクトパラメータの取得.
//=============================================================================
void CSoundManager::GetPreset(enList list, I3DL2_ENV_PRESET presetNo)
{
	CSoundManager::GetInstance()->m_pSound[list]->GetPreset(presetNo);
}


//-----------------------------------------------------------------------------
//		stringからLPSTRへ変換.
//-----------------------------------------------------------------------------
LPSTR CSoundManager::ConvertStringToLPSTR(const std::string& str)
{
	return const_cast<LPSTR>(str.c_str());
}