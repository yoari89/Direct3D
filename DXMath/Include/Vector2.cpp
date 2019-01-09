#include "PCHMath.h"

_tagVector2	_tagVector2::Zero = Vector2(0.0f, 0.0f);
_tagVector2	_tagVector2::One  = Vector2(1.0f, 1.0f);

// 생성자
_tagVector2::_tagVector2() :
	x(0.0f),
	y(0.0f)
{
}

_tagVector2::_tagVector2(const _tagVector2 & _v)
{
	x = _v.x;
	y = _v.y;
}

_tagVector2::_tagVector2(const XMVECTOR & _v)
{
	XMStoreFloat2((XMFLOAT2*)this, _v);
}

_tagVector2::_tagVector2(const XMFLOAT3 & _v)
{
	x = _v.x;
	y = _v.y;
}

_tagVector2::_tagVector2(int _i)
{
	x = (float)_i;
	y = (float)_i;
}

_tagVector2::_tagVector2(int _arrI[2])
{
	x = (float)_arrI[0];
	y = (float)_arrI[1];
}

_tagVector2::_tagVector2(int _x, int _y)
{
	x = (float)_x;
	y = (float)_y;
}

_tagVector2::_tagVector2(float _f)
{
	x = _f;
	y = _f;
}

_tagVector2::_tagVector2(float _arrF[2])
{
	x = _arrF[0];
	y = _arrF[1];
}

_tagVector2::_tagVector2(float _x, float _y)
{
	x = _x;
	y = _y;
}

// 연산자 오버로딩
// []
float& _tagVector2::operator[](unsigned int _index)
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
	}
	return x;
}

// =
void _tagVector2::operator=(const _tagVector2& _v)
{
	x = _v.x;
	y = _v.y;
}

void _tagVector2::operator=(const XMVECTOR& _v)
{
	XMStoreFloat2((XMFLOAT2*)this, _v);
}

void _tagVector2::operator=(const XMFLOAT2& _v)
{
	x = _v.x;
	y = _v.y;
}

void _tagVector2::operator=(int _i)
{
	x = (float)_i;
	y = (float)_i;
}

void _tagVector2::operator=(int _arrI[2])
{
	for (int j = 0; j < 2; ++j)
	{
		(*this)[j] = (float)_arrI[j];
	}
}

void _tagVector2::operator=(float _f)
{
	x = _f;
	y = _f;
}

void _tagVector2::operator=(float _arrF[2])
{
	x = _arrF[0];
	y = _arrF[1];
}

// +
_tagVector2 _tagVector2::operator+(const _tagVector2& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	v = v1 + v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator+(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v = v1 + _v;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator+(const XMFLOAT2& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = XMLoadFloat2(&_v);
	v = v1 + v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator+(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_i);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 + v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator+(int _arrI[2]) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_arrI);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 + v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator+(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_f);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 + v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator+(float _arrF[2]) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_arrF);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 + v2;

	return _tagVector2(v);
}

// +=
void _tagVector2::operator+=(const _tagVector2& _v)
{
	XMVECTOR v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = _v.Convert();
	v1 += v2;
	this->Convert(v1);
}

void _tagVector2::operator+=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 += _v;
	this->Convert(v1);
}

