#include "CRandom.h"


//=============================================================================
//		ランダムクラス.
//=============================================================================
CRandom::CRandom()
	: m_RandomEngine (std::random_device{}())
{
}

CRandom::~CRandom()
{
}


//=============================================================================
//		指定した範囲のランダムな整数を生成する.
//=============================================================================
int CRandom::GetRandomInt(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(m_RandomEngine);
}


//=============================================================================
//		指定した範囲のランダムな浮動小数点数を生成する.
//=============================================================================
float CRandom::GetRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(m_RandomEngine);
}