#pragma once

#include "Singleton/CSingleton.h"

class CImageManager
	: public CSingleton<CImageManager>
{
public:
	friend class CSingleton<CImageManager>;// �V���O���g���N���X���t�����h�錾.

	CImageManager();
	~CImageManager();

	// 
	//
	//

	void Init();
	void Release();
	void Create();
	void Update();


private:

};