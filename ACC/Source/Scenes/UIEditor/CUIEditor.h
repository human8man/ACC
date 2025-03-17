#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"


//=============================================================================
//		WinUIクラス.
//=============================================================================
class CUIEditor
	: public CSceneBase
{
public:
	CUIEditor(HWND hWnd);
	~CUIEditor();

	void Create()	override;	// 作成処理.
	HRESULT LoadData()override;	// データ読込.
	void Init()		override;	// 初期化処理.
	void Update()	override;	// 更新処理.
	void Draw()		override;	// 描画処理.
	void Release()	override;	// 解放処理.


private:
	// 現在シーンのUIを保存.


private:

};