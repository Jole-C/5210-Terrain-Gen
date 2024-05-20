#include "MathHelpers.h"

#include <cstdlib>


double Mid(double _min, double _val, double _max)
{

	if (_val < _min) return _min;
	if (_val > _max) return _max;

	return _val;

}

//Lerps from the first value to the second value, with a percentage of perc - use like this: x = Lerp(x, targetValue, valueBetween0And1);
//Source: https://github.com/NangiDev/pico-8-projects/blob/master/tech/lerp.p8
double Lerp(double tar, double pos, double perc)
{
	return (1 - perc) * tar + perc * pos;
}

//Generates an integer within a range
//Source: https://stackoverflow.com/questions/5008804/generating-a-random-integer-from-a-range
int RandomRangeInteger(int min, int max)
{
	return min + (rand() % static_cast<int>(max - min + 1));
}

double RandomRangeDouble(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}