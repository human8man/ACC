#pragma once

#include "Singleton/CSingleton.h"

class CImageManager
	: public CSingleton<CImageManager>
{
public:
	friend class CSingleton<CImageManager>;// シングルトンクラスをフレンド宣言.

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