#pragma once

#include "Sprite/2D/UI/CUIObject.h"

class CGameUI
	:public CUIObject
{
public:
	// �摜��ނ��Ƃɔԍ���U�蓖�Ă�.
	enum GameSprite {
		Bullets,
		Crosshair,
		LowHP,
		Reload
	};

public:
	CGameUI();
	~CGameUI();

	void Create();
	void Update();
	void Draw();
	void Release();

	// Hit���̐ݒ�.
	void SetHit(int hit);

	// �c�e���̐ݒ�.
	void SetAmmo( int val )	{ m_Ammo = val; }
	
	// HP�̐ݒ�.
	void SetHP( int val, int max)	{ m_HP = val; m_HPMax = max;}

	// �����[�h���Ԃ̐ݒ�.
	void SetReloadTime( float time ){ m_ReloadTime = time; }

private:
	std::vector<std::string> m_SpriteDataList;	//�X�v���C�g�����܂Ƃ߂�z��.
	std::vector<D3DXVECTOR3> m_SpritePosList;	//�X�v���C�g���W���܂Ƃ߂�z��.

	std::vector<CUIObject*> m_pUIs;			// UI�N���X.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2D�N���X.

	int		m_HP;				// HP.
	int		m_HPMax;			// �ő�HP.
	int		m_Ammo;				// �c�e��.
	int		m_HitKind;			// Hit�̎��.
	float	m_ReloadTime;		// �����[�h����.
	float	m_ViewHitTime;		// �q�b�g�\������.
	float	m_ViewHitTimeMax;	// �q�b�g�ő�\������.
};