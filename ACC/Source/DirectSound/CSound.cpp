#include "CSound.h"
#include <crtdbg.h>		// _ASSERT_EXPR を使用するのに必要.
#undef max
#undef min

// 範囲内に丸める関数.
template<typename T>
T CLAMP(T value, T min, T  max) { return (value > max) ? max : (value < min) ? min : value; }
//	if( value < min ) value = min;
//	if( value > max ) value = max;
//	return value;


//==================================================================================================
//		Soundクラス.
//==================================================================================================
CSound::CSound()
	: m_pDS			()	
	, m_pDSBuffer	()
	, VOLUME_DEFAULT( -1000 )
{
}

CSound::~CSound()
{
	// COMライブラリをクローズ.
	Release();
}


//==================================================================================================
//		Soundデバイス作成.
//==================================================================================================
HRESULT CSound::CreateDevice( HWND hWnd )
{
	// エフェクトの使用前にCOMを初期化.
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		_ASSERT_EXPR(0, L"COMライブラリの初期化に失敗しました");
		return hr;
	}

	// DirectSoundオブジェクトの作成.
	if( FAILED( DirectSoundCreate8( nullptr, &m_pDS, nullptr ) ) )
	{
		_ASSERT_EXPR( 0, L"DSオブジェクト作成失敗" );
		return E_FAIL;
	}
	
	// 協調レベルの設定.
	if( FAILED(
		m_pDS->SetCooperativeLevel(
			hWnd,
			DSSCL_EXCLUSIVE	)))	// 占有する.
	{
		_ASSERT_EXPR( 0, L"協調レベルの設定に失敗" );
		return E_FAIL;
	}
	return S_OK;
}


//==================================================================================================
//		Soundデバイス解放.
//==================================================================================================
HRESULT CSound::ReleaseDevice()
{
	SAFE_RELEASE( m_pDS );

	return S_OK;
}


