#pragma once
#include "CSound.h"
#include "Common/Singleton/CSingleton.h"

//=============================================
//		�T�E���h�}�l�[�W���[�N���X.
//=============================================
class CSoundManager
	: public CSingleton<CSoundManager>
{
private:
	friend class CSingleton<CSoundManager>; // �V���O���g���N���X���t�����h�錾.

public:
	// �T�E���h���X�g�񋓌^.
	enum enList
	{
		SE_Jump,		// �W�����v.
		SE_JumpEnd,		// �W�����v�I��.
		SE_JumpPad,		// �W�����v�p�b�h.
		SE_Wire,		// ���C���[.
		SE_CanSliding,	// �X���C�f�B���O�\.
		SE_Sliding,		// �X���C�f�B���O.
		SE_Move,		// �ړ��p.
		SE_WallJump,	// �ǃW�����v.
		SE_Pause,		// �ꎞ��~��
		SE_Decision,	// ����
		SE_GameStart,	// �Q�[���J�n
		SE_SelectMove,	// �I�����ړ�
		SE_GameOver,	// �Q�[���I�[�o�[
		SE_GameClear,	// �Q�[���N���A.
		BGM_GameMain,	// �Q�[�����C��.
		BGM_Title,		// �^�C�g��.

		ImGui_Add,		// �ǉ�.
		ImGui_Delete,	// �폜.
		ImGui_Ground,	// ��.
		ImGui_JumpPad,	// �W�����v�p�b�h.
		ImGui_Laser,	// ���[�U�[.
		ImGui_Save,		// �ۑ�.
		ImGui_Select,	// �I��.
		ImGui_Wall,		// ��.

		// ������������u�����v�ɒǉ����Ă�������.
		max, // �ő吔.
	};

	struct SoundList {
		enList listNo;					// enList�񋓌^��ݒ�.
		std::string path;				// �t�@�C���̖��O(�p�X�t��).
		std::string alias[32];			// �G�C���A�X��.

		LONG defaultVolume;				// �f�t�H���g����(-10000 ~ 0).
	};

public:
	CSoundManager();
	~CSoundManager();


	HRESULT Create(CSound** ppSound, std::string filePath, HWND hWnd);
	bool Load( HWND hWnd );
	void Release();

	
	//���ʂ�K�p������.
	void ApplyVolumeSetting();
	
	//SE���Đ�����.
	static void PlaySE( enList list ) {
		CSoundManager::GetInstance()->m_pSound[list]->Play();
	}
	//���[�v�Đ�����.
	static void PlayLoop( enList list ) {
		CSoundManager::GetInstance()->m_pSound[list]->PlayLoop();
	}
	//��~����.
	static void Stop( enList list ) {
		CSoundManager::GetInstance()->m_pSound[list]->Stop();
	}
	static void MiniStop( enList list ) {
		CSoundManager::GetInstance()->m_pSound[list]->MiniStop();
	}

	// �Đ��������m�F.
	static bool IsPlaying( enList list ){
		return CSoundManager::GetInstance()->m_pSound[list]->IsPlaying();
	}

	// ��ɍŏ�����Đ�����.
	static void PlayEx( enList list, I3DL2_ENV_PRESET preset ) {
		CSoundManager::GetInstance()->m_pSound[list]->PlayEx(preset);
	}
	// ���[�v�Đ�.
	static void playLoopEx( enList list, I3DL2_ENV_PRESET preset ) {
		CSoundManager::GetInstance()->m_pSound[list]->playLoopEx(preset);
	}


	// �p���j���O��ݒ肷��.
	static void SetPan( enList list,LONG pan ) {
		CSoundManager::GetInstance()->m_pSound[list]->SetPan(pan);
	}
	// ���g����ݒ肷��.
	static void SetFrequency( enList list,DWORD freq ) {
		CSoundManager::GetInstance()->m_pSound[list]->SetFrequency(freq);
	}
	// ���ʂ�ݒ肷��.
	static void SetVolume( enList list,LONG volume ) {
		CSoundManager::GetInstance()->m_pSound[list]->SetVolume(volume);
	}

	// �p���j���O��ݒ肷��.
	static LONG GetPan( enList list ) {
		return CSoundManager::GetInstance()->m_pSound[list]->GetPan();
	}
	// ���g�����擾����.
	static DWORD GetFrequency( enList list ) {
		return CSoundManager::GetInstance()->m_pSound[list]->GetFrequency();
	}
	// ���ʂ��擾����.
	static LONG GetVolume( enList list ) {
		return CSoundManager::GetInstance()->m_pSound[list]->GetVolume();
	}
	// �G�t�F�N�g�p�����[�^�̎擾.
	static void GetPreset(enList list, I3DL2_ENV_PRESET presetNo) {
		CSoundManager::GetInstance()->m_pSound[list]->GetPreset(presetNo);
	}

	// MasterVolume�̎擾.
	float GetMasterVol() { return m_MasterVolume; }
	// MasterVolume�̐ݒ�.
	void  SetMasterVol(float Vol) { m_MasterVolume = Vol; }

	// BGMVolume�̎擾.
	float GetBGMVol() { return m_BGMVolume; }
	// BGMVolume�̐ݒ�.
	void SetBGMVol(float Vol) { m_BGMVolume = Vol; }

	// SEVolume�̎擾.
	float GetSEVol() { return m_SEVolume; }
	//SEVolume�̐ݒ�
	void SetSEVol(float Vol) { m_SEVolume = Vol; }
	
private:
	// string����LPSTR�֕ϊ�.
	LPSTR ConvertStringToLPSTR(const std::string& str);

private:
	std::vector<CSound*> m_pSound;

	std::vector<SoundList> m_SoundList;	//�T�E���h���X�g.
	float m_MasterVolume;
	float m_BGMVolume;
	float m_SEVolume;
};