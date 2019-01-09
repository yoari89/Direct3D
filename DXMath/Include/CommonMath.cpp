#include "PCHMath.h"

float RadianToDegree(float _rad)
{
	// Radian(호도법) -> Degree(각도) 변환
	// PI(3.14...) 값이 들어왔을 때 180이 반환되어야 한다.
	return _rad / PI * 180.f;
}

float DegreeToRadian(float _deg)
{
	// Degree(각도) -> Radian(호도법) 변환
	// 180도가 들어왔을 때 PI가 반환되어야 한다.
	return _deg / 180.f * PI;
}