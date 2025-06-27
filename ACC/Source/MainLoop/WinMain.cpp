#include "Main.h"
#include "DirectX/DirectX11.h"
#include <crtdbg.h>	//_ASSERT_EXPR()で必要
#include "MyMacro.h"

#include "FileManager/FileManager.h"

namespace {
	// Sceneのパス
	constexpr char ScenePath[] = "Data\\Scene.json";
}

//=============================================================================
//		メイン関数
//=============================================================================
INT WINAPI WinMain(
	_In_ HINSTANCE hInstance,	// インスタンス番号（ウィンドウの番号）
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	Main* pMain = new Main();	// 初期化＆クラス宣言

	// UIEditorの場合はDEBUGモードに切り替え
	Json m_SceneData = nullptr;
	m_SceneData = FileManager::JsonLoad(ScenePath);
	if (m_SceneData["Scene"].get<std::string>() == "UIEditor") { ISDEBUG = true; }

#if _DEBUG
	ISDEBUG = true;
#endif

	if (pMain != nullptr)
	{
		// ウィンドウ作成成功したら
		if( SUCCEEDED(
			pMain->InitWindow(
				hInstance,
				0, 0,
				WND_W, WND_H)))
		{
			// Dx11用の初期化
			if( SUCCEEDED( pMain->Create() ))
			{
				// メッセージループ
				pMain->Loop();
			}
		}
		// 終了
		pMain->Release();	// Direct3Dの解放

		SAFE_DELETE( pMain );	// クラスの破棄
	}

	return 0;
}