#include "Main.h"
#include "DirectX/DirectX11.h"
#include <crtdbg.h>	//_ASSERT_EXPR()�ŕK�v
#include "MyMacro.h"

#include "FileManager/FileManager.h"

namespace {
	// Scene�̃p�X
	constexpr char ScenePath[] = "Data\\Scene.json";
}

//=============================================================================
//		���C���֐�
//=============================================================================
INT WINAPI WinMain(
	_In_ HINSTANCE hInstance,	// �C���X�^���X�ԍ��i�E�B���h�E�̔ԍ��j
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	Main* pMain = new Main();	// ���������N���X�錾

	// UIEditor�̏ꍇ��DEBUG���[�h�ɐ؂�ւ�
	Json m_SceneData = nullptr;
	m_SceneData = FileManager::JsonLoad(ScenePath);
	if (m_SceneData["Scene"].get<std::string>() == "UIEditor") { ISDEBUG = true; }

#if _DEBUG
	ISDEBUG = true;
#endif

	if (pMain != nullptr)
	{
		// �E�B���h�E�쐬����������
		if( SUCCEEDED(
			pMain->InitWindow(
				hInstance,
				0, 0,
				WND_W, WND_H)))
		{
			// Dx11�p�̏�����
			if( SUCCEEDED( pMain->Create() ))
			{
				// ���b�Z�[�W���[�v
				pMain->Loop();
			}
		}
		// �I��
		pMain->Release();	// Direct3D�̉��

		SAFE_DELETE( pMain );	// �N���X�̔j��
	}

	return 0;
}