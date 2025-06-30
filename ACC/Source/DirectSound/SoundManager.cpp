#include "SoundManager.h"


// サウンド読み込み用のリスト設定
static SoundManager::SoundList s_SoundList[] =
{
	// 10000がMAXの音量
	// ここで設定する音量の数値はMAX時の音量の数値
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
//		作成処理
//==============================================================================
HRESULT SoundManager::Create(Sound** ppSound, std::string filePath, HWND hWnd)
{
	*ppSound = new Sound();

	if (!*ppSound)
	{
		return E_FAIL;	// メモリ確保失敗時のエラー処理
	}
	(*ppSound)->CreateDevice(hWnd);
	return (*ppSound)->LoadData(ConvertStringToLPSTR(filePath));
}


//==============================================================================
//		データ読込処理
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
//		解放処理
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
//		音量を適用させる
//==============================================================================
void SoundManager::ApplyVolumeSetting()
{
	for (size_t i = 0; i < m_SoundList.size(); i++)
	{
		if (i < m_pSound.size())
		{
			LONG adjustedVolume;	// 適用させるための変数

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
				// エイリアス名にBGMがついている物の音の調整
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume * m_BGMVolume);
			}
			else if(m_SoundList[i].alias->find("SE") != std::string::npos)
			{
				// エイリアス名にSEがついている物の音の調整
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume * m_SEVolume);
			}
			else
			{
				// それ以外の音の調整
				adjustedVolume = static_cast<LONG>(m_SoundList[i].defaultVolume * m_MasterVolume);
			}

			//設定した音をSoundに適応
			m_pSound[i]->SetVolume(adjustedVolume);
		}
	}
}


//=============================================================================
//		一度だけ再生する
//=============================================================================
void SoundManager::Play(enList list)
{
	SoundManager::GetInstance()->m_pSound[list]->Play();
}


//=============================================================================
//		ループ再生する
//=============================================================================
void SoundManager::PlayLoop(enList list)
{
	SoundManager::GetInstance()->m_pSound[list]->PlayLoop();
}


//=============================================================================
//		停止する
//=============================================================================
void SoundManager::Stop(enList list)
{
	SoundManager::GetInstance()->m_pSound[list]->Stop();
}


//=============================================================================
//		再生位置を戻さずに停止
//=============================================================================
void SoundManager::MiniStop(enList list)
{
	SoundManager::GetInstance()->m_pSound[list]->MiniStop();
}


//=============================================================================
//		すべての音を停止
//=============================================================================
void SoundManager::AllStop()
{
	for (int i = 0;i < enList::max;++i) {
		SoundManager::GetInstance()->m_pSound[i]->Stop();
	}
}


//=============================================================================
//		再生中かを確認
//=============================================================================
bool SoundManager::IsPlaying(enList list)
{
	return SoundManager::GetInstance()->m_pSound[list]->IsPlaying();
}


//=============================================================================
//		常に最初から再生する
//=============================================================================
void SoundManager::PlayEx(enList list, I3DL2_ENV_PRESET preset)
{
	SoundManager::GetInstance()->m_pSound[list]->playLoopEx(preset);
}


//=============================================================================
//		ループ再生
//=============================================================================
void SoundManager::playLoopEx(enList list, I3DL2_ENV_PRESET preset)
{
	SoundManager::GetInstance()->m_pSound[list]->PlayEx(preset);
}


//=============================================================================
//		パンニングを設定する
//=============================================================================
void SoundManager::SetPan(enList list, LONG pan)
{
	SoundManager::GetInstance()->m_pSound[list]->SetPan(pan);
}


//=============================================================================
//		周波数を設定する
//=============================================================================
void SoundManager::SetFrequency(enList list, DWORD freq)
{
	SoundManager::GetInstance()->m_pSound[list]->SetFrequency(freq);
}


//=============================================================================
//		音量を設定する
//=============================================================================
void SoundManager::SetVolume(enList list, LONG volume)
{
	SoundManager::GetInstance()->m_pSound[list]->SetVolume(volume);
}


//=============================================================================
//		パンニングを設定する
//=============================================================================
LONG SoundManager::GetPan(enList list)
{
	return SoundManager::GetInstance()->m_pSound[list]->GetPan();
}


//=============================================================================
//		周波数を取得する
//=============================================================================
DWORD SoundManager::GetFrequency(enList list)
{
	return SoundManager::GetInstance()->m_pSound[list]->GetFrequency();
}


//=============================================================================
//		音量を取得する
//=============================================================================
LONG SoundManager::GetVolume(enList list)
{
	return SoundManager::GetInstance()->m_pSound[list]->GetVolume();
}


//=============================================================================
//		エフェクトパラメータの取得
//=============================================================================
void SoundManager::GetPreset(enList list, I3DL2_ENV_PRESET presetNo)
{
	SoundManager::GetInstance()->m_pSound[list]->GetPreset(presetNo);
}


//-----------------------------------------------------------------------------
//		stringからLPSTRへ変換
//-----------------------------------------------------------------------------
LPSTR SoundManager::ConvertStringToLPSTR(const std::string& str)
{
	return const_cast<LPSTR>(str.c_str());
}