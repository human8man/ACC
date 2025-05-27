#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/CSprite2D.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"

// 前方宣言.
class CUIObject;
class CStaticMesh;
class CTitleUI;

//=============================================================================
//		タイトルシーンクラス.
//=============================================================================
class CTitle
	: public CSceneBase
{
public:
	CTitle(HWND hWnd);
	~CTitle();

	void Create() override;			// 作成処理.
	HRESULT LoadData() override;	// 読込処理.
	void Init() override;			// 初期化処理.
	void Update() override;			// 更新処理.
	void Draw() override;			// 描画処理.
	void Release() override;		// 解放処理.

private:
	HRESULT ShaderTestFunction();

	void CameraDrawShader(D3DXMATRIX& mView, D3DXMATRIX& mProj );
	void UIDrawShader();
	void EditLineImGui();

private:
	LIGHT		m_Light;
	D3DXMATRIX	m_mView;
	D3DXMATRIX	m_mProj;

	ID3D11VertexShader* m_pVertexShader;	// 頂点シェーダ.
	ID3D11InputLayout* m_pVertexLayout;		// 頂点レイアウト.
	ID3D11PixelShader* m_pPixelShader;		// ピクセルシェーダ.
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pCBufferPerFrame;	//フレーム毎のバッファ.
	ID3D11Buffer* m_pCBufferMatrix;		//フレーム毎のバッファ.

	std::unique_ptr<CTitleUI>	m_pTitleUI;	// UIクラス.
	CStaticMesh* m_pEgg;	// 卵.

	bool m_Start;	// ゲーム開始フラグ.
};