#pragma once

#include "Sprite/2D/CSprite2D.h"
#include "Singleton/CSingleton.h"


class CSpriteManager
	: public CSingleton<CSpriteManager>
{
public:
	using Sprite_map = std::unordered_map<std::string, std::shared_ptr<CSprite2D>>;
	using Sprite_List = std::vector<std::string>;

public:
	friend class CSingleton<CSpriteManager>;// �V���O���g���N���X���t�����h�錾.

	CSpriteManager();
	~CSpriteManager();

	// �X�v���C�g�̓ǂݍ���.
	HRESULT SpriteLoad();

	// �X�v���C�g�����擾.
	CSprite2D* GetSprite(const std::string& filename);
	// �X�v���C�g�̃p�X���擾.
	std::string GetFilePath(const std::string& filename);

	// �ۑ����Ă���X�v���C�g�̖��O���X�g�̎擾.
	Sprite_List GetSpriteNames() { return GetInstance()->m_SpriteNames; }


private:
	Sprite_map	m_SpriteList;	// �X�v���C�g���X�g.
	Sprite_List	m_SpriteNames;	// �X�v���C�g�̖��O���X�g.
};