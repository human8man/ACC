#include "SpriteManager.h"


namespace {
	constexpr char FILE_PATH[] = "Data\\Texture"; // �X�v���C�g�t�@�C���p�X
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
//		�X�v���C�g�ǂݍ���
//=============================================================================
HRESULT CSpriteManager::SpriteLoad()
{
	auto SpriteLoad = [&](const std::filesystem::directory_entry& Entry)
		{
			const std::string Extension = Entry.path().extension().string();	// �g���q
			const std::string FilePath = Entry.path().string();					// �t�@�C���p�X
			const std::string FileName = Entry.path().stem().string();			// �t�@�C����

			// �g���q��png,bmp,jpg�łȂ��ꍇ�ǂݍ��܂Ȃ�
			if (Extension != ".png" && Extension != ".PNG" &&
				Extension != ".bmp" && Extension != ".BMP" &&
				Extension != ".jpg" && Extension != ".JPG") return;

			// �X�v���C�g�t�@�C���̓ǂݍ���
			m_Sprite2DList[FileName] = std::make_unique<Sprite2D>();
			if (FAILED(m_Sprite2DList[FileName]->Init(FilePath))) throw E_FAIL;
			// �X�v���C�g�t�@�C���̓ǂݍ���
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
//		�������
//=============================================================================
void CSpriteManager::Release()
{
	m_Sprite2DList.clear();
	m_Sprite3DList.clear();
	m_SpriteNames.clear();
}


//=============================================================================
//		�X�v���C�g�����擾
//=============================================================================
Sprite3D* CSpriteManager::Get3DSprite(const std::string& filename)
{
	// �w�肵���X�v���C�g���擾
	for (auto& s : GetInstance()->m_Sprite3DList) {
		if (s.first != filename) continue;
		return s.second.get();
	}

	return nullptr;
}
Sprite2D* CSpriteManager::Get2DSprite(const std::string& filename)
{
	// �w�肵���X�v���C�g���擾
	for (auto& s : GetInstance()->m_Sprite2DList) {
		if (s.first != filename) continue;
		return s.second.get();
	}

	return nullptr;
}
//=============================================================================
//		�X�v���C�g�����擾(�N���[��)
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
//		�X�v���C�g�̃p�X���擾
//=============================================================================
std::string CSpriteManager::GetFilePath(const std::string& filename)
{
	// �w�肵���X�v���C�g���擾
	for (auto& s : GetInstance()->m_Sprite2DList) {
		if (s.first != filename) continue;
		return s.second->GetSpriteData().Path;
	}

	return std::string();
}