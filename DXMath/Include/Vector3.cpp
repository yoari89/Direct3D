#include "PCHMath.h"
#include "CommonMath.h"

_tagVector3	_tagVector3::Axis[3] = { Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) };
_tagVector3	_tagVector3::Zero = Vector3(0.0f, 0.0f, 0.0f);
_tagVector3	_tagVector3::One = Vector3(1.0f, 1.0f, 1.0f);

// 생성자
_tagVector3::_tagVector3() :
	x(0.0f),
	y(0.0f),
	z(0.0f)
{
}

_tagVector3::_tagVector3(const _tagVector3& _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
}

_tagVector3::_tagVector3(const XMVECTOR& _v)
{
	// SIMD를 사용하기 위해서는 XMVECTOR 타입을 이용해야 한다.
	// XMStore = XMVECTOR 타입을 XMFLOAT* 타입으로 변환하는데 사용
	// XMFLOAT3 = 구조체로 x, y, z 멤버를 가지고 있다. 
	//            this를 XMFLOAT3 형태로 형변환은 문제가 없다. (메모리 크기 일치)
	XMStoreFloat3((XMFLOAT3*)this, _v);
}

_tagVector3::_tagVector3(const XMFLOAT3& _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
}

_tagVector3::_tagVector3(int _i)
{
	x = (float)_i;
	y = (float)_i;
	z = (float)_i;
}

_tagVector3::_tagVector3(int _arrI[3])
{
	x = (float)_arrI[0];
	y = (float)_arrI[1];
	z = (float)_arrI[2];
}

_tagVector3::_tagVector3(int _x, int _y, int _z)
{
	x = (float)_x;
	y = (float)_y;
	z = (float)_z;
}

_tagVector3::_tagVector3(float _f)
{
	x = _f;
	y = _f;
	z = _f;
}

_tagVector3::_tagVector3(float _arrF[3])
{
	x = _arrF[0];
	y = _arrF[1];
	z = _arrF[2];
}

_tagVector3::_tagVector3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

// 연산자 오버로딩
// []
float& _tagVector3::operator[](unsigned int _index)
{
	switch (_index)
	{
	case 0:
	{
		return x;
	}
	case 1:
	{
		return y;
	}
	case 2:
	{
		return z;
	}
	}
	return x;
}

// =
void _tagVector3::operator=(const _tagVector3& _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
}

void _tagVector3::operator=(const XMVECTOR& _v)
{
	XMStoreFloat3((XMFLOAT3*)this, _v);
}

void _tagVector3::operator=(const XMFLOAT3& _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
}

void _tagVector3::operator=(int _i)
{
	x = (float)_i;
	y = (float)_i;
	z = (float)_i;
}

void _tagVector3::operator=(int _arrI[3])
{
	for (int j = 0; j < 3; ++j)
	{
		(*this)[j] = (float)_arrI[j];
	}
}

void _tagVector3::operator=(float _f)
{
	x = _f;
	y = _f;
	z = _f;
}

void _tagVector3::operator=(float _arrF[3])
{
	x = _arrF[0];
	y = _arrF[1];
	z = _arrF[2];
}

// +
_tagVector3 _tagVector3::operator+(const _tagVector3& _v) const
{
	// XMLoad = XMFLOAT*를 XMVECTOR로 변환하는데 사용
	// XMFLOAT3 = 구조체로 x, y, z 멤버를 가지고 있다. 
	//            this를 XMFLOAT3 형태로 형변환은 문제가 없다. (메모리 크기 일치)
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = XMLoadFloat3((XMFLOAT3*)&_v);
	v = v1 + v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator+(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v = v1 + _v;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator+(const XMFLOAT3& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = XMLoadFloat3(&_v);
	v = v1 + v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator+(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_i);
	v1 = XMLoadFloat3((XMFLOAT3*)this);

	// _tagVec.Convert() = _tagVec(_tagVector3 타입)을 XMVECTOR 타입으로 형변환
	v2 = tagVec.Convert();

	// v(XMVECTOR) = XMVECTOR + XMVECTOR
	v = v1 + v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator+(int _arrI[3]) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_arrI);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 + v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator+(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_f);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 + v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator+(float _arrF[3]) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_arrF);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 + v2;

	return _tagVector3(v);
}

