#pragma once

#include "Singleton/Singleton.h"
#include <chrono>


class Time
	: public Singleton<Time>
{
private:
	friend class Singleton<Time>;

public:
	Time();
	~Time();

	// 更新処理
	void Update();

	static float GetDeltaTime() { return GetInstance()->m_DeltaTime; }
	static float GetNormalDeltaTime() { return GetInstance()->m_NormalDeltaTime; }
	static float GetUnscaledDeltaTime() { return GetInstance()->m_UnscaledDeltaTime; }
	static float GetTimeScale() { return GetInstance()->m_TimeScale; }
	static float GetTotalTime() { return GetInstance()->m_TotalTime; }

	// スローモーション倍率の設定
	void SetTimeScale(float scale) { m_TimeScale = scale; }
private:
	float m_DeltaTime;			// 1フレームの経過時間（秒）
	float m_NormalDeltaTime;	// 平均的な１フレーム経過時間
	float m_UnscaledDeltaTime;	// 1フレームの経過時間（秒.スケール無し）
	float m_TotalTime;			// 起動からの累積時間（秒）

	float m_TimeScale;	// スローモーション倍率（1.0が通常）

	std::chrono::steady_clock::time_point m_LastTime;
};