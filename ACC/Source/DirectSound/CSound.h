#pragma once

#include <dsound.h>

#pragma comment ( lib, "dsound.lib" )	// DirectSound.
#pragma comment ( lib, "dxguid.lib" )	// �G�t�F�N�g�p.


enum class I3DL2_ENV_PRESET : int
{
	DEFAULT,		// �W��.
	GENERIC,		// ���.
	PADDEDCELL,		// �N�b�V�����ǂ̂��镔��.
	ROOM,			// ����.
	BATHROOM,		// �o�X���[��.
	LIVINGROOM,		// ���r���O���[��.
	STONEROOM,		// �Α���̕���.
	AUDITORIUM,		// �u��.
	CONCERTHALL,	// �R���T�[�g�z�[��.
	CAVE,			// ���A.
	ARENA,			// ���Z��.
	HANGAR,			// �i�[��.
	CARPETEDHALLWAY,// �O�~(���イ����)�~�̒ʘH.
	HALLWAY,		// �ʘH�E�L��.
	STONECORRIDOR,	// �Α���̘L��.
	ALLEY,			// �H�n.
	FOREST,			// �X.
	CITY,			// �X�E�s��.
	MOUNTAINS,		// �R��.
	QUARRY,			// �̐Ώ�
	PLAIN,			// ����.
	PARKINGLOT,		// ���ԏ�.
	SEWERPIPE,		// �����Ǔ�.
	UNDERWATER,		// ����.
	SMALLROOM,		// ������.
	MEDIUMROOM,		// ������.
	LARGEROOM,		// �啔��.
	MEDIUMHALL,		// ���z�[��.
	LARGEHALL,		// ��z�[��.
	PLATE,			// �v���[�g���o�[�u�̃V�~�����[�g.

	MAX,
	NONE = -1,		// ���ݒ�.
};


//============================================================================
//		Sound�N���X.
//============================================================================
class CSound
{
public:
	const LONG VOLUME_DEFAULT;		// ���ʏ����l(db).

	CSound();	// �R���X�g���N�^.
	CSound(LONG VolumeDefault)
		: VOLUME_DEFAULT(VolumeDefault), m_pDS(), m_pDSBuffer() {};	// �R���X�g���N�^(�������ʐݒ�\��).
	~CSound();	// �f�X�g���N�^.

	// Sound�f�o�C�X�쐬�E���.
	HRESULT CreateDevice( HWND hWnd );
	HRESULT ReleaseDevice();

	// �f�[�^�Ǎ��E����֐�.
	HRESULT LoadData( LPSTR szFileName );
	HRESULT ReleaseData();

	// ����֐�.
	HRESULT Release();

	// ��ɍŏ�����Đ�����.
	HRESULT Play( const GUID *pGuid = nullptr );
	// ���[�v�Đ�.
	void PlayLoop( const GUID *pGuid = nullptr );
	// �Đ������m�F����.
	bool IsPlaying();

	// ��ɍŏ�����G�t�F�N�g�t���ōĐ�����.
	HRESULT PlayEx( I3DL2_ENV_PRESET preset );
	// �G�t�F�N�g�t���Ń��[�v�Đ�.
	void playLoopEx( I3DL2_ENV_PRESET preset );

	// ���̒�~(�Đ��ʒu��0�ɂ���).
	HRESULT Stop();
	// ���̒�~(�Đ��ʒu��0�ɂ��Ȃ�).
	HRESULT MiniStop();

	// �p���j���O��ݒ肷��.
	//	�p���j���O = ���E�ǂ��炩�ɉ���΂点��.
	//	�� = -10000�A�E = 10000.
	void SetPan( LONG pan );

	// ���g����ݒ肷��.
	//	���̍������ς��.
	void SetFrequency( DWORD freq );

	// ���ʂ�ݒ肷��.
	void SetVolume( LONG volume );

	// ���ʐݒ�̂��߂̔��]����.
	LONG Mapvol(LONG volume);
	
	// ���g�����擾����.
	DWORD GetFrequency();
	// �p���j���O���擾����.
	LONG GetPan();
	// ���ʂ��擾����.
	LONG GetVolume();
	// �G�t�F�N�g�p�����[�^�̎擾.
	DWORD GetPreset( I3DL2_ENV_PRESET presetNo );

private:
	LPDIRECTSOUND8			m_pDS;			// DirectSound�I�u�W�F�N�g.
	LPDIRECTSOUNDBUFFER8	m_pDSBuffer;	// DS�Z�J���_���o�b�t�@.

	// �Đ��֐�.
	HRESULT PlayBase( const GUID *pGuid, DWORD preset = -1 );
	// �G�t�F�N�g�p�����[�^��ݒ�E�K�p����.
	HRESULT SetEffectParam( LPDIRECTSOUNDBUFFER8 pDSBuffer8, DWORD dwPreset );
	
	// LPSTR�^��LPWSTR�^�ɃL���X�g����֐�
	LPWSTR ConvertLPSTRToLPWSTR(LPSTR lpstr);

	// �p���j���O�A���g���A���ʂ��͈͊O�ɐݒ肳��Ȃ��悤�ɂ���֐�.
	LONG  RoundingPan( LONG pan );
	DWORD RoundingFrequency( DWORD freq );
	LONG  RoundingVolume( LONG volume );
};