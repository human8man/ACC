#pragma once

#include "CSound.h"
#include "Singleton/CSingleton.h"


//==============================================================================
//		サウンドマネージャークラス.
//==============================================================================
class CSoundManager
	: public CSingleton<CSoundManager>
{
private:
	friend class CSingleton<CSoundManager>; // シングルトンクラスをフレンド宣言.

public:
	// サウンドリスト列挙型.
	enum enList
	{
		SE_CritHit,		// 気室ショット.
		SE_Hit,			// 命中.
		SE_NoAmmo,		// 残弾数なしのときに撃つとなる音.
		SE_Reload,		// リロード.
		SE_ReloadEnd,	// リロード終了.
		SE_SelectMove,	// 選択肢移動.
		SE_Shot,		// 射撃.
		SE_Dash,		// ダッシュ.
		SE_Jump,		// ジャンプ.
		SE_Damage,		// ダメージ.
		SE_DamageCrit,	// クリティカルダメージ.
		SE_Lose,		// まけ.
		SE_Win,			// かち.

		BGM_Game,
		BGM_Title,

		// 音が増えたら「ここ」に追加してください.
		max, // 最大数.
	};

	struct SoundList {
		enList listNo;					// enList列挙型を設定.
		std::string path;				// ファイルの名前(パス付き).
		std::string alias[32];			// エイリアス名.

		LONG defaultVolume;				// デフォルト音量(-10000 ~ 0).
	};

public:
	CSoundManager();
	~CSoundManager();

	// 作成処理.
	HRESULT Create(CSound** ppSound, std::string filePath, HWND hWnd);
	// データ読込処理.
	HRESULT Load( HWND hWnd );
	// 解放処理.
	void Release();
	
	// 音量を適用させる.
	void ApplyVolumeSetting();
	
	// 一度だけ再生する.
	static void Play			( enList list );
	// ループ再生する.
	static void PlayLoop		( enList list );
	// 停止する.
	static void Stop			( enList list );
	// 再生位置を戻さずに停止.
	static void MiniStop		( enList list );
	// すべての音を停止.
	static void AllStop			();

	// 再生中かを確認.
	static bool IsPlaying		( enList list );

	// 常に最初から再生する.
	static void PlayEx			( enList list, I3DL2_ENV_PRESET preset );
	// ループ再生.
	static void playLoopEx		( enList list, I3DL2_ENV_PRESET preset );

	// パンニングを設定する.
	static void SetPan			( enList list, LONG pan );
	// 周波数を設定する.
	static void SetFrequency	( enList list, DWORD freq );
	// 音量を設定する.
	static void SetVolume		( enList list, LONG volume );

	// パンニングを設定する.
	static LONG GetPan			( enList list );
	// 周波数を取得する.
	static DWORD GetFrequency	( enList list );
	// 音量を取得する.
	static LONG GetVolume		( enList list );
	// エフェクトパラメータの取得.
	static void GetPreset		( enList list, I3DL2_ENV_PRESET presetNo );

	// MasterVolumeの取得.
	float GetMasterVol() { return m_MasterVolume; }
	// MasterVolumeの設定.
	void  SetMasterVol(float Vol) { m_MasterVolume = Vol; }

	// BGMVolumeの取得.
	float GetBGMVol() { return m_BGMVolume; }
	// BGMVolumeの設定.
	void SetBGMVol(float Vol) { m_BGMVolume = Vol; }

	// SEVolumeの取得.
	float GetSEVol() { return m_SEVolume; }
	// SEVolumeの設定
	void SetSEVol(float Vol) { m_SEVolume = Vol; }
	
private:
	// stringからLPSTRへ変換.
	LPSTR ConvertStringToLPSTR(const std::string& str);

private:
	std::vector<CSound*> m_pSound;

	std::vector<SoundList> m_SoundList;	// サウンドリスト.
	float m_MasterVolume;
	float m_BGMVolume;
	float m_SEVolume;
};