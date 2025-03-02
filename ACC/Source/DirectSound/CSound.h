#pragma once

#include <dsound.h>

#pragma comment ( lib, "dsound.lib" )	// DirectSound.
#pragma comment ( lib, "dxguid.lib" )	// エフェクト用.


enum class I3DL2_ENV_PRESET : int
{
	DEFAULT,		// 標準.
	GENERIC,		// 一般.
	PADDEDCELL,		// クッション壁のある部屋.
	ROOM,			// 室内.
	BATHROOM,		// バスルーム.
	LIVINGROOM,		// リビングルーム.
	STONEROOM,		// 石造りの部屋.
	AUDITORIUM,		// 講堂.
	CONCERTHALL,	// コンサートホール.
	CAVE,			// 洞窟.
	ARENA,			// 競技場.
	HANGAR,			// 格納庫.
	CARPETEDHALLWAY,// 絨毯(じゅうたん)敷の通路.
	HALLWAY,		// 通路・廊下.
	STONECORRIDOR,	// 石造りの廊下.
	ALLEY,			// 路地.
	FOREST,			// 森.
	CITY,			// 街・都会.
	MOUNTAINS,		// 山中.
	QUARRY,			// 採石場
	PLAIN,			// 平原.
	PARKINGLOT,		// 駐車場.
	SEWERPIPE,		// 下水管内.
	UNDERWATER,		// 水中.
	SMALLROOM,		// 小部屋.
	MEDIUMROOM,		// 中部屋.
	LARGEROOM,		// 大部屋.
	MEDIUMHALL,		// 中ホール.
	LARGEHALL,		// 大ホール.
	PLATE,			// プレートリバーブのシミュレート.

	MAX,
	NONE = -1,		// 未設定.
};


//============================================================================
//		Soundクラス.
//============================================================================
class CSound
{
public:
	const LONG VOLUME_DEFAULT;		// 音量初期値(db).

	CSound();	// コンストラクタ.
	CSound(LONG VolumeDefault)
		: VOLUME_DEFAULT(VolumeDefault), m_pDS(), m_pDSBuffer() {};	// コンストラクタ(初期音量設定可能版).
	~CSound();	// デストラクタ.

	// Soundデバイス作成・解放.
	HRESULT CreateDevice( HWND hWnd );
	HRESULT ReleaseDevice();

	// データ読込・解放関数.
	HRESULT LoadData( LPSTR szFileName );
	HRESULT ReleaseData();

	// 解放関数.
	HRESULT Release();

	// 常に最初から再生する.
	HRESULT Play( const GUID *pGuid = nullptr );
	// ループ再生.
	void PlayLoop( const GUID *pGuid = nullptr );
	// 再生中を確認する.
	bool IsPlaying();

	// 常に最初からエフェクト付きで再生する.
	HRESULT PlayEx( I3DL2_ENV_PRESET preset );
	// エフェクト付きでループ再生.
	void playLoopEx( I3DL2_ENV_PRESET preset );

	// 音の停止(再生位置を0にする).
	HRESULT Stop();
	// 音の停止(再生位置を0にしない).
	HRESULT MiniStop();

	// パンニングを設定する.
	//	パンニング = 左右どちらかに音を偏らせる.
	//	左 = -10000、右 = 10000.
	void SetPan( LONG pan );

	// 周波数を設定する.
	//	音の高さが変わる.
	void SetFrequency( DWORD freq );

	// 音量を設定する.
	void SetVolume( LONG volume );

	// 音量設定のための反転処理.
	LONG Mapvol(LONG volume);
	
	// 周波数を取得する.
	DWORD GetFrequency();
	// パンニングを取得する.
	LONG GetPan();
	// 音量を取得する.
	LONG GetVolume();
	// エフェクトパラメータの取得.
	DWORD GetPreset( I3DL2_ENV_PRESET presetNo );

private:
	LPDIRECTSOUND8			m_pDS;			// DirectSoundオブジェクト.
	LPDIRECTSOUNDBUFFER8	m_pDSBuffer;	// DSセカンダリバッファ.

	// 再生関数.
	HRESULT PlayBase( const GUID *pGuid, DWORD preset = -1 );
	// エフェクトパラメータを設定・適用する.
	HRESULT SetEffectParam( LPDIRECTSOUNDBUFFER8 pDSBuffer8, DWORD dwPreset );
	
	// LPSTR型をLPWSTR型にキャストする関数
	LPWSTR ConvertLPSTRToLPWSTR(LPSTR lpstr);

	// パンニング、周波数、音量が範囲外に設定されないようにする関数.
	LONG  RoundingPan( LONG pan );
	DWORD RoundingFrequency( DWORD freq );
	LONG  RoundingVolume( LONG volume );
};