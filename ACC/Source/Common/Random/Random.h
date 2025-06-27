#pragma once

#include "Singleton/Singleton.h"
#include <random>


class Random
	: public Singleton<Random>
{
private:
	friend class Singleton<Random>; // シングルトンクラスをフレンド宣言

public:
	Random();
	~Random();

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

	// 50%の確率で true または false を返す
	bool GetRandomBool();

private:
	std::mt19937 m_RandomEngine;
};