// +=
void _tagVector3::operator+=(const _tagVector3& _v)
{
	// v1은 함수 호출자(this)의 포인터 객체가 된다. (값이 변경된다.)
	// operator+= 연산자 오버로딩 함수에 v1은 함수 호출자로 보면된다.
	XMVECTOR v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = _v.Convert();
	v1 += v2;

	// Convert(XMVECTOR 타입) -> XMFLOAT3* 타입으로 변환된다.
	this->Convert(v1);
}

void _tagVector3::operator+=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 += _v;
	this->Convert(v1);
}

void _tagVector3::operator+=(const XMFLOAT3& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&_v);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector3::operator+=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector3::operator+=(int _arrI[3])
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector3::operator+=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector3::operator+=(float _arrF[3])
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

// -
_tagVector3 _tagVector3::operator-(const _tagVector3& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = XMLoadFloat3((XMFLOAT3*)&_v);
	v = v1 - v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator-(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v = v1 - _v;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator-(const XMFLOAT3& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = XMLoadFloat3(&_v);
	v = v1 - v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator-(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_i);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator-(int _arrI[3]) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_arrI);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator-(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_f);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator-(float _arrF[3]) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_arrF);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector3(v);
}

// -=
void _tagVector3::operator-=(const _tagVector3& _v)
{
	// v1은 함수 호출자(this)의 포인터 객체가 된다. (값이 변경된다.)
	// operator+= 연산자 오버로딩 함수에 v1은 함수 호출자로 보면된다.
	XMVECTOR v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = _v.Convert();
	v1 -= v2;

	// Convert(XMVECTOR 타입) -> XMFLOAT3* 타입으로 변환된다.
	this->Convert(v1);
}

void _tagVector3::operator-=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 -= _v;
	this->Convert(v1);
}

void _tagVector3::operator-=(const XMFLOAT3& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&_v);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector3::operator-=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector3::operator-=(int _arrI[3])
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector3::operator-=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector3::operator-=(float _arrF[3])
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

// *
_tagVector3 _tagVector3::operator*(const _tagVector3& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = XMLoadFloat3((XMFLOAT3*)&_v);
	v = v1 * v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator*(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v = v1 * _v;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator*(const XMFLOAT3& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = XMLoadFloat3(&_v);
	v = v1 * v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator*(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_i);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator*(int _arrI[3]) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_arrI);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator*(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_f);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator*(float _arrF[3]) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_arrF);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector3(v);
}

// *=
void _tagVector3::operator*=(const _tagVector3& _v)
{
	// v1은 함수 호출자(this)의 포인터 객체가 된다. (값이 변경된다.)
	// operator+= 연산자 오버로딩 함수에 v1은 함수 호출자로 보면된다.
	XMVECTOR v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = _v.Convert();
	v1 *= v2;

	// Convert(XMVECTOR 타입) -> XMFLOAT3* 타입으로 변환된다.
	this->Convert(v1);
}

void _tagVector3::operator*=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 *= _v;
	this->Convert(v1);
}

void _tagVector3::operator*=(const XMFLOAT3& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&_v);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector3::operator*=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector3::operator*=(int _arrI[3])
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector3::operator*=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector3::operator*=(float _arrF[3])
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

// /
_tagVector3 _tagVector3::operator/(const _tagVector3& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = XMLoadFloat3((XMFLOAT3*)&_v);
	v = v1 / v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator/(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v = v1 / _v;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator/(const XMFLOAT3& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = XMLoadFloat3(&_v);
	v = v1 / v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator/(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_i);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator/(int _arrI[3]) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_arrI);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator/(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_f);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector3(v);
}

_tagVector3 _tagVector3::operator/(float _arrF[3]) const
{
	XMVECTOR v, v1, v2;
	_tagVector3 tagVec(_arrF);
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector3(v);
}

// /=
void _tagVector3::operator/=(const _tagVector3& _v)
{
	// v1은 함수 호출자(this)의 포인터 객체가 된다. (값이 변경된다.)
	// operator+= 연산자 오버로딩 함수에 v1은 함수 호출자로 보면된다.
	XMVECTOR v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = _v.Convert();
	v1 /= v2;

	// Convert(XMVECTOR 타입) -> XMFLOAT3* 타입으로 변환된다.
	this->Convert(v1);
}

void _tagVector3::operator/=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 /= _v;
	this->Convert(v1);
}

