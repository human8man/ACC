#pragma once

#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


namespace MyEasing {
	//==========================================================================
	//	チートシート(https://easings.net/ja)
	//==========================================================================


	/**************************************************************************
	* @brief  最初はゆっくり、後半は速く加速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InSine(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初は速く、徐々に減速し終わりがゆっくり.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T OutSine(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  始めと終わりがゆっくりで、中間が速い動き.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InOutSine(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  加速しながら動く、最初は遅く後半で急激に速くなる.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InQuad(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初は速く、後半で急激に減速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T OutQuad(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初と最後でゆっくりし、中間で急速に動く.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InOutQuad(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初は非常にゆっくり、後半で急速に動く.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InCubic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初に急速に動き、後半はゆっくり減速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T OutCubic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  始めと終わりはゆっくりで、中間は急速に動く.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InOutCubic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  徐々に加速し、終わり際に急速に動く.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InQuart(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初に急速に動き、後半は徐々に減速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T OutQuart(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初と最後がゆっくりで、中間が非常に速くなる.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InOutQuart(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初は非常にゆっくり、後半で急激に加速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InQuint(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初に急激に動き、後半は非常にゆっくり.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T OutQuint(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  始めと終わりはゆっくりで、中間は急激に動く.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InOutQuint(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初はほとんど動かず、後半で急速に動き始める.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InExpo(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初に急速に動き、後半は急激に減速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T OutExpo(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  始めと終わりがゆっくりで、中間は急激に動く.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InOutExpo(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初はゆっくり動き、後半は急激に加速し、最後に減速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InCirc(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初に急速に動き、後半でゆっくり動く.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T OutCirc(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  始めはゆっくり、後半は急速に加速し、最後に再び減速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InOutCirc(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  始めに動きが少なく、後半で急速に動き始める、再び減速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InBack(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  始めは急速に動き、後半で動きが減速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T OutBack(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  始めと終わりがゆっくりで、中間が急激に加速し、最後に再び減速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InOutBack(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初は少し動き、後半で弾むような動き.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InElastic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初は急速に動き、後半で弾むような動き.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T OutElastic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  始めはゆっくりで、中間で弾むような動きになり、最後に再びゆっくり減速する.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InOutElastic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初はほとんど動かず、後半で跳ねるような動き.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InBounce(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  最初に急速に動き、後半で跳ねるような動き.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T OutBounce(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  始めと終わりがゆっくりで、中間が跳ねるような動き.
	* @param  Time		: 経過時間(フレーム).
	* @param  MaxTime	: アニメーションにかかる時間(フレーム).
	* @param  Start		: 始まりの値.
	* @param  End		: 終わりの値.
	* @return			: 更新した座標.
	**************************************************************************/
	template<typename T>
	inline T InOutBounce(float Time, float MaxTime, T Start, T End);


}
#include "Easing.inl" 