#pragma once

#include "Singleton/CSingleton.h"
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

	/****************************************************************
	// @brief 指定した範囲 [min, max] のランダムな整数を生成する。
	// @param min	: 最小値
	// @param max	: 最大値
	// @return min 以上 max 以下のランダムな整数
	****************************************************************/
	int GetRandomInt(int min, int max);

	/****************************************************************
	// @brief 指定した範囲 [min, max] のランダムな浮動小数点数を生成する。
	// @param min	: 最小値
	// @param max	: 最大値
	// @return min 以上 max 以下のランダムな浮動小数点数
	****************************************************************/
	float GetRandomFloat(float min, float max);

private:
	std::mt19937 m_RandomEngine;
};