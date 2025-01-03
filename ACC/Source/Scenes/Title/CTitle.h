#pragma once

#include "Scenes/CSceneBase.h"
#include "..//GameObject/Sprite/2D/CSprite2D.h"
#include "DirectInput/CDirectInput.h"

//前方宣言
class CUIObject;

/********************************************************************************
*	タイトルシーンクラス.
**/
class CTitle
	: public CSceneBase
{
public:
	// Titleで使用するスプライト名を追加していく(名前順).
	enum TitleSprite {
		FullScreen,
		BestTimeText,
		Button3,
		ButtonText2,
		Button,
		ButtonText3,
		ButtonText,
		Button2,
		TitleText,
	};

public:
	CTitle(HWND hWnd);
	~CTitle();

	void Create() override;
	HRESULT LoadData() override;
	void Release() override;
	void Init() override;

	void Update() override;
	void Draw() override;

private:
	// 見づらいのでボタンごとに処理をまとめる.
	void StartButtonUpdate(CKey* key, CGamePad* pad);
	void OptionButtonUpdate(CKey* key, CGamePad* pad);
	void EndButtonUpdate(CKey* key, CGamePad* pad);

	// Win側のキー入力空打ち(テスト用なので隙があれば変更すべき).
	void EmptyInput();

private:
	// 画像情報リスト.
	std::vector<std::string> m_SpriteDataList;//スプライト情報をまとめる配列.

	std::vector<D3DXVECTOR3> m_SpritePosList;	//スプライト座標をまとめる配列.

	std::vector<CUIObject*> m_pUIs;			// UIクラス.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2Dクラス.

	float m_ScaleDef;		//スケールの初期値.
	float m_StartScale;		//スタートボタンのスケール値.
	float m_EndScale;		//エンドボタンのスケール値.
	float m_OptionScale;	//オプションボタンのスケール値.
	float m_OpTextDefScale; //オプションテキストの初期スケール値.

	int  m_SelectButton; //コントローラーもしくはキーボードでボタンを選択するときに使う.
	bool m_SelectOther;	 //ほかの選択肢で遷移処理をしたとき用.
};