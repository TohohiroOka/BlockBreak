#pragma once
class EASE {
public:
	static double InSine(const double change, const double base, const double duration, double time);

	static double OutSine(const double change, const double base, const double duration, double time);

	static double InOutSine(const double change, const double base, const double duration, double time);

	static double InQuad(const double change, const double base, const double duration, double time);

	static double OutQuad(const double change, const double base, const double duration, double time);

	static double InOutQuad(const double change, const double base, const double duration, double time);

	static double InCubic(const double change, const double base, const double duration, double time);

	static double OutCubic(const double change, const double base, const double duration, double time);

	static double InOutCubic(const double change, const double base, const double duration, double time);

	static double InQuart(const double change, const double base, const double duration, double time);

	static double OutQuart(const double change, const double base, const double duration, double time);

	static double InOutQuart(const double change, const double base, const double duration, double time);

	static double InQuint(const double change, const double base, const double duration, double time);

	static double OutQuint(const double change, const double base, const double duration, double time);

	static double InOutQuint(const double change, const double base, const double duration, double time);

	static double InExpo(const double change, const double base, const double duration, double time);

	static double OutExpo(const double change, const double base, const double duration, double time);

	static double InBack(const double change, const double base, const double duration, double time);

	static double OutBack(const double change, const double base, const double duration, double time);

	static double InOutBack(const double change, const double base, const double duration, double time);

	static double lerp(const double &start, const double &end, const float time);

	static double easeIn(const double &start, const double &end, const float time);

	static double easeOut(const double &start, const double &end, const float time);

	static double easeInOut(const double &start, const double &end, const float time);
};