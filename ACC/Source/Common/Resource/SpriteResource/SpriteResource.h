#pragma once
#include "Global.h"
#include "Sprite/2D/CSprite2D.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include <filesystem>

class CSprite2D;

class CSpriteResource final
{
public:
	using Sprite_map	= std::unordered_map<std::string, std::shared_ptr<CSprite2D>>;
	using Sprite_List	= std::vector<std::string>;

public:
	CSpriteResource();
	~CSpriteResource();

	// インスタンスの取得.
	static CSpriteResource* GetInstance();

	// スプライトの読み込み.
	static HRESULT SpriteLoad();

	// スプライト取得関数.
	static CSprite2D* GetSprite( 
			const std::string& FileName ,
			CSprite2D::SPRITE_STATE *pState = nullptr);

	static CSprite2D* GetSprite(
		const std::string& FileName,
		std::vector<CSprite2D::SPRITE_STATE> *pState);

	// 保存しているスプライトの名前リストの取得.
	static Sprite_List GetSpriteNames() { return GetInstance()->m_SpriteNames; }

private:
	Sprite_map	m_SpriteList;	// スプライトリスト.
	Sprite_List	m_SpriteNames;	// スプライトの名前リスト.
	std::mutex	m_Mutex;
	bool		m_IsLoadEnd;	// 読み込みが終わったか.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CSpriteResource( const CSpriteResource& )				= delete;
	CSpriteResource& operator= ( const CSpriteResource& )	= delete;
	CSpriteResource( CSpriteResource&& )					= delete;
	CSpriteResource& operator = ( CSpriteResource&& )		= delete;
};