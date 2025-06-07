#pragma once

namespace MyEasing {

	template<typename T>
	inline T InSine(float Time, float MaxTime, T Start, T End) {
		float t = Time / MaxTime;
		return Start + (End - Start) * (1 - std::cos(t * (M_PI / 2)));
	}

	template<typename T>
	T OutSine(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		return Start + (End - Start) * std::sin(t * (M_PI / 2));
	}

	template<typename T>
	T InOutSine(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		return Start + (End - Start) * (-(std::cos(M_PI * t) - 1) / 2);
	}

	template<typename T>
	T InQuad(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		return Start + (End - Start) * t * t;
	}

	template<typename T>
	T OutQuad(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		return Start + (End - Start) * t * (2 - t);
	}

	template<typename T>
	T InOutQuad(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		if (t < 0.5f) return Start + (End - Start) * 2 * t * t;
		return Start + (End - Start) * (-1 + (4 - 2 * t) * t);
	}

	template<typename T>
	T InCubic(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		return Start + (End - Start) * t * t * t;
	}

	template<typename T>
	T OutCubic(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		t--;
		return Start + (End - Start) * (t * t * t + 1);
	}

	template<typename T>
	T InOutCubic(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		if (t < 0.5f) return Start + (End - Start) * 4 * t * t * t;
		t -= 1;
		return Start + (End - Start) * (t * t * t * 4 + 1);
	}

	template<typename T>
	T InQuart(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		return Start + (End - Start) * t * t * t * t;
	}

	template<typename T>
	T OutQuart(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		t--;
		return Start + (End - Start) * (1 - t * t * t * t);
	}

	template<typename T>
	T InOutQuart(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		if (t < 0.5f) return Start + (End - Start) * 8 * t * t * t * t;
		t -= 1;
		return Start + (End - Start) * (1 - t * t * t * t);
	}

	template<typename T>
	T InQuint(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		return Start + (End - Start) * t * t * t * t * t;
	}

	template<typename T>
	T OutQuint(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		t--;
		return Start + (End - Start) * (t * t * t * t * t + 1);
	}

	template<typename T>
	T InOutQuint(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		if (t < 0.5f) return Start + (End - Start) * 16 * t * t * t * t * t;
		t -= 1;
		return Start + (End - Start) * (t * t * t * t * t * 16 + 1);
	}

	template<typename T>
	T InExpo(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		return Start + (End - Start) * (t == 0 ? 0 : std::pow(2, 10 * (t - 1)));
	}

	template<typename T>
	T OutExpo(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		return Start + (End - Start) * (t == 1 ? 1 : 1 - std::pow(2, -10 * t));
	}

	template<typename T>
	T InOutExpo(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		if (t == 0) return Start;
		if (t == 1) return End;
		if (t < 0.5f) return Start + (End - Start) / 2 * std::pow(2, 10 * (2 * t - 1));
		return Start + (End - Start) / 2 * (2 - std::pow(2, -10 * (2 * t - 1)));
	}

	template<typename T>
	T InCirc(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		return Start + (End - Start) * (1 - std::sqrt(1 - t * t));
	}

	template<typename T>
	T OutCirc(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		t--;
		return Start + (End - Start) * std::sqrt(1 - t * t);
	}

	template<typename T>
	T InOutCirc(float Time, float MaxTime, T Start, T End)
	{
		float t = std::clamp(Time / MaxTime, 0.0f, 1.0f);
		if (t < 0.5f) return Start + (End - Start) / 2 * (1 - std::sqrt(1 - 4 * t * t));
		t = t * 2 - 1;
		return Start + (End - Start) / 2 * (std::sqrt(1 - t * t) + 1);
	}

	template<typename T>
	T InBack(float Time, float MaxTime, T Start, T End)
	{
		const float s = 1.70158f;
		float t = Time / MaxTime;
		return Start + (End - Start) * t * t * ((s + 1) * t - s);
	}

	template<typename T>
	T OutBack(float Time, float MaxTime, T Start, T End)
	{
		const float s = 1.70158f;
		float t = Time / MaxTime;
		t--;
		return Start + (End - Start) * (t * t * ((s + 1) * t + s) + 1);
	}

	template<typename T>
	T InOutBack(float Time, float MaxTime, T Start, T End)
	{
		const float s = 1.70158f * 1.525f;
		float t = Time / MaxTime;
		if (t < 0.5f) return Start + (End - Start) / 2 * (t * t * ((s + 1) * 2 * t - s));
		t = 2 * t - 1;
		return Start + (End - Start) / 2 * (t * t * ((s + 1) * t + s) + 1);
	}

	template<typename T>
	T InElastic(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		if (t == 0) return Start;
		if (t == 1) return End;
		return Start - (End - Start) * std::pow(2, 10 * (t - 1)) * std::sin((t - 1.1f) * 5 * M_PI);
	}

	template<typename T>
	T OutElastic(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		if (t == 0) return Start;
		if (t == 1) return End;
		return Start + (End - Start) * std::pow(2, -10 * t) * std::sin((t - 0.1f) * 5 * M_PI) + (End - Start);
	}

	template<typename T>
	T InOutElastic(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		if (t == 0) return Start;
		if (t == 1) return End;
		if (t < 0.5f) {
			t *= 2;
			return Start - (End - Start) / 2 * std::pow(2, 10 * (t - 1)) * std::sin((t - 1.1f) * 5 * M_PI);
		}
		t = t * 2 - 1;
		return Start + (End - Start) / 2 * std::pow(2, -10 * t) * std::sin((t - 0.1f) * 5 * M_PI);
	}

	template<typename T>
	T InBounce(float Time, float MaxTime, T Start, T End)
	{
		return End - OutBounce(MaxTime - Time, MaxTime, T(0), End - Start) + Start;
	}

	template<typename T>
	T OutBounce(float Time, float MaxTime, T Start, T End)
	{
		float t = Time / MaxTime;
		if (t < 1 / 2.75f) {
			return Start + (End - Start) * (7.5625f * t * t);
		}
		else if (t < 2 / 2.75f) {
			t -= 1.5f / 2.75f;
			return Start + (End - Start) * (7.5625f * t * t + 0.75f);
		}
		else if (t < 2.5f / 2.75f) {
			t -= 2.25f / 2.75f;
			return Start + (End - Start) * (7.5625f * t * t + 0.9375f);
		}
		else {
			t -= 2.625f / 2.75f;
			return Start + (End - Start) * (7.5625f * t * t + 0.984375f);
		}
	}

	template<typename T>
	T InOutBounce(float Time, float MaxTime, T Start, T End)
	{
		if (Time < MaxTime / 2) return InBounce(Time * 2, MaxTime, Start, End / 2);
		return OutBounce(Time * 2 - MaxTime, MaxTime, Start + End / 2, End / 2);
	}

}
