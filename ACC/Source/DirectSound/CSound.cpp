#include "CSound.h"
#include <crtdbg.h>		// _ASSERT_EXPR ���g�p����̂ɕK�v.
#undef max
#undef min

// �͈͓��Ɋۂ߂�֐�.
template<typename T>
T CLAMP(T value, T min, T  max) { return (value > max) ? max : (value < min) ? min : value; }
//	if( value < min ) value = min;
//	if( value > max ) value = max;
//	return value;


//==================================================================================================
//		Sound�N���X.
//==================================================================================================
CSound::CSound()
	: m_pDS			()	
	, m_pDSBuffer	()
	, VOLUME_DEFAULT( -1000 )
{
}

CSound::~CSound()
{
	// COM���C�u�������N���[�Y.
	Release();
}


//==================================================================================================
//		Sound�f�o�C�X�쐬.
//==================================================================================================
HRESULT CSound::CreateDevice( HWND hWnd )
{
	// �G�t�F�N�g�̎g�p�O��COM��������.
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		_ASSERT_EXPR(0, L"COM���C�u�����̏������Ɏ��s���܂���");
		return hr;
	}

	// DirectSound�I�u�W�F�N�g�̍쐬.
	if( FAILED( DirectSoundCreate8( nullptr, &m_pDS, nullptr ) ) )
	{
		_ASSERT_EXPR( 0, L"DS�I�u�W�F�N�g�쐬���s" );
		return E_FAIL;
	}
	
	// �������x���̐ݒ�.
	if( FAILED(
		m_pDS->SetCooperativeLevel(
			hWnd,
			DSSCL_EXCLUSIVE	)))	// ��L����.
	{
		_ASSERT_EXPR( 0, L"�������x���̐ݒ�Ɏ��s" );
		return E_FAIL;
	}
	return S_OK;
}


//==================================================================================================
//		Sound�f�o�C�X���.
//==================================================================================================
HRESULT CSound::ReleaseDevice()
{
	SAFE_RELEASE( m_pDS );

	return S_OK;
}


