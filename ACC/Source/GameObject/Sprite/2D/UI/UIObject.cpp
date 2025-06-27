#include "UIObject.h"
#include "DirectX/DirectX11.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "FileManager/FileManager.h"


UIObject::UIObject()
	: m_pSprite			( nullptr )
	, m_PatternNo		()
{
}
UIObject::~UIObject()
{
	DetachSprite();
}


//=============================================================================
//		更新処理
//=============================================================================
void UIObject::Update()
{
	if( m_pSprite == nullptr ){ return; }
}


//=============================================================================
//		描画処理
//=============================================================================
void UIObject::Draw()
{
	if( m_pSprite == nullptr ){ return; }

	// 描画直前で座標や回転情報などを更新
	m_pSprite->SetPos( m_vPosition );
	m_pSprite->SetPivot( m_vPivot );
	m_pSprite->SetRot( m_vRotation );
	m_pSprite->SetScale( m_vScale );
	m_pSprite->SetColor( m_vColor );
	m_pSprite->SetAlpha( m_Alpha );

	// パターン番号を設定
	m_pSprite->SetPatternNo( m_PatternNo.x, m_PatternNo.y );


	DirectX11::GetInstance()->SetDepth(false);
	// レンダリング
	m_pSprite->Render();
	DirectX11::GetInstance()->SetDepth(true);
}


//=============================================================================
//		スプライトを接続する
//=============================================================================
void UIObject::AttachSprite(Sprite2D* pSprite)
{
	m_pSprite	= new Sprite2D(*pSprite);
	m_vPosition	= m_pSprite->GetPos();
	m_vPivot	= m_pSprite->GetPivot();
	m_vRotation	= m_pSprite->GetRot();
	m_vScale	= m_pSprite->GetScale();
	m_vColor	= m_pSprite->GetColor();
	m_Alpha		= m_pSprite->GetAlpha();
}


//=============================================================================
//		スプライトを切り離す
//=============================================================================
void UIObject::DetachSprite()
{
	m_pSprite = nullptr;
}


//=============================================================================
//		点と四角のあたり判定
//=============================================================================
bool UIObject::PointInSquare( POINT ppos, D3DXVECTOR2 windowpos )
{
	D3DXVECTOR3 pos = {
		m_vPosition.x * FULLSCREENSCALEX + windowpos.x,
		m_vPosition.y * FULLSCREENSCALEY + windowpos.y,
		0.f};

 	if (pos.x < ppos.x
	&&  pos.y < ppos.y
	&& ppos.x < pos.x + m_pSprite->GetSpriteData().Disp.w * m_vScale.x * FULLSCREENSCALEX 
	&& ppos.y < pos.y + m_pSprite->GetSpriteData().Disp.h * m_vScale.y * FULLSCREENSCALEY )
	{
		return true;
	}
	return false;
}


//=============================================================================
//		描画処理
//=============================================================================
void UIObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	Draw();
}


//=============================================================================
//		画像名リストのデータ読込
//=============================================================================
void UIObject::LoadFromJson(
	const std::string& scenepath,
	std::vector<UIObject*>& uis)
{
	// JSON読み込み
	Json jsonData = FileManager::JsonLoad(scenepath);

	// 保存されたUIデータを読み込み、展開
	for (auto& [imageName, spriteArray] : jsonData.items()) {
		// 拡張子が .json ならスキップ
		std::string::size_type dotPos = imageName.find_last_of('.');
		if (dotPos != std::string::npos) {
			std::string ext = imageName.substr(dotPos);
			if (ext == ".json" || ext == ".JSON") continue;
		}

		// スプライト取得
		Sprite2D* pSprite = CSpriteManager::GetInstance()->Clone2DSprite(GetBaseName(imageName));
		if (!pSprite) {
			MessageBoxA(NULL, ("スプライトが見つかりません: " + imageName).c_str(), "Error", MB_OK);
			continue;
		}

		// 各UIインスタンスを展開
		for (auto& value : spriteArray) {
			auto ui = new UIObject();

			// 各種情報を設定
			pSprite->SetPos(D3DXVECTOR3(value["Pos"]["x"], value["Pos"]["y"], value["Pos"]["z"]));
			pSprite->SetColor(D3DXVECTOR3(value["Color"]["x"], value["Color"]["y"], value["Color"]["z"]));
			pSprite->SetAlpha(value["Alpha"]);
			pSprite->SetScale(D3DXVECTOR3(value["Scale"]["x"], value["Scale"]["y"], value["Scale"]["z"]));
			pSprite->SetPivot(D3DXVECTOR3(value["Pivot"]["x"], value["Pivot"]["y"], value["Pivot"]["z"]));
			pSprite->SetRot(D3DXVECTOR3(value["Rotate"]["x"], value["Rotate"]["y"], value["Rotate"]["z"]));

			// SpriteDataの一部も上書き
			pSprite->SetBase(D3DXVECTOR2(value["Base"]["w"], value["Base"]["h"]));
			pSprite->SetDisp(D3DXVECTOR2(value["Disp"]["w"], value["Disp"]["h"]));
			pSprite->SetStride(D3DXVECTOR2(value["Stride"]["w"], value["Stride"]["h"]));

			// リストに追加
			ui->AttachSprite(pSprite);
			uis.push_back(ui);
		}
	}
	// Z座標を基準にソートする
	std::sort(m_pUIs.begin(), m_pUIs.end(), [](const UIObject* a, const UIObject* b) {
		return a->GetPos().z < b->GetPos().z;
		});
}


//=============================================================================
//		パターン番号を設定
//=============================================================================
void UIObject::SetPatternNo( SHORT x, SHORT y )
{
	m_PatternNo = POINTS(x,y);
}


//=============================================================================
//		画像の元サイズを設定
//=============================================================================
void UIObject::SetBase( D3DXVECTOR2& base )
{
	m_pSprite->SetBase(base);
}


//=============================================================================
//		画像の表示範囲を設定
//=============================================================================
void UIObject::SetDisp( D3DXVECTOR2& disp )
{
	m_pSprite->SetDisp(disp);
}


//=============================================================================
//		画像の分割サイズを設定
//=============================================================================
void UIObject::SetStride( D3DXVECTOR2& stride )
{
	m_pSprite->SetStride(stride);
}


//=============================================================================
//		画像名を設定
//=============================================================================
void UIObject::SetSpriteName( const std::string& name )
{
	m_pSprite->SetSpriteName(name);
}


//=============================================================================
//		パターン番号を取得
//=============================================================================
POINTS UIObject::GetPatternNo() const
{
	return m_PatternNo;
}


//=============================================================================
//		スプライト情報の取得
//=============================================================================
Sprite2D::SPRITE_STATE UIObject::GetSpriteData() const
{  
	return m_pSprite->GetSpriteData();
}
