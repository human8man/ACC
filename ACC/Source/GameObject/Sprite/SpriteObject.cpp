#include "SpriteObject.h"
#include "FileManager/FileManager.h"
#include "FileManager/LoadImage/LoadImage.h"


SpriteObject::SpriteObject()
	: m_pSprite		( nullptr )
{
}
SpriteObject::~SpriteObject()
{
	DetachSprite();
}


//==============================================================================
//		�X�V����
//==============================================================================
void SpriteObject::Update()
{
	if( m_pSprite == nullptr ){
		return;
	}
}


//==============================================================================
//		�`�揈��
//==============================================================================
void SpriteObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj )
{
	if( m_pSprite == nullptr ){ return; }

	// �`�撼�O�ō��W���]���Ȃǂ��X�V
	m_pSprite->SetPos( m_vPosition );
	m_pSprite->SetRot( m_vRotation );
	m_pSprite->SetScale( m_vScale );

	// �����_�����O
	m_pSprite->Draw( View, Proj );
}


//-----------------------------------------------------------------------------
//		�q�N���X�𒊏ۃN���X�ɂ��Ȃ����߂ɕK�v
//-----------------------------------------------------------------------------
void SpriteObject::Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light)
{
	Draw(mView, mProj);
}


//=============================================================================
//		�X�v���C�g���̓Ǎ�
//=============================================================================
HRESULT SpriteObject::SpriteStateDataLoad(const std::string& FilePath)
{
	Json m_SpriteStateData = nullptr;	// �摜���

	// �␳�l�e�L�X�g�̓ǂݍ���
	std::string TextPath = FilePath;
	TextPath.erase(TextPath.rfind("\\"), TextPath.size()) += "\\OffSet.json";
	Json OffSetData = FileManager::JsonLoad(TextPath);

	// �������O�̃e�L�X�g�̓ǂݍ���
	TextPath = FilePath;
	TextPath.erase(TextPath.find("."), TextPath.size()) += ".json";
	m_SpriteStateData = FileManager::JsonLoad(TextPath);

	// �t�@�C�����������߃t�@�C�����쐬����
	if (m_SpriteStateData.empty()) {
		if (FAILED(CreateSpriteState(FilePath))) return E_FAIL;

		// �쐬�ł������߂�����x�ǂݍ��ݒ���
		SpriteStateDataLoad(FilePath);
		return S_OK;
	}

	// �摜�̃p�X�̎擾
	if (m_SpriteStateData.contains("FilePath")) {
		if (m_SpriteStateData["FilePath"].is_string()) {
			m_SpriteState.Path = m_SpriteStateData["FilePath"].get<std::string>();
		}
	}

	// �摜�̃t�@�C������ۑ�
	std::string ImageName = FilePath.substr(FilePath.find_last_of("\\/") + 1);
	ImageName = ImageName.substr(0, ImageName.find_last_of(".")); // �g���q�폜
	m_SpriteState.Name = ImageName;

	m_SpriteState.Pos.x = m_SpriteStateData["Pos"]["x"].get<float>();
	m_SpriteState.Pos.y = m_SpriteStateData["Pos"]["y"].get<float>();
	m_SpriteState.Pos.z = m_SpriteStateData["Pos"]["z"].get<float>();
	m_SpriteState.Disp.w = m_SpriteStateData["Disp"]["w"];
	m_SpriteState.Disp.h = m_SpriteStateData["Disp"]["h"];
	m_SpriteState.Base.w = m_SpriteStateData["Base"]["w"];
	m_SpriteState.Base.h = m_SpriteStateData["Base"]["h"];
	m_SpriteState.Stride.w = m_SpriteStateData["Stride"]["w"];
	m_SpriteState.Stride.h = m_SpriteStateData["Stride"]["h"];

	m_vColor.x = m_SpriteStateData["Color"]["x"].get<float>();
	m_vColor.y = m_SpriteStateData["Color"]["y"].get<float>();
	m_vColor.z = m_SpriteStateData["Color"]["z"].get<float>();
	m_Alpha = m_SpriteStateData["Alpha"];

	m_vScale.x = m_SpriteStateData["Scale"]["x"].get<float>();
	m_vScale.y = m_SpriteStateData["Scale"]["y"].get<float>();
	m_vScale.z = m_SpriteStateData["Scale"]["z"].get<float>();

	m_vPivot.x = m_SpriteStateData["Pivot"]["x"].get<float>();
	m_vPivot.y = m_SpriteStateData["Pivot"]["y"].get<float>();
	m_vPivot.z = m_SpriteStateData["Pivot"]["z"].get<float>();

	m_vRotation.x = m_SpriteStateData["Rotate"]["x"].get<float>();
	m_vRotation.y = m_SpriteStateData["Rotate"]["y"].get<float>();
	m_vRotation.z = m_SpriteStateData["Rotate"]["z"].get<float>();

	// �t�@�C���p�X���X�V����
	m_SpriteStateData["FilePath"] = TextPath;
	if (FAILED(FileManager::JsonSave(TextPath, m_SpriteStateData))) return E_FAIL;

	return S_OK;
}


//=============================================================================
//		�X�v���C�g�����܂Ƃ߂�json�t�@�C���̍쐬
//=============================================================================
HRESULT SpriteObject::CreateSpriteState(const std::string& FilePath)
{
	// �쐬����t�@�C���p�X
	std::string TextPath = FilePath;
	TextPath.erase(TextPath.find("."), TextPath.size()) += ".json";

	// �摜�̕��A�����̎擾
	const D3DXVECTOR2& BaseSize = LoadImageFile::GetImageSize(FilePath);

	// �摜���̃f�t�H���g�̒l��ǉ����Ă���
	Json SpriteState;
	SpriteState["Pos"]["x"] = 0.0;
	SpriteState["Pos"]["y"] = 0.0;
	SpriteState["Pos"]["z"] = 0.0;
	SpriteState["Disp"]["w"] = BaseSize.x;
	SpriteState["Disp"]["h"] = BaseSize.y;
	SpriteState["Base"]["w"] = BaseSize.x;
	SpriteState["Base"]["h"] = BaseSize.y;
	SpriteState["Stride"]["w"] = BaseSize.x;
	SpriteState["Stride"]["h"] = BaseSize.y;

	SpriteState["Color"]["x"] = 1.0;
	SpriteState["Color"]["y"] = 1.0;
	SpriteState["Color"]["z"] = 1.0;
	SpriteState["Alpha"] = 1.0;

	SpriteState["Scale"]["x"] = 1.0;
	SpriteState["Scale"]["y"] = 1.0;
	SpriteState["Scale"]["z"] = 1.0;
	SpriteState["Pivot"]["x"] = 0.0;
	SpriteState["Pivot"]["y"] = 0.0;
	SpriteState["Pivot"]["z"] = 0.0;
	SpriteState["Rotate"]["x"] = 0.0;
	SpriteState["Rotate"]["y"] = 0.0;
	SpriteState["Rotate"]["z"] = 0.0;

	// �X�v���C�g���̍쐬
	if (FAILED(FileManager::JsonSave(TextPath, SpriteState))) return E_FAIL;

	// �쐬����
	return S_OK;
}