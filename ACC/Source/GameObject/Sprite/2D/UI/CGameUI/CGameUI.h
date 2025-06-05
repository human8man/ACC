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
	// ��_���G�t�F�N�g����.
	void InitHiteffect();
	void Hiteffect(CUIObject* object);

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
	
	bool	m_PlayerDamage;				// ��_���t���O.
	bool	m_ChangedRed;				// �F���ω���.
	float	m_HitEffectColor;			// �F���ω��p.
	float	m_HitEffectAlpha;			// ��_�����ߒl.
	float	m_HitEffectTime;			// ��_���o�ߎ���.
	float	m_HitEffectTimeMax;			// ��_���ő厞��.
	float	m_HitEffectChangeTime;		// ��_���ϐF�o�ߎ���.
	float	m_HitEffectChangeTimeMax;	// ��_���ϐF�ő厞��.
	float	m_HitEffectDeleteTime;		// ��_���폜�o�ߎ���.
	float	m_HitEffectDeleteTimeMax;	// ��_���폜�ő厞��.
};