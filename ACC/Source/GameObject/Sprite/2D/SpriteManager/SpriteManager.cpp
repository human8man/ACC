
#include "Sprite/2D/SpriteManager/SpriteManager.h"


namespace {
	constexpr char FILE_PATH[] = "Data\\Texture"; // �X�v���C�g�t�@�C���p�X.
}


//=============================================================================
//		CSpriteManager�N���X. 
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
//		�X�v���C�g�ǂݍ���.
//=============================================================================
HRESULT CSpriteManager::SpriteLoad()
{
	auto SpriteLoad = [&](const std::filesystem::directory_entry& Entry)
		{
			const std::string Extension = Entry.path().extension().string();	// �g���q.
			const std::string FilePath = Entry.path().string();					// �t�@�C���p�X.
			const std::string FileName = Entry.path().stem().string();			// �t�@�C����.

			// �g���q��png,bmp,jpg�łȂ��ꍇ�ǂݍ��܂Ȃ�.
			if (Extension != ".png" && Extension != ".PNG" &&
				Extension != ".bmp" && Extension != ".BMP" &&
				Extension != ".jpg" && Extension != ".JPG") return;

			// �X�v���C�g�t�@�C���̓ǂݍ���.
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
//		�������.
//=============================================================================
void CSpriteManager::Release()
{
	m_SpriteList.clear();
	m_SpriteNames.clear();
}


//=============================================================================
//		�X�v���C�g�����擾.
//=============================================================================
CSprite2D* CSpriteManager::GetSprite(const std::string& filename)
{
	// �w�肵���X�v���C�g���擾.
	for (auto& s : GetInstance()->m_SpriteList) {
		if (s.first != filename) continue;
		return s.second.get();
	}

	return nullptr;
}


//=============================================================================
//		�X�v���C�g�̃p�X���擾.
//=============================================================================
std::string CSpriteManager::GetFilePath(const std::string& filename)
{
	// �w�肵���X�v���C�g���擾.
	for (auto& s : GetInstance()->m_SpriteList) {
		if (s.first != filename) continue;
		return s.second->GetSpriteData().Path;
	}

	return std::string();
}