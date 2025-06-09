
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	constexpr char FILE_PATH[] = "Data\\Texture"; // スプライトファイルパス.
}


//=============================================================================
//		CSpriteManagerクラス. 
//=============================================================================
CSpriteManager::CSpriteManager()
	: m_SpriteList	()
	, m_SpriteNames	()

{
}
CSpriteManager::~CSpriteManager()
{
}


//=============================================================================
//		スプライト読み込み.
//=============================================================================
HRESULT CSpriteManager::SpriteLoad()
{
	auto SpriteLoad = [&](const std::filesystem::directory_entry& Entry)
		{
			const std::string Extension = Entry.path().extension().string();	// 拡張子.
			const std::string FilePath = Entry.path().string();					// ファイルパス.
			const std::string FileName = Entry.path().stem().string();			// ファイル名.

			// 拡張子がpng,bmp,jpgでない場合読み込まない.
			if (Extension != ".png" && Extension != ".PNG" &&
				Extension != ".bmp" && Extension != ".BMP" &&
				Extension != ".jpg" && Extension != ".JPG") return;

			// スプライトファイルの読み込み.
			m_SpriteList[FileName] = std::make_unique<CSprite2D>();
			if (FAILED(m_SpriteList[FileName]->Init(FilePath))) throw E_FAIL;
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
//		解放処理.
//=============================================================================
void CSpriteManager::Release()
{
	m_SpriteList.clear();
	m_SpriteNames.clear();
}


//=============================================================================
//		スプライト情報を取得.
//=============================================================================
CSprite2D* CSpriteManager::GetSprite(const std::string& filename)
{
	// 指定したスプライトを取得.
	for (auto& s : GetInstance()->m_SpriteList) {
		if (s.first != filename) continue;
		return s.second.get();
	}

	return nullptr;
}


//=============================================================================
//		スプライトのパスを取得.
//=============================================================================
std::string CSpriteManager::GetFilePath(const std::string& filename)
{
	// 指定したスプライトを取得.
	for (auto& s : GetInstance()->m_SpriteList) {
		if (s.first != filename) continue;
		return s.second->GetSpriteData().Path;
	}

	return std::string();
}