void _tagVector3::operator/=(const XMFLOAT3& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&_v);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector3::operator/=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector3::operator/=(int _arrI[3])
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector3::operator/=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector3::operator/=(float _arrF[3])
{
	XMVECTOR v1, v2;
	_tagVector3 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat3((XMFLOAT3*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

// ==
bool _tagVector3::operator==(const _tagVector3& _v) const
{
	return x == _v.x && y == _v.y && z == _v.z;
}

bool _tagVector3::operator==(const XMVECTOR& _v) const
{
	_tagVector3 tagVec(_v);
	return x == tagVec.x && y == tagVec.y && z == tagVec.z;
}

bool _tagVector3::operator==(const XMFLOAT3& _v) const
{
	return x == _v.x && y == _v.y && z == _v.z;
}

bool _tagVector3::operator==(int _i) const
{
	return x == (float)_i && y == (float)_i && z == (float)_i;
}

bool _tagVector3::operator==(int _arrI[3]) const
{
	return x == (float)_arrI[0] && y == (float)_arrI[1] && z == (float)_arrI[2];
}

bool _tagVector3::operator==(float _f) const
{
	return x == _f && y == _f && z == _f;
}

bool _tagVector3::operator==(float _arrF[3]) const
{
	return x == _arrF[0] && y == _arrF[1] && z == _arrF[2];
}

// !=
bool _tagVector3::operator!=(const _tagVector3& _v) const
{
	return x != _v.x || y != _v.y || z != _v.z;
}

bool _tagVector3::operator!=(const XMVECTOR& _v) const
{
	_tagVector3 tagVec(_v);
	return x != tagVec.x || y != tagVec.y || z != tagVec.z;
}

bool _tagVector3::operator!=(const XMFLOAT3& _v) const
{
	return x != _v.x || y != _v.y || z == _v.z;
}

bool _tagVector3::operator!=(int _i) const
{
	return x != (float)_i || y != (float)_i || z != (float)_i;
}

bool _tagVector3::operator!=(int _arrI[3]) const
{
	return x != (float)_arrI[0] || y != (float)_arrI[1] || z != (float)_arrI[2];
}

bool _tagVector3::operator!=(float _f) const
{
	return x != _f || y != _f || z != _f;
}

bool _tagVector3::operator!=(float _arrF[3]) const
{
	return x != _arrF[0] || y != _arrF[1] || z != _arrF[2];
}

// 길이
float _tagVector3::Length() const
{
	XMVECTOR v = this->Convert();

	// XMVector3Length : 3D 벡터의 길이를 계산합니다.
	return XMVectorGetX(XMVector3Length(v));
}

// 거리
float _tagVector3::Distance(const _tagVector3& _v) const
{
	return (*this - _v).Length();
}

// 정규화
void _tagVector3::Normalize()
{
	// 자신의 _tagVector3 값을 XMVECTOR로 변환 후
	// XMVector3Normalize() 함수를 이용하여 정규화 시킨다.
	// this = const point 이기 때문에 값을 변경할 수 없다.
	XMVECTOR v = this->Convert();
	*this = XMVector3Normalize(v);
}

_tagVector3 _tagVector3::Normalize(const _tagVector3& _v)
{
	// 인자로 넘어온 _tagVector3& 레퍼런스 변수를 
	// 지역변수인 XMVECTOR v 변수를 벡터 정규화 후 v 값을 반환
	XMVECTOR v = _v.Convert();
	return _tagVector3(XMVector3Normalize(v));
}

// 변환
XMVECTOR _tagVector3::Convert() const
{
	return XMLoadFloat3((XMFLOAT3*)this);
}

void _tagVector3::Convert(const XMVECTOR& _v)
{
	// XMVECTOR타입 _v 변수를 XMFLOAT3* 타입으로 변환
	// _v가 레퍼런스 타입이므로 _v의 값이 직접 변경된다.
	XMStoreFloat3((XMFLOAT3*)this, _v);
}

// 각도
float _tagVector3::Angle(const _tagVector3& _v) const
{
	_tagVector3 v1 = Normalize(*this);
	_tagVector3 v2 = Normalize(_v);
	return RadianToDegree(acosf(v1.Dot(v2)));
}

float _tagVector3::Angle(const XMVECTOR& _v) const
{
	_tagVector3 v1 = Normalize(*this);
	_tagVector3 v2 = Normalize(_v);
	return RadianToDegree(acosf(v1.Dot(v2)));
}

float _tagVector3::Angle(const XMFLOAT3& _v) const
{
	_tagVector3 v1 = Normalize(*this);
	_tagVector3 v2 = Normalize(_v);
	return RadianToDegree(acosf(v1.Dot(v2)));
}

float _tagVector3::Angle(int _arrI[3]) const
{
	_tagVector3 v1 = Normalize(*this);
	_tagVector3 v2 = Normalize(_arrI);
	return RadianToDegree(acosf(v1.Dot(v2)));
}

float _tagVector3::Angle(float _arrF[3]) const
{
	_tagVector3 v1 = Normalize(*this);
	_tagVector3 v2 = Normalize(_arrF);
	return RadianToDegree(acosf(v1.Dot(v2)));
}

// 내적
float _tagVector3::Dot(const _tagVector3& _v) const
{
	return Dot(_v.Convert());
}

float _tagVector3::Dot(const XMVECTOR& _v) const
{
	// XMVector3Dot : 3D 벡터간의 내적을 계산
	return XMVectorGetX(XMVector3Dot(this->Convert(), _v));
}

float _tagVector3::Dot(const XMFLOAT3& _v) const
{
	return Dot(XMLoadFloat3(&_v));
}

float _tagVector3::Dot(int _arrI[3]) const
{
	_tagVector3 tagVec(_arrI);
	return Dot(tagVec.Convert());
}

float _tagVector3::Dot(float _arrF[3]) const
{
	_tagVector3 tagVec(_arrF);
	return Dot(tagVec.Convert());
}

// 외적
_tagVector3 _tagVector3::Cross(const _tagVector3& _v) const
{
	return Cross(_v.Convert());
}

_tagVector3 _tagVector3::Cross(const XMVECTOR& _v) const
{
	// XMVector3Cross : 3D 벡터 사이의 외적을 계산
	_tagVector3 v = _tagVector3(XMVector3Cross(this->Convert(), _v));
	if (isnan(v.x) || isnan(v.y) || isnan(v.z))
	{
		return _tagVector3();
	}
	return v;
}

_tagVector3 _tagVector3::Cross(const XMFLOAT3& _v) const
{
	return Cross(XMLoadFloat3(&_v));
}

_tagVector3 _tagVector3::Cross(int _arrI[3]) const
{
	_tagVector3 tagVec(_arrI);
	return Cross(tagVec.Convert());
}

_tagVector3 _tagVector3::Cross(float _arrF[3]) const
{
	_tagVector3 tagVec(_arrF);
	return Cross(tagVec.Convert());
}

// 벡터 * 행렬 곱
_tagVector3 _tagVector3::TransformNormal(const _tagMatrix& _m)
{
	// Normal : 방향 벡터
	return _tagVector3(XMVector3TransformNormal(this->Convert(), _m.m));
}

_tagVector3 _tagVector3::TransformNormal(const XMMATRIX& _m)
{
	return _tagVector3(XMVector3TransformNormal(this->Convert(), _m));
}

_tagVector3 _tagVector3::TransformCoord(const _tagMatrix& _m)
{
	// Coord : w값이 1이며, 이동행렬을 위해 사용한다.
	//         w값이 0이면 행렬 계산 결과의 위치를 담당하는 값이 모두 0이 된다.
	return _tagVector3(XMVector3TransformCoord(this->Convert(), _m.m));
}

_tagVector3 _tagVector3::TransformCoord(const XMMATRIX& _m)
{
	return _tagVector3(XMVector3TransformCoord(this->Convert(), _m));
}