void _tagVector2::operator+=(const XMFLOAT2& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector2::operator+=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector2::operator+=(int _arrI[2])
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector2::operator+=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector2::operator+=(float _arrF[2])
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

// -
_tagVector2 _tagVector2::operator-(const _tagVector2& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	v = v1 - v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator-(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v = v1 - _v;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator-(const XMFLOAT2& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = XMLoadFloat2(&_v);
	v = v1 - v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator-(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_i);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator-(int _arrI[2]) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_arrI);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator-(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_f);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator-(float _arrF[2]) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_arrF);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector2(v);
}

// -=
void _tagVector2::operator-=(const _tagVector2& _v)
{
	XMVECTOR v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = _v.Convert();
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector2::operator-=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 -= _v;
	this->Convert(v1);
}

void _tagVector2::operator-=(const XMFLOAT2& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector2::operator-=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector2::operator-=(int _arrI[2])
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector2::operator-=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector2::operator-=(float _arrF[2])
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

// *
_tagVector2 _tagVector2::operator*(const _tagVector2& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	v = v1 * v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator*(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v = v1 * _v;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator*(const XMFLOAT2& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = XMLoadFloat2(&_v);
	v = v1 * v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator*(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_i);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator*(int _arrI[2]) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_arrI);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator*(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_f);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator*(float _arrF[2]) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_arrF);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector2(v);
}

// *=
void _tagVector2::operator*=(const _tagVector2& _v)
{
	XMVECTOR v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = _v.Convert();
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector2::operator*=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 *= _v;
	this->Convert(v1);
}

void _tagVector2::operator*=(const XMFLOAT2& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector2::operator*=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector2::operator*=(int _arrI[2])
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector2::operator*=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector2::operator*=(float _arrF[2])
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

// /
_tagVector2 _tagVector2::operator/(const _tagVector2& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	v = v1 / v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator/(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v = v1 / _v;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator/(const XMFLOAT2& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = XMLoadFloat2(&_v);
	v = v1 / v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator/(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_i);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator/(int _arrI[2]) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_arrI);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator/(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_f);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector2(v);
}

_tagVector2 _tagVector2::operator/(float _arrF[2]) const
{
	XMVECTOR v, v1, v2;
	_tagVector2 tagVec(_arrF);
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector2(v);
}

// /=
void _tagVector2::operator/=(const _tagVector2& _v)
{
	XMVECTOR v1, v2;
	v1 = XMLoadFloat2((XMFLOAT2*)this);
	v2 = _v.Convert();
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector2::operator/=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 /= _v;
	this->Convert(v1);
}

void _tagVector2::operator/=(const XMFLOAT2& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&_v);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector2::operator/=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector2::operator/=(int _arrI[2])
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector2::operator/=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector2::operator/=(float _arrF[2])
{
	XMVECTOR v1, v2;
	_tagVector2 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat2((XMFLOAT2*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

// == 
bool _tagVector2::operator==(const _tagVector2& _v) const
{
	return x == _v.x && y == _v.y;
}

bool _tagVector2::operator==(const XMVECTOR& _v) const
{
	_tagVector2 tagVec(_v);
	return x == tagVec.x && y == tagVec.y;
}

bool _tagVector2::operator==(const XMFLOAT2& _v) const
{
	return x == _v.x && y == _v.y;
}

bool _tagVector2::operator==(int _i) const
{
	return x == (float)_i && y == (float)_i;
}

bool _tagVector2::operator==(int _arrI[2]) const
{
	return x == (float)_arrI[0] && y == (float)_arrI[1];
}

bool _tagVector2::operator==(float _f) const
{
	return x == _f && y == _f;
}

bool _tagVector2::operator==(float _arrF[2]) const
{
	return x == _arrF[0] && y == _arrF[1];
}

// !=
bool _tagVector2::operator!=(const _tagVector2& _v) const
{
	return x != _v.x || y != _v.y;
}

bool _tagVector2::operator!=(const XMVECTOR& _v) const
{
	_tagVector2 tagVec(_v);
	return x != tagVec.x || y != tagVec.y;
}

bool _tagVector2::operator!=(const XMFLOAT2& _v) const
{
	return x != _v.x || y != _v.y;
}

bool _tagVector2::operator!=(int _i) const
{
	return x != (float)_i || y != (float)_i;
}

bool _tagVector2::operator!=(int _arrI[2]) const
{
	return x != (float)_arrI[0] || y != (float)_arrI[1];
}

bool _tagVector2::operator!=(float _f) const
{
	return x != _f || y != _f;
}

bool _tagVector2::operator!=(float _arrF[2]) const
{
	return x != _arrF[0] || y != _arrF[1];
}

// 길이
float _tagVector2::Length() const
{
	XMVECTOR v = Convert();
	return XMVectorGetX(XMVector2Length(v));
}

// 정규화
void _tagVector2::Normalize()
{
	XMVECTOR v = this->Convert();
	*this = XMVector2Normalize(v);
}

_tagVector2 _tagVector2::Normalize(const _tagVector2& _v)
{
	XMVECTOR v = _v.Convert();
	return _tagVector2(XMVector2Normalize(v));
}

// 변환
XMVECTOR _tagVector2::Convert() const
{
	return XMLoadFloat2((XMFLOAT2*)this);
}

void _tagVector2::Convert(const XMVECTOR& _v)
{
	XMStoreFloat2((XMFLOAT2*)this, _v);
}