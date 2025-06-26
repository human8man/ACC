#pragma once

#include "Singleton/CSingleton.h"
#include <chrono>


class CTime
	: public CSingleton<CTime>
{
private:
	friend class CSingleton<CTime>;

public:
	CTime();
	~CTime();

	// �X�V����
	void Update();

	static float GetDeltaTime() { return GetInstance()->m_DeltaTime; }
	static float GetNormalDeltaTime() { return GetInstance()->m_NormalDeltaTime; }
	static float GetUnscaledDeltaTime() { return GetInstance()->m_UnscaledDeltaTime; }
	static float GetTimeScale() { return GetInstance()->m_TimeScale; }
	static float GetTotalTime() { return GetInstance()->m_TotalTime; }

	// �X���[���[�V�����{���̐ݒ�
	void SetTimeScale(float scale) { m_TimeScale = scale; }
private:
	float m_DeltaTime;			// 1�t���[���̌o�ߎ��ԁi�b�j
	float m_NormalDeltaTime;	// ���ϓI�ȂP�t���[���o�ߎ���
	float m_UnscaledDeltaTime;	// 1�t���[���̌o�ߎ��ԁi�b.�X�P�[�������j
	float m_TotalTime;			// �N������̗ݐώ��ԁi�b�j

	float m_TimeScale;	// �X���[���[�V�����{���i1.0���ʏ�j

	std::chrono::steady_clock::time_point m_LastTime;
};