//==================================================================================================
//		データ読込.
//==================================================================================================
HRESULT CSound::LoadData( LPSTR szFileName )
{
	HMMIO hMmio		= nullptr;		// WindowsマルチメディアAPIのハンドル.
	DWORD dwWavSize	= 0;			// wavデータのサイズ.
	WAVEFORMATEX *pwfex = nullptr;	// wavのフォーマット情報.
	MMCKINFO ckInfo;				// チャンク情報.
	MMCKINFO riffckInfo = {};		// 最上部チャンク.
	PCMWAVEFORMAT pcmWaveFormat;	// PCM構造体.

	LPWSTR wstr_filename = ConvertLPSTRToLPWSTR(szFileName);

	// wavファイル内のヘッダー情報の確認と(音データ以外の)読み込み.
	//	MMIO_ALLOCBUF：ファイルをバッファリングされた入出力用に開く.
	//	MMIO_READ	 ：読み取り専用.
	hMmio = mmioOpen(
				wstr_filename,
				nullptr,
				MMIO_ALLOCBUF | MMIO_READ);
	_ASSERT_EXPR( hMmio != nullptr, L"Waveファイルの読み込み失敗" );

	SAFE_DELETE(wstr_filename);

	//--------------------------------------------------------------------------------------------------
	//		ファイルポインタをRIFFチャンクの先頭にセット.
	//--------------------------------------------------------------------------------------------------
	if( ( MMSYSERR_NOERROR != mmioDescend( hMmio, &riffckInfo, nullptr, 0 ) ) )
	{
		_ASSERT_EXPR( 0, L"最初のチャンクに進入できません。" );
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		チャンク情報によりwavファイルかどうか確認する.
	//--------------------------------------------------------------------------------------------------
	if( ( riffckInfo.ckid != mmioFOURCC( 'R', 'I', 'F', 'F' ) ) 
	||  ( riffckInfo.fccType != mmioFOURCC( 'W', 'A', 'V', 'E' ) ) )
	{
		_ASSERT_EXPR( 0, L"これはwaveファイルではありません。" );
	   return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		ファイルポインタをフォーマットチャンクにセットする.
	//--------------------------------------------------------------------------------------------------
	ckInfo.ckid = mmioFOURCC( 'f', 'm', 't', ' ' );
	if( MMSYSERR_NOERROR
		!= mmioDescend(
			hMmio,
			&ckInfo,
			&riffckInfo,
			MMIO_FINDCHUNK	// 指定されたチャンクを検索.
			) )
	{
		_ASSERT_EXPR( 0, L"フォーマットチャンクが見つかりません。" );
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		フォーマットを読み込む.
	//--------------------------------------------------------------------------------------------------
	if( mmioRead(
		hMmio, (HPSTR)&pcmWaveFormat,
		sizeof( pcmWaveFormat ) ) != sizeof( pcmWaveFormat ) )
	{
		_ASSERT_EXPR( 0, L"waveフォーマットの読込み失敗" );
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		リニアPCMで、かつマルチチャンネルwavは想定外.
	//--------------------------------------------------------------------------------------------------
	if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
	{
		pwfex = (WAVEFORMATEX*)new CHAR[ sizeof( WAVEFORMATEX ) ];	// wave構造体初期化.
		if( pwfex == nullptr ){
			_ASSERT_EXPR( 0, L"wave構造体初期化失敗" );
			return E_FAIL;
		}
		// PCMフォーマットをwave構造体にコピー.
		memcpy( pwfex, &pcmWaveFormat, sizeof( pcmWaveFormat ) );
		pwfex->cbSize = 0;
	}
	else
	{
		_ASSERT_EXPR( 0, L"標準のリニアPCMフォーマットではありません" );
		return E_FAIL;
	}
	if( MMSYSERR_NOERROR != mmioAscend( hMmio, &ckInfo, 0 ) )
	{
		delete[] pwfex;
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		waveファイル内の音データの読み込み.
	//--------------------------------------------------------------------------------------------------
	ckInfo.ckid = mmioFOURCC( 'd', 'a', 't', 'a' );
	if( MMSYSERR_NOERROR
		!= mmioDescend(
			hMmio,
			&ckInfo,
			&riffckInfo,
			MMIO_FINDCHUNK
			) )
	{
		_ASSERT_EXPR( 0, L"dataチャンクが見つかりません" );
		return E_FAIL;
	}
	dwWavSize = ckInfo.cksize; // wavデータリサイズ.


	//--------------------------------------------------------------------------------------------------
	//		DirectSoundセカンダリバッファ作成.
	//--------------------------------------------------------------------------------------------------
	DSBUFFERDESC dsbd;	// DSバッファ構造体.
	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );// 初期化.
	dsbd.dwSize		= sizeof( DSBUFFERDESC );	//構造体サイズ.
	dsbd.dwFlags	=	DSBCAPS_CTRLPAN |		// パンニングを有効にする.
						DSBCAPS_CTRLVOLUME |	// ボリュームを有効にする.
						DSBCAPS_CTRLFREQUENCY |	// 周波数を有効にする.
						DSBCAPS_CTRLFX;			// エフェクトを使用する.
	dsbd.dwBufferBytes	 = dwWavSize;			// wavデータのサイズ.
	dsbd.guid3DAlgorithm = DS3DALG_DEFAULT;		// 3Dサウンド.
	dsbd.lpwfxFormat	 = pwfex;				// wavのフォーマット.


	LPDIRECTSOUNDBUFFER ptmpBuffer;	// 仮作成用バッファ.

	if( FAILED( m_pDS->CreateSoundBuffer( &dsbd, &ptmpBuffer, NULL) ) )
	{
		_ASSERT_EXPR( 0, L"セカンダリバッファの作成失敗" );
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//	DirectSound8のインターフェースをサポートしているか？.
	//		(サポートしていれば、m_pDSBufferにインターフェースのポインタが格納される).
	//--------------------------------------------------------------------------------------------------
	if( FAILED(
		ptmpBuffer->QueryInterface(
			IID_IDirectSoundBuffer8,	// DxSound8インタフェース.
			(VOID**)&m_pDSBuffer ) ) )
	{
		_ASSERT_EXPR( 0, L"DirectSoundBuffer8を取得できません" );
		return E_FAIL;
	}
	delete[] pwfex;	// wav情報構造体削除.


	//--------------------------------------------------------------------------------------------------
	//		波形データの書込み準備としてセカンダリバッファをロックする.
	//--------------------------------------------------------------------------------------------------
	VOID *pBuffer		= nullptr;
	DWORD dwBufferSize	= 0;
	if( FAILED(
		m_pDSBuffer->Lock( 0,
			dwWavSize, &pBuffer,
			&dwBufferSize, nullptr, nullptr, 0 ) ) )
	{
		_ASSERT_EXPR( 0, L"セカンダリバッファのロック失敗" );
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		ロックしたバッファサイズが実際の波形データより大きい場合(クラッシュ防止).
	//--------------------------------------------------------------------------------------------------
	DWORD dwSize = dwBufferSize;
	if( dwSize > ckInfo.cksize )
	{
		dwSize = ckInfo.cksize;
	}
	// バッファに音データを読み込み.
	FILE *fp;
	errno_t error;
	error = fopen_s( &fp, szFileName, "rb" );
	if( error != 0 )
	{
		_ASSERT_EXPR( 0, L"ファイルのデータ取得に失敗しました" );
		return E_FAIL;
	}

	// こちらでdataまでのオフセットが書き込める.
	fseek( fp, ckInfo.dwDataOffset, SEEK_SET );

	// wavデータの一時領域を作成.
	BYTE *pWavData = new BYTE[ dwSize ];
	// まずは一時領域に音データを読み込み.
	fread( pWavData, 1, dwSize, fp );
	for( DWORD i = 0; i < dwSize; i++ )
	{
		// 一時領域からセカンダリバッファにコピー.
		*((BYTE*)pBuffer+i) = *((BYTE*)pWavData+i);
	}
	fclose(fp);
	delete[] pWavData;	// 一時領域の削除.

	// セカンダリバッファのアンロック.
	m_pDSBuffer->Unlock( pBuffer, dwBufferSize, nullptr, 0 );

	return S_OK;
}


//==================================================================================================
//		データ解放関数.
//==================================================================================================
HRESULT CSound::ReleaseData()
{
	SAFE_RELEASE( m_pDSBuffer );

	return S_OK;
}


//==================================================================================================
//		解放関数.
//==================================================================================================
HRESULT CSound::Release()
{
	ReleaseData();
	ReleaseDevice();

	return S_OK;
}


//==================================================================================================
//		常に最初から再生する.
//==================================================================================================
HRESULT CSound::Play(const GUID* pGuid)
{
	if (FAILED(m_pDSBuffer->Stop()) == S_OK) {
		// 巻き戻し.
		m_pDSBuffer->SetCurrentPosition(0);
	}
	return PlayBase(pGuid);
}


//==================================================================================================
//		ループ再生.
//==================================================================================================
void CSound::PlayLoop( const GUID *pGuid )
{
	m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
}


//==================================================================================================
//		再生中を確認する.
//==================================================================================================
bool CSound::IsPlaying()
{
	DWORD status;
	m_pDSBuffer->GetStatus(&status);
	if ((status & DSBSTATUS_PLAYING) == 0)
	{
		// まだ再生中.
		return true;
	}
	return false;
}


//==================================================================================================
//		常に最初からエフェクト付きで再生する.
//==================================================================================================
HRESULT CSound::PlayEx( I3DL2_ENV_PRESET preset )
{
	if( FAILED( m_pDSBuffer->Stop() ) == S_OK ){
		// 巻き戻し.
		m_pDSBuffer->SetCurrentPosition( 0 );
	}
	return PlayBase( &GUID_DSFX_STANDARD_I3DL2REVERB, GetPreset( preset ) );
}


//==================================================================================================
//		エフェクト付きでループ再生.
//==================================================================================================
void CSound::playLoopEx( I3DL2_ENV_PRESET preset )
{
	m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
	GetPreset(preset);
}


//==================================================================================================
//		音の停止(再生位置を0にする).
//==================================================================================================
HRESULT CSound::Stop()
{
	// 停止.
	m_pDSBuffer->Stop();
	// 再生位置を0にし、リセット.
	m_pDSBuffer->SetCurrentPosition(0);

	if( IsPlaying() == false )
	{
		return S_OK;
	}
	return E_FAIL;
}


//==================================================================================================
//		音の停止(再生位置を0にしない).
//==================================================================================================
HRESULT CSound::MiniStop()
{
	// 停止.
	m_pDSBuffer->Stop();

	if( IsPlaying() == false )
	{
		return S_OK;
	}
	return E_FAIL;
}


//==================================================================================================
//		パンニングを設定する.
//==================================================================================================
void CSound::SetPan( LONG pan )
{
	pan = RoundingPan( pan );
	m_pDSBuffer->SetPan( pan );
}


//==================================================================================================
//		周波数を設定する.
//==================================================================================================
void CSound::SetFrequency( DWORD freq )
{
	freq = RoundingFrequency( freq );
	m_pDSBuffer->SetFrequency( freq );
}


//==================================================================================================
//		音量を設定する.
//==================================================================================================
void CSound::SetVolume( LONG volume )
{
	LONG mappedvol = Mapvol(volume);

	mappedvol = RoundingVolume(mappedvol);
	m_pDSBuffer->SetVolume(mappedvol);
}


//==================================================================================================
//		音量設定のための反転処理.
//==================================================================================================
LONG CSound::Mapvol(LONG volume)
{
	volume = std::max(0L, std::min(volume, 10000L));

	return -10000 + (volume * 10000) / 10000;
}


//=============================================================================
//		周波数を取得する.
//=============================================================================
DWORD CSound::GetFrequency()
{
	DWORD dwFreq = 0;
	m_pDSBuffer->GetFrequency(&dwFreq);

	return dwFreq;
}


//=============================================================================
//		パンニングを取得する.
//=============================================================================
LONG CSound::GetPan()
{
	LONG lPan = 0;
	m_pDSBuffer->GetPan(&lPan);

	return lPan;
}


//=============================================================================
//		音量を取得する.
//=============================================================================
LONG CSound::GetVolume()
{
	LONG lVolume = 0;
	m_pDSBuffer->GetVolume(&lVolume);

	return lVolume;
}


//=============================================================================
//		エフェクトパラメータの取得.
//=============================================================================
DWORD CSound::GetPreset( I3DL2_ENV_PRESET presetNo )
{
	DWORD param = static_cast<DWORD>( I3DL2_ENV_PRESET::NONE );

	switch( presetNo ){
	case I3DL2_ENV_PRESET::DEFAULT:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_DEFAULT;
		break;
	case I3DL2_ENV_PRESET::GENERIC:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_GENERIC;
		break;
	case I3DL2_ENV_PRESET::PADDEDCELL:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_PADDEDCELL;
		break;
	case I3DL2_ENV_PRESET::ROOM:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_ROOM;
		break;
	case I3DL2_ENV_PRESET::BATHROOM:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_BATHROOM;
		break;
	case I3DL2_ENV_PRESET::LIVINGROOM:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_LIVINGROOM;
		break;
	case I3DL2_ENV_PRESET::STONEROOM:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_STONEROOM;
		break;
	case I3DL2_ENV_PRESET::AUDITORIUM:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_AUDITORIUM;
		break;
	case I3DL2_ENV_PRESET::CONCERTHALL:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_CONCERTHALL;
		break;
	case I3DL2_ENV_PRESET::CAVE:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_CAVE;
		break;
	case I3DL2_ENV_PRESET::ARENA:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_ARENA;
		break;
	case I3DL2_ENV_PRESET::HANGAR:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_HANGAR;
		break;
	case I3DL2_ENV_PRESET::CARPETEDHALLWAY:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY;
		break;
	case I3DL2_ENV_PRESET::HALLWAY:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_HALLWAY;
		break;
	case I3DL2_ENV_PRESET::STONECORRIDOR:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
		break;
	case I3DL2_ENV_PRESET::ALLEY:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_ALLEY;
		break;
	case I3DL2_ENV_PRESET::FOREST:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_FOREST;
		break;
	case I3DL2_ENV_PRESET::CITY:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_CITY;
		break;
	case I3DL2_ENV_PRESET::MOUNTAINS:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_MOUNTAINS;
		break;
	case I3DL2_ENV_PRESET::QUARRY:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_QUARRY;
		break;
	case I3DL2_ENV_PRESET::PLAIN:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_PLAIN;
		break;
	case I3DL2_ENV_PRESET::PARKINGLOT:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_PARKINGLOT;
		break;
	case I3DL2_ENV_PRESET::SEWERPIPE:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_SEWERPIPE;
		break;
	case I3DL2_ENV_PRESET::UNDERWATER:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_UNDERWATER;
		break;
	case I3DL2_ENV_PRESET::SMALLROOM:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_SMALLROOM;
		break;
	case I3DL2_ENV_PRESET::MEDIUMROOM:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMROOM;
		break;
	case I3DL2_ENV_PRESET::LARGEROOM:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEROOM;
		break;
	case I3DL2_ENV_PRESET::MEDIUMHALL:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMHALL;
		break;
	case I3DL2_ENV_PRESET::LARGEHALL:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEHALL;
		break;
	case I3DL2_ENV_PRESET::PLATE:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_PLATE;
		break;
	default:
		param = DSFX_I3DL2_ENVIRONMENT_PRESET_DEFAULT;
		break;
	}

	return param;
}


//----------------------------------------------------------------------------
//		再生関数.
//----------------------------------------------------------------------------
HRESULT CSound::PlayBase(const GUID* pGuid, DWORD preset)
{
	// 現在のボリューム退避.
	LONG lVolume;
	m_pDSBuffer->GetVolume(&lVolume);
	// ボリューム最少を設定.
	m_pDSBuffer->SetVolume(DSBVOLUME_MIN);

	// 音声停止.
	m_pDSBuffer->Stop();

	// エフェクトが設定されていない場合.
	if (pGuid != nullptr)
	{
		// エフェクトが設定されている場合、そのエフェクトをバッファにセットする.
		DSEFFECTDESC dsEffect;	// エフェクト構造体.
		DWORD dwResult;			// 結果.
		memset(&dsEffect, 0, sizeof(DSEFFECTDESC));

		dsEffect.dwSize = sizeof(DSEFFECTDESC);	// サイズ.
		dsEffect.dwFlags = 0;
		dsEffect.guidDSFXClass = *pGuid;	// エフェクト.

		if (FAILED(
			m_pDSBuffer->SetFX(
				1,			// エフェクトの数.
				&dsEffect,	// エフェクト構造体.
				&dwResult	// (out)実行結果.
			)))
		{
			_ASSERT_EXPR(0, L"エフェクト作成失敗");
			return E_FAIL;
		}
		if (preset != -1) {
			if (FAILED(
				SetEffectParam(m_pDSBuffer, preset)))
			{
				_ASSERT_EXPR(0, L"エフェクトパラメータ設定失敗");
				return E_FAIL;
			}
		}
	}
	else
	{
		// バッファから全てのエフェクトを取り去る.
		if (FAILED(m_pDSBuffer->SetFX(0, NULL, NULL)))
		{
			_ASSERT_EXPR(0, L"エフェクト解除失敗");
			return E_FAIL;
		}
	}


	// ボリューム戻す.
	m_pDSBuffer->SetVolume(lVolume);

	// 再生.
	m_pDSBuffer->Play(NULL, 0, NULL);

	return S_OK;
}


//--------------------------------------------------------------------------------------------------
//		エフェクトパラメータを設定・適用する.
//--------------------------------------------------------------------------------------------------
HRESULT CSound::SetEffectParam(LPDIRECTSOUNDBUFFER8 pDSBuffer8, DWORD dwPreset)
{
	IDirectSoundFXI3DL2Reverb8* pI3DL2Reverb;
	if (FAILED(
		pDSBuffer8->GetObjectInPath(
			GUID_DSFX_STANDARD_I3DL2REVERB,
			0,
			IID_IDirectSoundFXI3DL2Reverb8,
			(VOID**)&pI3DL2Reverb)))
	{
		return E_FAIL;
	}

	// リバーブの品質を設定する.
	if (FAILED(
		pI3DL2Reverb->SetQuality(DSFX_I3DL2REVERB_QUALITY_MAX)))
	{
		ERR_MSG(L"最高品質にセット出来ませんでした", L"エラー");
		if (FAILED(
			pI3DL2Reverb->SetQuality(DSFX_I3DL2REVERB_QUALITY_DEFAULT)))
		{
			ERR_MSG(L"標準品質にもセット出来ませんでした", L"エラー");
			if (FAILED(
				pI3DL2Reverb->SetQuality(DSFX_I3DL2REVERB_QUALITY_MIN)))
			{
				ERR_MSG(L"最低品質でも失敗しました", L"エラー");
				return E_FAIL;
			}
			ERR_MSG(L"最低品質にセットしました", L"エラー");
		}
	}
	// エフェクトパラメーターをセット.
	if (FAILED(pI3DL2Reverb->SetPreset(dwPreset)))
	{
		pI3DL2Reverb->Release();
		return E_FAIL;
	}
	pI3DL2Reverb->Release();
	return S_OK;
}


//-----------------------------------------------------------------------------
//		LPSTR型をLPWSTR型にキャストする関数.
//-----------------------------------------------------------------------------
LPWSTR CSound::ConvertLPSTRToLPWSTR(LPSTR lpstr) 
{
	// 変換に必要なワイド文字のバッファサイズを取得.
	int size_needed = MultiByteToWideChar(CP_ACP, 0, lpstr, -1, NULL, 0);

	// 必要なバッファを確保.
	LPWSTR lpwstr = new wchar_t[size_needed];

	// シングルバイト文字列をワイド文字列に変換.
	MultiByteToWideChar(CP_ACP, 0, lpstr, -1, lpwstr, size_needed);

	return lpwstr;  // 呼び出し側でメモリを解放する必要がある.
}


//-----------------------------------------------------------------------------
//		パンニングが範囲外に設定されないようにする関数.
//-----------------------------------------------------------------------------
LONG CSound::RoundingPan( LONG pan )
{
	if( pan != DSBPAN_CENTER )
	{
		pan = CLAMP( pan, (LONG)DSBPAN_LEFT, (LONG)DSBPAN_RIGHT );
	}
	return pan;
}


//-----------------------------------------------------------------------------
//		周波数が範囲外に設定されないようにする関数.
//-----------------------------------------------------------------------------
DWORD CSound::RoundingFrequency( DWORD freq )
{
	if( freq != DSBFREQUENCY_ORIGINAL )
	{
		freq = CLAMP( freq, (DWORD)DSBFREQUENCY_MIN, (DWORD)DSBFREQUENCY_MAX );
	}
	return freq;
}


//-----------------------------------------------------------------------------
//		音量が範囲外に設定されないようにする関数.
//-----------------------------------------------------------------------------
LONG CSound::RoundingVolume( LONG volume )
{
	volume = CLAMP( volume,  (LONG)DSBVOLUME_MIN, (LONG)DSBVOLUME_MAX );

	return volume;
}