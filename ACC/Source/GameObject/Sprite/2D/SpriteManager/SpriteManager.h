#pragma once

#include "Sprite/2D/CSprite2D.h"
#include "Sprite/3D/CSprite3D.h"
#include "Singleton/CSingleton.h"


class CSpriteManager
	: public CSingleton<CSpriteManager>
{
public:
	using Sprite2D_map = std::unordered_map<std::string, std::shared_ptr<CSprite2D>>;
	using Sprite3D_map = std::unordered_map<std::string, std::shared_ptr<CSprite3D>>;
	using Sprite_List = std::vector<std::string>;

public:
	friend class CSingleton<CSpriteManager>;// シングルトンクラスをフレンド宣言.

	CSpriteManager();
	~CSpriteManager();

	// スプライトの読み込み.
	HRESULT SpriteLoad();
	// 解放処理.
	void Release();

	// スプライト情報を取得.
	CSprite3D* Get3DSprite(const std::string& filename);
	CSprite2D* Get2DSprite(const std::string& filename);
	CSprite3D* Clone3DSprite(const std::string& filename);
	CSprite2D* Clone2DSprite(const std::string& filename);
	// スプライトのパスを取得.
	std::string GetFilePath(const std::string& filename);

	// 保存しているスプライトの名前リストの取得.
	Sprite_List GetSpriteNames() { return GetInstance()->m_SpriteNames; }

private:
	Sprite2D_map	m_Sprite2DList;	// スプライト2Dリスト.
	Sprite3D_map	m_Sprite3DList;	// スプライト3Dリスト.

	Sprite_List		m_SpriteNames;	// スプライトの名前リスト.
};