//==================================================================================================
//		�f�[�^�Ǎ�.
//==================================================================================================
HRESULT CSound::LoadData( LPSTR szFileName )
{
	HMMIO hMmio		= nullptr;		// Windows�}���`���f�B�AAPI�̃n���h��.
	DWORD dwWavSize	= 0;			// wav�f�[�^�̃T�C�Y.
	WAVEFORMATEX *pwfex = nullptr;	// wav�̃t�H�[�}�b�g���.
	MMCKINFO ckInfo;				// �`�����N���.
	MMCKINFO riffckInfo = {};		// �ŏ㕔�`�����N.
	PCMWAVEFORMAT pcmWaveFormat;	// PCM�\����.

	LPWSTR wstr_filename = ConvertLPSTRToLPWSTR(szFileName);

	// wav�t�@�C�����̃w�b�_�[���̊m�F��(���f�[�^�ȊO��)�ǂݍ���.
	//	MMIO_ALLOCBUF�F�t�@�C�����o�b�t�@�����O���ꂽ���o�͗p�ɊJ��.
	//	MMIO_READ	 �F�ǂݎ���p.
	hMmio = mmioOpen(
				wstr_filename,
				nullptr,
				MMIO_ALLOCBUF | MMIO_READ);
	_ASSERT_EXPR( hMmio != nullptr, L"Wave�t�@�C���̓ǂݍ��ݎ��s" );

	SAFE_DELETE(wstr_filename);

	//--------------------------------------------------------------------------------------------------
	//		�t�@�C���|�C���^��RIFF�`�����N�̐擪�ɃZ�b�g.
	//--------------------------------------------------------------------------------------------------
	if( ( MMSYSERR_NOERROR != mmioDescend( hMmio, &riffckInfo, nullptr, 0 ) ) )
	{
		_ASSERT_EXPR( 0, L"�ŏ��̃`�����N�ɐi���ł��܂���B" );
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		�`�����N���ɂ��wav�t�@�C�����ǂ����m�F����.
	//--------------------------------------------------------------------------------------------------
	if( ( riffckInfo.ckid != mmioFOURCC( 'R', 'I', 'F', 'F' ) ) 
	||  ( riffckInfo.fccType != mmioFOURCC( 'W', 'A', 'V', 'E' ) ) )
	{
		_ASSERT_EXPR( 0, L"�����wave�t�@�C���ł͂���܂���B" );
	   return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		�t�@�C���|�C���^���t�H�[�}�b�g�`�����N�ɃZ�b�g����.
	//--------------------------------------------------------------------------------------------------
	ckInfo.ckid = mmioFOURCC( 'f', 'm', 't', ' ' );
	if( MMSYSERR_NOERROR
		!= mmioDescend(
			hMmio,
			&ckInfo,
			&riffckInfo,
			MMIO_FINDCHUNK	// �w�肳�ꂽ�`�����N������.
			) )
	{
		_ASSERT_EXPR( 0, L"�t�H�[�}�b�g�`�����N��������܂���B" );
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		�t�H�[�}�b�g��ǂݍ���.
	//--------------------------------------------------------------------------------------------------
	if( mmioRead(
		hMmio, (HPSTR)&pcmWaveFormat,
		sizeof( pcmWaveFormat ) ) != sizeof( pcmWaveFormat ) )
	{
		_ASSERT_EXPR( 0, L"wave�t�H�[�}�b�g�̓Ǎ��ݎ��s" );
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		���j�APCM�ŁA���}���`�`�����l��wav�͑z��O.
	//--------------------------------------------------------------------------------------------------
	if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
	{
		pwfex = (WAVEFORMATEX*)new CHAR[ sizeof( WAVEFORMATEX ) ];	// wave�\���̏�����.
		if( pwfex == nullptr ){
			_ASSERT_EXPR( 0, L"wave�\���̏��������s" );
			return E_FAIL;
		}
		// PCM�t�H�[�}�b�g��wave�\���̂ɃR�s�[.
		memcpy( pwfex, &pcmWaveFormat, sizeof( pcmWaveFormat ) );
		pwfex->cbSize = 0;
	}
	else
	{
		_ASSERT_EXPR( 0, L"�W���̃��j�APCM�t�H�[�}�b�g�ł͂���܂���" );
		return E_FAIL;
	}
	if( MMSYSERR_NOERROR != mmioAscend( hMmio, &ckInfo, 0 ) )
	{
		delete[] pwfex;
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		wave�t�@�C�����̉��f�[�^�̓ǂݍ���.
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
		_ASSERT_EXPR( 0, L"data�`�����N��������܂���" );
		return E_FAIL;
	}
	dwWavSize = ckInfo.cksize; // wav�f�[�^���T�C�Y.


	//--------------------------------------------------------------------------------------------------
	//		DirectSound�Z�J���_���o�b�t�@�쐬.
	//--------------------------------------------------------------------------------------------------
	DSBUFFERDESC dsbd;	// DS�o�b�t�@�\����.
	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );// ������.
	dsbd.dwSize		= sizeof( DSBUFFERDESC );	//�\���̃T�C�Y.
	dsbd.dwFlags	=	DSBCAPS_CTRLPAN |		// �p���j���O��L���ɂ���.
						DSBCAPS_CTRLVOLUME |	// �{�����[����L���ɂ���.
						DSBCAPS_CTRLFREQUENCY |	// ���g����L���ɂ���.
						DSBCAPS_CTRLFX;			// �G�t�F�N�g���g�p����.
	dsbd.dwBufferBytes	 = dwWavSize;			// wav�f�[�^�̃T�C�Y.
	dsbd.guid3DAlgorithm = DS3DALG_DEFAULT;		// 3D�T�E���h.
	dsbd.lpwfxFormat	 = pwfex;				// wav�̃t�H�[�}�b�g.


	LPDIRECTSOUNDBUFFER ptmpBuffer;	// ���쐬�p�o�b�t�@.

	if( FAILED( m_pDS->CreateSoundBuffer( &dsbd, &ptmpBuffer, NULL) ) )
	{
		_ASSERT_EXPR( 0, L"�Z�J���_���o�b�t�@�̍쐬���s" );
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//	DirectSound8�̃C���^�[�t�F�[�X���T�|�[�g���Ă��邩�H.
	//		(�T�|�[�g���Ă���΁Am_pDSBuffer�ɃC���^�[�t�F�[�X�̃|�C���^���i�[�����).
	//--------------------------------------------------------------------------------------------------
	if( FAILED(
		ptmpBuffer->QueryInterface(
			IID_IDirectSoundBuffer8,	// DxSound8�C���^�t�F�[�X.
			(VOID**)&m_pDSBuffer ) ) )
	{
		_ASSERT_EXPR( 0, L"DirectSoundBuffer8���擾�ł��܂���" );
		return E_FAIL;
	}
	delete[] pwfex;	// wav���\���̍폜.


	//--------------------------------------------------------------------------------------------------
	//		�g�`�f�[�^�̏����ݏ����Ƃ��ăZ�J���_���o�b�t�@�����b�N����.
	//--------------------------------------------------------------------------------------------------
	VOID *pBuffer		= nullptr;
	DWORD dwBufferSize	= 0;
	if( FAILED(
		m_pDSBuffer->Lock( 0,
			dwWavSize, &pBuffer,
			&dwBufferSize, nullptr, nullptr, 0 ) ) )
	{
		_ASSERT_EXPR( 0, L"�Z�J���_���o�b�t�@�̃��b�N���s" );
		return E_FAIL;
	}


	//--------------------------------------------------------------------------------------------------
	//		���b�N�����o�b�t�@�T�C�Y�����ۂ̔g�`�f�[�^���傫���ꍇ(�N���b�V���h�~).
	//--------------------------------------------------------------------------------------------------
	DWORD dwSize = dwBufferSize;
	if( dwSize > ckInfo.cksize )
	{
		dwSize = ckInfo.cksize;
	}
	// �o�b�t�@�ɉ��f�[�^��ǂݍ���.
	FILE *fp;
	errno_t error;
	error = fopen_s( &fp, szFileName, "rb" );
	if( error != 0 )
	{
		_ASSERT_EXPR( 0, L"�t�@�C���̃f�[�^�擾�Ɏ��s���܂���" );
		return E_FAIL;
	}

	// �������data�܂ł̃I�t�Z�b�g���������߂�.
	fseek( fp, ckInfo.dwDataOffset, SEEK_SET );

	// wav�f�[�^�̈ꎞ�̈���쐬.
	BYTE *pWavData = new BYTE[ dwSize ];
	// �܂��͈ꎞ�̈�ɉ��f�[�^��ǂݍ���.
	fread( pWavData, 1, dwSize, fp );
	for( DWORD i = 0; i < dwSize; i++ )
	{
		// �ꎞ�̈悩��Z�J���_���o�b�t�@�ɃR�s�[.
		*((BYTE*)pBuffer+i) = *((BYTE*)pWavData+i);
	}
	fclose(fp);
	delete[] pWavData;	// �ꎞ�̈�̍폜.

	// �Z�J���_���o�b�t�@�̃A�����b�N.
	m_pDSBuffer->Unlock( pBuffer, dwBufferSize, nullptr, 0 );

	return S_OK;
}


//==================================================================================================
//		�f�[�^����֐�.
//==================================================================================================
HRESULT CSound::ReleaseData()
{
	SAFE_RELEASE( m_pDSBuffer );

	return S_OK;
}


//==================================================================================================
//		����֐�.
//==================================================================================================
HRESULT CSound::Release()
{
	ReleaseData();
	ReleaseDevice();

	return S_OK;
}


//==================================================================================================
//		��ɍŏ�����Đ�����.
//==================================================================================================
HRESULT CSound::Play(const GUID* pGuid)
{
	if (FAILED(m_pDSBuffer->Stop()) == S_OK) {
		// �����߂�.
		m_pDSBuffer->SetCurrentPosition(0);
	}
	return PlayBase(pGuid);
}


//==================================================================================================
//		���[�v�Đ�.
//==================================================================================================
void CSound::PlayLoop( const GUID *pGuid )
{
	m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
}


//==================================================================================================
//		�Đ������m�F����.
//==================================================================================================
bool CSound::IsPlaying()
{
	DWORD status;
	m_pDSBuffer->GetStatus(&status);
	if ((status & DSBSTATUS_PLAYING) == 0)
	{
		// �܂��Đ���.
		return true;
	}
	return false;
}


//==================================================================================================
//		��ɍŏ�����G�t�F�N�g�t���ōĐ�����.
//==================================================================================================
HRESULT CSound::PlayEx( I3DL2_ENV_PRESET preset )
{
	if( FAILED( m_pDSBuffer->Stop() ) == S_OK ){
		// �����߂�.
		m_pDSBuffer->SetCurrentPosition( 0 );
	}
	return PlayBase( &GUID_DSFX_STANDARD_I3DL2REVERB, GetPreset( preset ) );
}


//==================================================================================================
//		�G�t�F�N�g�t���Ń��[�v�Đ�.
//==================================================================================================
void CSound::playLoopEx( I3DL2_ENV_PRESET preset )
{
	m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
	GetPreset(preset);
}


//==================================================================================================
//		���̒�~(�Đ��ʒu��0�ɂ���).
//==================================================================================================
HRESULT CSound::Stop()
{
	// ��~.
	m_pDSBuffer->Stop();
	// �Đ��ʒu��0�ɂ��A���Z�b�g.
	m_pDSBuffer->SetCurrentPosition(0);

	if( IsPlaying() == false )
	{
		return S_OK;
	}
	return E_FAIL;
}


//==================================================================================================
//		���̒�~(�Đ��ʒu��0�ɂ��Ȃ�).
//==================================================================================================
HRESULT CSound::MiniStop()
{
	// ��~.
	m_pDSBuffer->Stop();

	if( IsPlaying() == false )
	{
		return S_OK;
	}
	return E_FAIL;
}


//==================================================================================================
//		�p���j���O��ݒ肷��.
//==================================================================================================
void CSound::SetPan( LONG pan )
{
	pan = RoundingPan( pan );
	m_pDSBuffer->SetPan( pan );
}


//==================================================================================================
//		���g����ݒ肷��.
//==================================================================================================
void CSound::SetFrequency( DWORD freq )
{
	freq = RoundingFrequency( freq );
	m_pDSBuffer->SetFrequency( freq );
}


//==================================================================================================
//		���ʂ�ݒ肷��.
//==================================================================================================
void CSound::SetVolume( LONG volume )
{
	LONG mappedvol = Mapvol(volume);

	mappedvol = RoundingVolume(mappedvol);
	m_pDSBuffer->SetVolume(mappedvol);
}


//==================================================================================================
//		���ʐݒ�̂��߂̔��]����.
//==================================================================================================
LONG CSound::Mapvol(LONG volume)
{
	volume = std::max(0L, std::min(volume, 10000L));

	return -10000 + (volume * 10000) / 10000;
}


//=============================================================================
//		���g�����擾����.
//=============================================================================
DWORD CSound::GetFrequency()
{
	DWORD dwFreq = 0;
	m_pDSBuffer->GetFrequency(&dwFreq);

	return dwFreq;
}


//=============================================================================
//		�p���j���O���擾����.
//=============================================================================
LONG CSound::GetPan()
{
	LONG lPan = 0;
	m_pDSBuffer->GetPan(&lPan);

	return lPan;
}


//=============================================================================
//		���ʂ��擾����.
//=============================================================================
LONG CSound::GetVolume()
{
	LONG lVolume = 0;
	m_pDSBuffer->GetVolume(&lVolume);

	return lVolume;
}


//=============================================================================
//		�G�t�F�N�g�p�����[�^�̎擾.
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
//		�Đ��֐�.
//----------------------------------------------------------------------------
HRESULT CSound::PlayBase(const GUID* pGuid, DWORD preset)
{
	// ���݂̃{�����[���ޔ�.
	LONG lVolume;
	m_pDSBuffer->GetVolume(&lVolume);
	// �{�����[���ŏ���ݒ�.
	m_pDSBuffer->SetVolume(DSBVOLUME_MIN);

	// ������~.
	m_pDSBuffer->Stop();

	// �G�t�F�N�g���ݒ肳��Ă��Ȃ��ꍇ.
	if (pGuid != nullptr)
	{
		// �G�t�F�N�g���ݒ肳��Ă���ꍇ�A���̃G�t�F�N�g���o�b�t�@�ɃZ�b�g����.
		DSEFFECTDESC dsEffect;	// �G�t�F�N�g�\����.
		DWORD dwResult;			// ����.
		memset(&dsEffect, 0, sizeof(DSEFFECTDESC));

		dsEffect.dwSize = sizeof(DSEFFECTDESC);	// �T�C�Y.
		dsEffect.dwFlags = 0;
		dsEffect.guidDSFXClass = *pGuid;	// �G�t�F�N�g.

		if (FAILED(
			m_pDSBuffer->SetFX(
				1,			// �G�t�F�N�g�̐�.
				&dsEffect,	// �G�t�F�N�g�\����.
				&dwResult	// (out)���s����.
			)))
		{
			_ASSERT_EXPR(0, L"�G�t�F�N�g�쐬���s");
			return E_FAIL;
		}
		if (preset != -1) {
			if (FAILED(
				SetEffectParam(m_pDSBuffer, preset)))
			{
				_ASSERT_EXPR(0, L"�G�t�F�N�g�p�����[�^�ݒ莸�s");
				return E_FAIL;
			}
		}
	}
	else
	{
		// �o�b�t�@����S�ẴG�t�F�N�g����苎��.
		if (FAILED(m_pDSBuffer->SetFX(0, NULL, NULL)))
		{
			_ASSERT_EXPR(0, L"�G�t�F�N�g�������s");
			return E_FAIL;
		}
	}


	// �{�����[���߂�.
	m_pDSBuffer->SetVolume(lVolume);

	// �Đ�.
	m_pDSBuffer->Play(NULL, 0, NULL);

	return S_OK;
}


//--------------------------------------------------------------------------------------------------
//		�G�t�F�N�g�p�����[�^��ݒ�E�K�p����.
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

	// ���o�[�u�̕i����ݒ肷��.
	if (FAILED(
		pI3DL2Reverb->SetQuality(DSFX_I3DL2REVERB_QUALITY_MAX)))
	{
		ERR_MSG(L"�ō��i���ɃZ�b�g�o���܂���ł���", L"�G���[");
		if (FAILED(
			pI3DL2Reverb->SetQuality(DSFX_I3DL2REVERB_QUALITY_DEFAULT)))
		{
			ERR_MSG(L"�W���i���ɂ��Z�b�g�o���܂���ł���", L"�G���[");
			if (FAILED(
				pI3DL2Reverb->SetQuality(DSFX_I3DL2REVERB_QUALITY_MIN)))
			{
				ERR_MSG(L"�Œ�i���ł����s���܂���", L"�G���[");
				return E_FAIL;
			}
			ERR_MSG(L"�Œ�i���ɃZ�b�g���܂���", L"�G���[");
		}
	}
	// �G�t�F�N�g�p�����[�^�[���Z�b�g.
	if (FAILED(pI3DL2Reverb->SetPreset(dwPreset)))
	{
		pI3DL2Reverb->Release();
		return E_FAIL;
	}
	pI3DL2Reverb->Release();
	return S_OK;
}


//-----------------------------------------------------------------------------
//		LPSTR�^��LPWSTR�^�ɃL���X�g����֐�.
//-----------------------------------------------------------------------------
LPWSTR CSound::ConvertLPSTRToLPWSTR(LPSTR lpstr) 
{
	// �ϊ��ɕK�v�ȃ��C�h�����̃o�b�t�@�T�C�Y���擾.
	int size_needed = MultiByteToWideChar(CP_ACP, 0, lpstr, -1, NULL, 0);

	// �K�v�ȃo�b�t�@���m��.
	LPWSTR lpwstr = new wchar_t[size_needed];

	// �V���O���o�C�g����������C�h������ɕϊ�.
	MultiByteToWideChar(CP_ACP, 0, lpstr, -1, lpwstr, size_needed);

	return lpwstr;  // �Ăяo�����Ń��������������K�v������.
}


//-----------------------------------------------------------------------------
//		�p���j���O���͈͊O�ɐݒ肳��Ȃ��悤�ɂ���֐�.
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
//		���g�����͈͊O�ɐݒ肳��Ȃ��悤�ɂ���֐�.
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
//		���ʂ��͈͊O�ɐݒ肳��Ȃ��悤�ɂ���֐�.
//-----------------------------------------------------------------------------
LONG CSound::RoundingVolume( LONG volume )
{
	volume = CLAMP( volume,  (LONG)DSBVOLUME_MIN, (LONG)DSBVOLUME_MAX );

	return volume;
}