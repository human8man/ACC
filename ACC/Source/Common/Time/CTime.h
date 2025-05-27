#pragma once

#include "Singleton/CSingleton.h"
#include <chrono>


//=============================================================================
//		�^�C���N���X.
//=============================================================================
class CTime
	: public CSingleton<CTime>
{
private:
	friend class CSingleton<CTime>; // �V���O���g���N���X���t�����h�錾.

public:
	CTime();
	~CTime();

	// �X�V����.
	void Update();

	// �f���^�^�C���i�O�t���[���Ƃ̍��j���擾.
	static float GetDeltaTime() { return GetInstance()->m_DeltaTime; }

	// �ݐώ��ԁi�N������̌o�߁j���擾.
	static float GetTotalTime() { return GetInstance()->m_TotalTime; }

	// �X���[���[�V�����{���̐ݒ�.
	void SetTimeScale(float scale) { m_TimeScale = scale; }
	float GetTimeScale() const { return m_TimeScale; }
private:
	float m_DeltaTime;	// 1�t���[���̌o�ߎ��ԁi�b�j.
	float m_TotalTime;	// �N������̗ݐώ��ԁi�b�j.

	float m_TimeScale;	// �X���[���[�V�����{���i1.0���ʏ�j.

	std::chrono::steady_clock::time_point m_LastTime;
};