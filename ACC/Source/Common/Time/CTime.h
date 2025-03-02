#pragma once

#include "Singleton/CSingleton.h"
#include <chrono>


//=============================================================================
//		タイムクラス.
//=============================================================================
class CTime
	: public CSingleton<CTime>
{
private:
	friend class CSingleton<CTime>; // シングルトンクラスをフレンド宣言.

public:
	CTime();
	~CTime();

	// 更新処理.
	void Update();


	// デルタタイムを取得.
	static float& GetDeltaTime() { return GetInstance()->m_DeltaTime; }
private:
	float m_DeltaTime;
	std::chrono::steady_clock::time_point m_LastTime;
};