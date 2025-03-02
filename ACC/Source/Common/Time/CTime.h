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


	// �f���^�^�C�����擾.
	static float& GetDeltaTime() { return GetInstance()->m_DeltaTime; }
private:
	float m_DeltaTime;
	std::chrono::steady_clock::time_point m_LastTime;
};