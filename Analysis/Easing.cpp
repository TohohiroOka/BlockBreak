#include"Easing.h"
#include<cmath>

double EASE::InSine(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * (1 - cos((time * 3.14159265359) / 2)) + base;
}

double EASE::OutSine(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * sin((time * 3.14159265359) / 2) + base;
}

double EASE::InOutSine(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * (-(cos(3.14159265359 * time) - 1) / 2) + base;
}


double EASE::InQuad(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * time * time + base;
}

double EASE::OutQuad(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * (1 - (1 - time) * (1 - time)) + base;

}

double EASE::InOutQuad(const double change, const double base, const double duration, double time)
{
	time /= duration;
	if (time < 0.5)
	{
		return change * 2 * time * time + base;
	}
	return change * (1 - pow(-2 * time + 2, 2) / 2) + base;
}

double EASE::InCubic(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * time * time * time + base;
}

double EASE::OutCubic(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * (1 - pow(1 - time, 3)) + base;
}

double EASE::InOutCubic(const double change, const double base, const double duration, double time)
{
	time /= duration;
	if (time <= 0.5)
	{
		return change * 4 * time * time * time + base;
	}
	return change * (1 - pow(-2 * time + 2, 3) / 2) + base;
}

double EASE::InQuart(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * time * time * time * time + base;
}

double EASE::OutQuart(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * (1 - pow(1 - time, 4)) + base;
}

double EASE::InOutQuart(const double change, const double base, const double duration, double time)
{
	time /= duration;
	if (time <= 0.5)
	{
		return change * 8 * time * time * time * time + base;
	}
	return change * (1 - pow(-2 * time + 2, 4) / 2) + base;
}

double EASE::InQuint(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * time * time * time * time * time + base;
}

double EASE::OutQuint(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * (1 - pow(1 - time, 5)) + base;
}

double EASE::InOutQuint(const double change, const double base, const double duration, double time)
{
	time /= duration;
	if (time <= 0.5)
	{
		return change * 16 * time * time * time * time * time + base;
	}
	return change * (1 - pow(-2 * time + 2, 5) / 2) + base;
}

double EASE::InExpo(const double change, const double base, const double duration, double time)
{
	time /= duration;
	if (time == 0)
	{
		return change * 0 + base;
	}
	return change * pow(2, 10 * time - 10) + base;
}

double EASE::OutExpo(const double change, const double base, const double duration, double time)
{
	time /= duration;
	if (time == 1)
	{
		return change * 1 + base;
	}
	return change * (1 - pow(2, -10 * time)) + base;
}

double EASE::InBack(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * (2.70158 * time * time * time - (2.70158 - 1) * time * time) + base;

}

double EASE::OutBack(const double change, const double base, const double duration, double time)
{
	time /= duration;
	return change * (1 + 2.70158 * pow(time - 1, 3) + (2.70158 - 1) * pow(time - 1, 2)) + base;
}

double EASE::InOutBack(const double change, const double base, const double duration, double time)
{
	time /= duration;
	if (time < 0.5)
	{
		return change * ((pow(2 * time, 2) * ((1.70158 * 1.525 + 1) * 2 * time - 1.70158 * 1.525)) / 2) + base;
	}
	return change * ((pow(2 * time - 2, 2) * ((1.70158 * 1.525 + 1) * (time * 2 - 2) + 1.70158 * 1.525) + 2) / 2) + base;
}

double EASE::lerp(const double &start, const double &end, const float time)
{
	return start * (1.0f - time) + end * time;
}

double EASE::easeIn(const double &start, const double &end, const float time) {
	float t = time * time;
	return start * (1.0f - t) + end * t;
}

double EASE::easeOut(const double &start, const double &end, const float time)
{
	float y = time * (2 - time);
	return start * (1.0f - y) + end * y;
}

double EASE::easeInOut(const double &start, const double &end, const float time)
{
	float y = time * time * (3 - 2 * time);
	return start * (1.0f - y) + end * y;
}

