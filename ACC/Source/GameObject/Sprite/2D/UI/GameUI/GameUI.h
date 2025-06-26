#pragma once

#include "Sprite/2D/UI/UIObject.h"
#include "Character/Player/CPlayer.h"


class GameUI
	:public UIObject
{
public:
	GameUI();
	~GameUI();

	// �쐬����
	void Create();
	// �X�V����
	void Update(std::unique_ptr<CPlayer>& chara);
	// �`�揈��
	void Draw(); 
	// �������
	void Release(); 

	// �X���[UI�ݒ�֐�
	void SetSlowUI(bool flag) { m_Slow = flag; }
private:
	// ��_���G�t�F�N�g����
	void InitHiteffect();
	void Hiteffect(UIObject* object);

	// �N���X�w�A����
	void Crosshair(UIObject* object);

private:
	int		m_HP;				// HP
	int		m_HPMax;			// �ő�HP
	int		m_Ammo;				// �c�e��
	int		m_HitKind;			// Hit�̎��

	float	m_ReloadRot;		// �����[�h��]
	float	m_ReloadTime;		// �����[�h����
	float	m_ReloadTimeMax;	// �����[�h�ő厞��

	float	m_ViewHitTime;		// �q�b�g�\������
	float	m_ViewHitTimeMax;	// �q�b�g�ő�\������

	bool	m_AutoAim;		// �I�[�g�G�C��
	bool	m_Homing;		// �z�[�~���O
	bool	m_WallHack;		// �E�H�[���n�b�N
	bool	m_TriggerHappy;	// �A�˃��[�h
	bool	m_Slow;			// �X���[���[�h

	// �N���X�w�A�̃C�[�W���O�Ɏg�p
	float	m_CrosshairEase;
	float	m_CrosshairEaseMax;
	float	m_CrosshairRot;

	// �_���[�W����HP�����Ɏg�p
	float	m_HPRate;
	float	m_DamageEase;
	float	m_DamageEaseMax;

	bool	m_PlayerDamage;				// ��_���t���O
	bool	m_ChangedRed;				// �F���ω���
	float	m_HitEffectColor;			// �F���ω��p
	float	m_HitEffectAlpha;			// ��_�����ߒl
	float	m_HitEffectTime;			// ��_���o�ߎ���
	float	m_HitEffectTimeMax;			// ��_���ő厞��
	float	m_HitEffectChangeTime;		// ��_���ϐF�o�ߎ���
	float	m_HitEffectChangeTimeMax;	// ��_���ϐF�ő厞��
	float	m_HitEffectDeleteTime;		// ��_���폜�o�ߎ���
	float	m_HitEffectDeleteTimeMax;	// ��_���폜�ő厞��
};