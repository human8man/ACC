#pragma once

#include "Singleton/CSingleton.h"
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

	/****************************************************************
	// @brief �w�肵���͈� [min, max] �̃����_���Ȑ����𐶐�����B
	// @param min	: �ŏ��l
	// @param max	: �ő�l
	// @return min �ȏ� max �ȉ��̃����_���Ȑ���
	****************************************************************/
	int GetRandomInt(int min, int max);

	/****************************************************************
	// @brief �w�肵���͈� [min, max] �̃����_���ȕ��������_���𐶐�����B
	// @param min	: �ŏ��l
	// @param max	: �ő�l
	// @return min �ȏ� max �ȉ��̃����_���ȕ��������_��
	****************************************************************/
	float GetRandomFloat(float min, float max);

private:
	std::mt19937 m_RandomEngine;
};