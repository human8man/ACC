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
	friend class CSingleton<CSpriteManager>;// シングルトンクラスをフレンド宣言.

	CSpriteManager();
	~CSpriteManager();

	// スプライトの読み込み.
	HRESULT SpriteLoad();

	// スプライト情報を取得.
	CSprite2D* GetSprite(const std::string& filename);
	// スプライトのパスを取得.
	std::string GetFilePath(const std::string& filename);

	// 保存しているスプライトの名前リストの取得.
	Sprite_List GetSpriteNames() { return GetInstance()->m_SpriteNames; }


private:
	Sprite_map	m_SpriteList;	// スプライトリスト.
	Sprite_List	m_SpriteNames;	// スプライトの名前リスト.
};