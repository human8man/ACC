#pragma once

#include "Common/Singleton/CSingleton.h"
#include <random>


//=============================================================================
//		ランダムクラス.
//=============================================================================
class CRandom
	: public CSingleton<CRandom>
{
private:
	friend class CSingleton<CRandom>; // シングルトンクラスをフレンド宣言.

public:
	CRandom();
	~CRandom();
	
	// 指定した範囲のランダムな整数を生成する.
	int GetRandomInt(int min, int max);

	// 指定した範囲のランダムな浮動小数点数を生成する.
	float GetRandomFloat(float min, float max);

private:
	std::mt19937 m_RandomEngine;
};