#include "Random.h"


Random::Random()
	: m_RandomEngine (std::random_device{}())
{
}

Random::~Random()
{
}


//=============================================================================
//		指定した範囲のランダムな整数を生成する
//=============================================================================
int Random::GetRandomInt(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max); // 整数の一様分布を定義
	return dist(m_RandomEngine); // 乱数を生成して返す
}


//=============================================================================
//		指定した範囲のランダムな浮動小数点数を生成する
//=============================================================================
float Random::GetRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max); // 浮動小数の一様分布を定義
	return dist(m_RandomEngine); // 乱数を生成して返す
}


//=============================================================================
//		50%の確率で true または false を返す
//=============================================================================
bool Random::GetRandomBool()
{
	// 0から1までの整数の一様分布を定義 (0と1のどちらか)
	std::uniform_int_distribution<int> dist(0, 1);

	// 乱数を生成. 0が出たら false, 1が出たら true を返す
	return (dist(m_RandomEngine) == 1);
}