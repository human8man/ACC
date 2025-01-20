#pragma once

#include "GameObject/Sprite/2D/UI/CUIObject.h"

class CGameUI
{
public:
	// �摜��ނ��Ƃɔԍ���U�蓖�Ă�.
	enum FadeSprite {
		Black,
		Gray,
		Number,
		White,
	};

public:
	CGameUI();
	~CGameUI();

	void Create();
	void Update();
	void Draw();
	void Release();

	// �t�F�[�h���J�n����(�����\������Ă��鎞��).
	void DoFade(int peaktime) { m_FadeStart = true; m_PeakCnt = peaktime; }

	// �l��Ԃ��֐�.
	bool GetFading()	const { return m_Fading; } // �t�F�[�h������m�点��.
	bool GetFadePeak()	const { return m_FadePeak; } // �t�F�[�h�̃s�[�N��m�点��.
	bool GetFadeEnd()	const { return m_FadeEnd; } // �t�F�[�h�̏I����m�点��.

private:
	std::vector<std::string> m_SpriteDataList;	//�X�v���C�g�����܂Ƃ߂�z��.
	std::vector<D3DXVECTOR3> m_SpritePosList;	//�X�v���C�g���W���܂Ƃ߂�z��.

	std::vector<CUIObject*> m_pUIs;			// UI�N���X.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2D�N���X.

	bool	m_FadeStart;	// �t�F�[�h�J�n.
	bool	m_Fading;		// �t�F�[�h��.
	bool	m_FadePeak;		// �t�F�[�h���.
	bool	m_FadeEnd;		// �t�F�[�h�I��.
	bool	m_Peaking;		// �s�[�N���Ԓ�.

	float	m_FadeAlpha;	// �t�F�[�h�̃A���t�@.
	float	m_AddAlpha;		// �t�F�[�h�̃A���t�@�ɒǉ�����l.
	float	m_AAVMAXVAL;	// �t�F�[�h�̃A���t�@�ɒǉ�����l�̒萔��.

	int		m_PeakCnt;		// �s�[�N�ɒB���Ă��鎞�Ԃ̃J�E���g.
};