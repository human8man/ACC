#include "CRandom.h"


//=============================================================================
//		�����_���N���X.
//=============================================================================
CRandom::CRandom()
	: m_RandomEngine (std::random_device{}())
{
}

CRandom::~CRandom()
{
}


//=============================================================================
//		�w�肵���͈͂̃����_���Ȑ����𐶐�����.
//=============================================================================
int CRandom::GetRandomInt(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(m_RandomEngine);
}


//=============================================================================
//		�w�肵���͈͂̃����_���ȕ��������_���𐶐�����.
//=============================================================================
float CRandom::GetRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(m_RandomEngine);
}