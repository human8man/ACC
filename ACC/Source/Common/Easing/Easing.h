#pragma once

#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


namespace MyEasing {
	//==========================================================================
	//	�`�[�g�V�[�g(https://easings.net/ja)
	//==========================================================================


	/**************************************************************************
	* @brief  �ŏ��͂������A�㔼�͑�����������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InSine(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��͑����A���X�Ɍ������I��肪�������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T OutSine(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �n�߂ƏI��肪�������ŁA���Ԃ���������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InOutSine(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �������Ȃ��瓮���A�ŏ��͒x���㔼�ŋ}���ɑ����Ȃ�.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InQuad(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��͑����A�㔼�ŋ}���Ɍ�������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T OutQuad(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��ƍŌ�ł�����肵�A���Ԃŋ}���ɓ���.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InOutQuad(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��͔��ɂ������A�㔼�ŋ}���ɓ���.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InCubic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��ɋ}���ɓ����A�㔼�͂�����茸������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T OutCubic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �n�߂ƏI���͂������ŁA���Ԃ͋}���ɓ���.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InOutCubic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  ���X�ɉ������A�I���ۂɋ}���ɓ���.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InQuart(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��ɋ}���ɓ����A�㔼�͏��X�Ɍ�������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T OutQuart(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��ƍŌオ�������ŁA���Ԃ����ɑ����Ȃ�.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InOutQuart(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��͔��ɂ������A�㔼�ŋ}���ɉ�������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InQuint(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��ɋ}���ɓ����A�㔼�͔��ɂ������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T OutQuint(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �n�߂ƏI���͂������ŁA���Ԃ͋}���ɓ���.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InOutQuint(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��͂قƂ�Ǔ������A�㔼�ŋ}���ɓ����n�߂�.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InExpo(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��ɋ}���ɓ����A�㔼�͋}���Ɍ�������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T OutExpo(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �n�߂ƏI��肪�������ŁA���Ԃ͋}���ɓ���.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InOutExpo(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��͂�����蓮���A�㔼�͋}���ɉ������A�Ō�Ɍ�������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InCirc(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��ɋ}���ɓ����A�㔼�ł�����蓮��.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T OutCirc(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �n�߂͂������A�㔼�͋}���ɉ������A�Ō�ɍĂь�������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InOutCirc(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �n�߂ɓ��������Ȃ��A�㔼�ŋ}���ɓ����n�߂�A�Ăь�������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InBack(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �n�߂͋}���ɓ����A�㔼�œ�������������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T OutBack(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �n�߂ƏI��肪�������ŁA���Ԃ��}���ɉ������A�Ō�ɍĂь�������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InOutBack(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��͏��������A�㔼�Œe�ނ悤�ȓ���.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InElastic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��͋}���ɓ����A�㔼�Œe�ނ悤�ȓ���.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T OutElastic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �n�߂͂������ŁA���ԂŒe�ނ悤�ȓ����ɂȂ�A�Ō�ɍĂт�����茸������.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InOutElastic(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��͂قƂ�Ǔ������A�㔼�Œ��˂�悤�ȓ���.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InBounce(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �ŏ��ɋ}���ɓ����A�㔼�Œ��˂�悤�ȓ���.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T OutBounce(float Time, float MaxTime, T Start, T End);


	/**************************************************************************
	* @brief  �n�߂ƏI��肪�������ŁA���Ԃ����˂�悤�ȓ���.
	* @param  Time		: �o�ߎ���(�t���[��).
	* @param  MaxTime	: �A�j���[�V�����ɂ����鎞��(�t���[��).
	* @param  Start		: �n�܂�̒l.
	* @param  End		: �I���̒l.
	* @return			: �X�V�������W.
	**************************************************************************/
	template<typename T>
	inline T InOutBounce(float Time, float MaxTime, T Start, T End);


}
#include "Easing.inl" 