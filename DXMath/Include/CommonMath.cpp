#include "PCHMath.h"

float RadianToDegree(float _rad)
{
	// Radian(ȣ����) -> Degree(����) ��ȯ
	// PI(3.14...) ���� ������ �� 180�� ��ȯ�Ǿ�� �Ѵ�.
	return _rad / PI * 180.f;
}

float DegreeToRadian(float _deg)
{
	// Degree(����) -> Radian(ȣ����) ��ȯ
	// 180���� ������ �� PI�� ��ȯ�Ǿ�� �Ѵ�.
	return _deg / 180.f * PI;
}