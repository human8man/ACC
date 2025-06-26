#include "SpriteManager.h"


namespace {
	constexpr char FILE_PATH[] = "Data\\Texture"; // スプライトファイルパス
}


CSpriteManager::CSpriteManager()
	: m_Sprite2DList()
	, m_Sprite3DList()
	, m_SpriteNames()
{
}
CSpriteManager::~CSpriteManager()
{
}


//=============================================================================
//		スプライト読み込み
//=============================================================================
HRESULT CSpriteManager::SpriteLoad()
{
	auto SpriteLoad = [&](const std::filesystem::directory_entry& Entry)
		{
			const std::string Extension = Entry.path().extension().string();	// 拡張子
			const std::string FilePath = Entry.path().string();					// ファイルパス
			const std::string FileName = Entry.path().stem().string();			// ファイル名

			// 拡張子がpng,bmp,jpgでない場合読み込まない
			if (Extension != ".png" && Extension != ".PNG" &&
				Extension != ".bmp" && Extension != ".BMP" &&
				Extension != ".jpg" && Extension != ".JPG") return;

			// スプライトファイルの読み込み
			m_Sprite2DList[FileName] = std::make_unique<Sprite2D>();
			if (FAILED(m_Sprite2DList[FileName]->Init(FilePath))) throw E_FAIL;
			// スプライトファイルの読み込み
			m_Sprite3DList[FileName] = std::make_unique<Sprite3D>();
			if (FAILED(m_Sprite2DList[FileName]->Init(FilePath))) throw E_FAIL;

			m_SpriteNames.emplace_back(FileName);
		};

	try {
		std::filesystem::recursive_directory_iterator Dir_itr(FILE_PATH), End_itr;
		std::for_each(Dir_itr, End_itr, SpriteLoad);
	}
	catch (const std::filesystem::filesystem_error&) { 
		return E_FAIL;
	}

	return S_OK;
}


//=============================================================================
//		解放処理
//=============================================================================
void CSpriteManager::Release()
{
	m_Sprite2DList.clear();
	m_Sprite3DList.clear();
	m_SpriteNames.clear();
}


//=============================================================================
//		スプライト情報を取得
//=============================================================================
Sprite3D* CSpriteManager::Get3DSprite(const std::string& filename)
{
	// 指定したスプライトを取得
	for (auto& s : GetInstance()->m_Sprite3DList) {
		if (s.first != filename) continue;
		return s.second.get();
	}

	return nullptr;
}
Sprite2D* CSpriteManager::Get2DSprite(const std::string& filename)
{
	// 指定したスプライトを取得
	for (auto& s : GetInstance()->m_Sprite2DList) {
		if (s.first != filename) continue;
		return s.second.get();
	}

	return nullptr;
}
//=============================================================================
//		スプライト情報を取得(クローン)
//=============================================================================
Sprite3D* CSpriteManager::Clone3DSprite(const std::string& filename)
{
	auto* src = Get3DSprite(filename);
	if (!src) return nullptr;

	Sprite3D* clone = new Sprite3D(*src);
	return clone;
}
Sprite2D* CSpriteManager::Clone2DSprite(const std::string& filename)
{
	auto* src = Get2DSprite(filename);
	if (!src) return nullptr;

	Sprite2D* clone = new Sprite2D(*src);
	return clone;
}


//=============================================================================
//		スプライトのパスを取得
//=============================================================================
std::string CSpriteManager::GetFilePath(const std::string& filename)
{
	// 指定したスプライトを取得
	for (auto& s : GetInstance()->m_Sprite2DList) {
		if (s.first != filename) continue;
		return s.second->GetSpriteData().Path;
	}

	return std::string();
}