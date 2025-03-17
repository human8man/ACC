#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"


//=============================================================================
//		WinUI�N���X.
//=============================================================================
class CUIEditor
	: public CSceneBase
{
public:
	CUIEditor(HWND hWnd);
	~CUIEditor();

	void Create()	override;	// �쐬����.
	HRESULT LoadData()override;	// �f�[�^�Ǎ�.
	void Init()		override;	// ����������.
	void Update()	override;	// �X�V����.
	void Draw()		override;	// �`�揈��.
	void Release()	override;	// �������.


private:
	// ���݃V�[����UI��ۑ�.


private:

};