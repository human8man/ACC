#include "SpriteResource.h"
#include "FileManager\FileManager.h"

namespace {
	constexpr char FILE_PATH[] = "Data\\Sprite"; // スプライトファイルパス.
}

CSpriteResource::CSpriteResource()
	: m_SpriteList		()
	, m_SpriteNames		()
	, m_Mutex			()
	, m_IsLoadEnd		( false )
{
}

CSpriteResource::~CSpriteResource()
{
	m_SpriteList.clear();
}

//---------------------------.
// インスタンスの取得.
//---------------------------.
CSpriteResource* CSpriteResource::GetInstance()
{
	static std::unique_ptr<CSpriteResource> pInstance = std::make_unique<CSpriteResource>();
	return pInstance.get();
}

//---------------------------.
// スプライトの読み込み.
//---------------------------.
HRESULT CSpriteResource::SpriteLoad()
{
	CSpriteResource* pI = GetInstance();
	std::unique_lock<std::mutex> Lock( pI->m_Mutex );

	auto SpriteLoad = [&]( const std::filesystem::directory_entry& Entry )
	{
		const std::string Extension = Entry.path().extension().string();	// 拡張子.
		const std::string FilePath	= Entry.path().string();				// ファイルパス.
		const std::string FileName	= Entry.path().stem().string();			// ファイル名.

		// 拡張子が ".midi" / ".mp3" / ".wav" ではない場合読み込まない.
		if ( Extension != ".png" && Extension != ".PNG" &&
			 Extension != ".bmp" && Extension != ".BMP" &&
			 Extension != ".jpg" && Extension != ".JPG" ) return;

		// スプライトファイルの読み込み.
		pI->m_SpriteList[FileName] = std::make_unique<CSprite2D>();
		if ( FAILED( pI->m_SpriteList[FileName]->Init( FilePath ) ) )throw E_FAIL;
		pI->m_SpriteNames.emplace_back( FileName );
	};

	try {
		std::filesystem::recursive_directory_iterator Dir_itr( FILE_PATH ), End_itr;
		std::for_each( Dir_itr, End_itr, SpriteLoad );
	}
	catch ( const std::filesystem::filesystem_error& e ) {
		// エラーメッセージを表示.
		return E_FAIL;
	}

	// 読み込み終了.
	pI->m_IsLoadEnd = true;
	return S_OK;
}

//---------------------------.
// スプライト取得関数.
//---------------------------.
CSprite2D* CSpriteResource::GetSprite( const std::string& FileName ,CSprite2D::SPRITE_STATE *pState )
{
	// 読み込みが終わっていなかったら null を返す.
	if ( GetInstance()->m_IsLoadEnd == false )
		return nullptr;

	// 指定したエフェクトを取得.
	for ( auto& s : GetInstance()->m_SpriteList ) {
		if ( s.first != FileName	) continue;
		if ( pState  != nullptr		) *pState = s.second->GetSpriteData();
		return s.second.get();
	}

	return nullptr;
}
CSprite2D* CSpriteResource::GetSprite( const std::string& FileName ,std::vector<CSprite2D::SPRITE_STATE> *pState )
{
	CSprite2D* Out;
	CSprite2D::SPRITE_STATE	State;
	Out = GetSprite( FileName, &State );
	for ( auto& s : *pState ) s = State;
	return Out;
}