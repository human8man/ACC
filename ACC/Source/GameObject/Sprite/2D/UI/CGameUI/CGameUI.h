#pragma once

#include "Sprite/2D/UI/CUIObject.h"
#include "Character/Player/CPlayer.h"

class CGameUI
	:public CUIObject
{
public:
	CGameUI();
	~CGameUI();

	// �쐬����.
	void Create();
	// �X�V����.
	void Update(std::unique_ptr<CPlayer>& chara);
	// �`�揈��.
	void Draw(); 
	// �������.
	void Release(); 

private:
	int		m_HP;				// HP.
	int		m_HPMax;			// �ő�HP.
	int		m_Ammo;				// �c�e��.
	int		m_HitKind;			// Hit�̎��.
	float	m_ReloadTime;		// �����[�h����.
	float	m_ViewHitTime;		// �q�b�g�\������.
	float	m_ViewHitTimeMax;	// �q�b�g�ő�\������.

	bool	m_AutoAim;		// �I�[�g�G�C��.
	bool	m_Homing;		// �z�[�~���O.
	bool	m_WallHack;		// �E�H�[���n�b�N.
	bool	m_TriggerHappy;	// �A�˃��[�h.
};