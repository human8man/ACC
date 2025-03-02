#pragma once

#include <iostream>
#include <utility>


//============================================================================
//		CRTPを使ったシングルトンクラス.
//============================================================================
template<typename T>
class CSingleton
{
public:
	static T* GetInstance() // インスタンス取得.
	{
		static T s_Instance;
		return &s_Instance;
	}

protected:
	CSingleton() = default;
	~CSingleton() = default;

protected:
	// コピー禁止.
	CSingleton(const CSingleton& rhs) = delete;
	CSingleton& operator = (const CSingleton& rhs) = delete;

};