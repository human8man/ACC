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

	// デルタタイム（前フレームとの差）を取得.
	static float GetDeltaTime() { return GetInstance()->m_DeltaTime; }

	// 累積時間（起動からの経過）を取得.
	static float GetTotalTime() { return GetInstance()->m_TotalTime; }

	// スローモーション倍率の設定.
	void SetTimeScale(float scale) { m_TimeScale = scale; }
	float GetTimeScale() const { return m_TimeScale; }
private:
	float m_DeltaTime;	// 1フレームの経過時間（秒）.
	float m_TotalTime;	// 起動からの累積時間（秒）.

	float m_TimeScale;	// スローモーション倍率（1.0が通常）.

	std::chrono::steady_clock::time_point m_LastTime;
};