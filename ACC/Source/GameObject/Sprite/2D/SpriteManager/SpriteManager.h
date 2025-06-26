#pragma once

#include "Sprite/2D/Sprite2D.h"
#include "Sprite/3D/Sprite3D.h"
#include "Singleton/CSingleton.h"


class CSpriteManager
	: public CSingleton<CSpriteManager>
{
public:
	using Sprite2D_map = std::unordered_map<std::string, std::shared_ptr<Sprite2D>>;
	using Sprite3D_map = std::unordered_map<std::string, std::shared_ptr<Sprite3D>>;
	using Sprite_List = std::vector<std::string>;

public:
	friend class CSingleton<CSpriteManager>;// �V���O���g���N���X���t�����h�錾

	CSpriteManager();
	~CSpriteManager();

	// �X�v���C�g�̓ǂݍ���
	HRESULT SpriteLoad();
	// �������
	void Release();

	// �X�v���C�g�����擾
	Sprite3D* Get3DSprite(const std::string& filename);
	Sprite2D* Get2DSprite(const std::string& filename);
	Sprite3D* Clone3DSprite(const std::string& filename);
	Sprite2D* Clone2DSprite(const std::string& filename);
	// �X�v���C�g�̃p�X���擾
	std::string GetFilePath(const std::string& filename);

	// �ۑ����Ă���X�v���C�g�̖��O���X�g�̎擾
	Sprite_List GetSpriteNames() { return GetInstance()->m_SpriteNames; }

private:
	Sprite2D_map	m_Sprite2DList;	// �X�v���C�g2D���X�g
	Sprite3D_map	m_Sprite3DList;	// �X�v���C�g3D���X�g

	Sprite_List		m_SpriteNames;	// �X�v���C�g�̖��O���X�g
};