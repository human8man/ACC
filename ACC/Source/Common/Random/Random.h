#pragma once

#include "Singleton/Singleton.h"
#include <random>


class Random
	: public Singleton<Random>
{
private:
	friend class Singleton<Random>; // �V���O���g���N���X���t�����h�錾

public:
	Random();
	~Random();

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

	// 50%�̊m���� true �܂��� false ��Ԃ�
	bool GetRandomBool();

private:
	std::mt19937 m_RandomEngine;
};