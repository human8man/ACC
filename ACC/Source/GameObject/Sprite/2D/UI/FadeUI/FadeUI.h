#pragma once

#include "Sprite/2D/UI/UIObject.h"
#include "Time/CTime.h"


class FadeUI
{
public:
	FadeUI();
	 ~FadeUI();

	void Create();	// �쐬����
	void Update();	// �X�V����
	void Draw();	// �`�揈��
	
	/****************************************************************
	// @brief �t�F�[�h�J�n����(1�b = 60�t���[��)
	// @param in ���ɂȂ�܂ł̎���
	// @param peak �����\������鎞��
	// @param out ����������܂łɂ����鎞��
	****************************************************************/
	void DoFade(int in,int peak,int out);


	bool GetFading()		const { return m_Fading; }		// �t�F�[�h������m�点��
	bool GetFirstPeak()		const { return m_FirstPeak; }	// �t�F�[�h�̏���s�[�N��m�点��
	bool GetFadePeak()		const { return m_Peaking; }		// �t�F�[�h�̃s�[�N����m�点��
	bool GetFadeEnd()		const { return m_End; }			// �t�F�[�h�̏I����m�点��
	bool GetFadeEndFrame()	const { return m_EndFrame; }	// �t�F�[�h�̏I���t���[����m�点��

private:
	std::unique_ptr<UIObject>	m_pBlack;	// UI�N���X
	Sprite2D*	m_pSprite2D;			// Sprite2D�N���X

	bool	m_Fading;		// �t�F�[�h��
	bool	m_FirstPeak;	// ����s�[�N
	bool	m_Peak;			// �s�[�N���O
	bool	m_BeforePeak;	// �O�t���[���Ƀs�[�N��������
	bool	m_End;			// �I��
	bool	m_EndFrame;		// �I���t���[��
	bool	m_Peaking;		// �s�[�N���Ԓ�

	float	m_FadeAlpha;	// �A���t�@
	float	m_AddAlpha;		// �A���t�@�ɒǉ�����l
	float	m_InAddAlpha;	// �t�F�[�h�C�����̃A���t�@�ɒǉ�����l
	float	m_OutAddAlpha;	// �t�F�[�h�A�E�g���̃A���t�@�ɒǉ�����l

	float	m_PeakCnt;	// �s�[�N���Ԃ̃J�E���g
};