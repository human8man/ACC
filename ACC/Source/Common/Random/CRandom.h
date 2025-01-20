#pragma once

#include "Common/Singleton/CSingleton.h"
#include <random>


//=============================================================================
//		�����_���N���X.
//=============================================================================
class CRandom
	: public CSingleton<CRandom>
{
private:
	friend class CSingleton<CRandom>; // �V���O���g���N���X���t�����h�錾.

public:
	CRandom();
	~CRandom();
	
	// �w�肵���͈͂̃����_���Ȑ����𐶐�����.
	int GetRandomInt(int min, int max);

	// �w�肵���͈͂̃����_���ȕ��������_���𐶐�����.
	float GetRandomFloat(float min, float max);

private:
	std::mt19937 m_RandomEngine;
};