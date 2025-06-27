#pragma once

#include <iostream>
#include <utility>


//============================================================================
//		CRTPを使ったシングルトンクラス
//============================================================================
template<typename T>
class Singleton
{
public:
	static T* GetInstance() // インスタンス取得
	{
		static T s_Instance;
		return &s_Instance;
	}

protected:
	Singleton() = default;
	~Singleton() = default;

protected:
	// コピー禁止
	Singleton(const Singleton& rhs) = delete;
	Singleton& operator = (const Singleton& rhs) = delete;

};