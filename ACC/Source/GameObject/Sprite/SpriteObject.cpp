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
//		更新処理
//==============================================================================
void SpriteObject::Update()
{
	if( m_pSprite == nullptr ){
		return;
	}
}


//==============================================================================
//		描画処理
//==============================================================================
void SpriteObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj )
{
	if( m_pSprite == nullptr ){ return; }

	// 描画直前で座標や回転情報などを更新
	m_pSprite->SetPos( m_vPosition );
	m_pSprite->SetRot( m_vRotation );
	m_pSprite->SetScale( m_vScale );

	// レンダリング
	m_pSprite->Draw( View, Proj );
}


//-----------------------------------------------------------------------------
//		子クラスを抽象クラスにしないために必要
//-----------------------------------------------------------------------------
void SpriteObject::Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light)
{
	Draw(mView, mProj);
}


//=============================================================================
//		スプライト情報の読込
//=============================================================================
HRESULT SpriteObject::SpriteStateDataLoad(const std::string& FilePath)
{
	Json m_SpriteStateData = nullptr;	// 画像情報

	// 補正値テキストの読み込み
	std::string TextPath = FilePath;
	TextPath.erase(TextPath.rfind("\\"), TextPath.size()) += "\\OffSet.json";
	Json OffSetData = FileManager::JsonLoad(TextPath);

	// 同じ名前のテキストの読み込み
	TextPath = FilePath;
	TextPath.erase(TextPath.find("."), TextPath.size()) += ".json";
	m_SpriteStateData = FileManager::JsonLoad(TextPath);

	// ファイルが無いためファイルを作成する
	if (m_SpriteStateData.empty()) {
		if (FAILED(CreateSpriteState(FilePath))) return E_FAIL;

		// 作成できたためもう一度読み込み直す
		SpriteStateDataLoad(FilePath);
		return S_OK;
	}

	// 画像のパスの取得
	if (m_SpriteStateData.contains("FilePath")) {
		if (m_SpriteStateData["FilePath"].is_string()) {
			m_SpriteState.Path = m_SpriteStateData["FilePath"].get<std::string>();
		}
	}

	// 画像のファイル名を保存
	std::string ImageName = FilePath.substr(FilePath.find_last_of("\\/") + 1);
	ImageName = ImageName.substr(0, ImageName.find_last_of(".")); // 拡張子削除
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

	// ファイルパスを更新する
	m_SpriteStateData["FilePath"] = TextPath;
	if (FAILED(FileManager::JsonSave(TextPath, m_SpriteStateData))) return E_FAIL;

	return S_OK;
}


//=============================================================================
//		スプライト情報をまとめたjsonファイルの作成
//=============================================================================
HRESULT SpriteObject::CreateSpriteState(const std::string& FilePath)
{
	// 作成するファイルパス
	std::string TextPath = FilePath;
	TextPath.erase(TextPath.find("."), TextPath.size()) += ".json";

	// 画像の幅、高さの取得
	const D3DXVECTOR2& BaseSize = LoadImageFile::GetImageSize(FilePath);

	// 画像情報のデフォルトの値を追加していく
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

	// スプライト情報の作成
	if (FAILED(FileManager::JsonSave(TextPath, SpriteState))) return E_FAIL;

	// 作成成功
	return S_OK;
}