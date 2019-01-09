#include "PCHMath.h"
#include "CommonMath.h"

_tagVector4	_tagVector4::Zero = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
_tagVector4	_tagVector4::One = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

// 색상
_tagVector4	_tagVector4::Red = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
_tagVector4	_tagVector4::Green = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
_tagVector4	_tagVector4::Blue = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
_tagVector4	_tagVector4::Black = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
_tagVector4	_tagVector4::White = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
_tagVector4	_tagVector4::Yellow = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
_tagVector4	_tagVector4::Magenta = Vector4(1.0f, 0.0f, 1.0f, 1.0f);

// 생성자
_tagVector4::_tagVector4() :
	x(0.0f),
	y(0.0f),
	z(0.0f),
	w(0.0f)
{
}

_tagVector4::_tagVector4(const _tagVector4& _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	w = _v.w;
}

_tagVector4::_tagVector4(const XMVECTOR& _v)
{
	XMStoreFloat4((XMFLOAT4*)this, _v);
}

_tagVector4::_tagVector4(const XMFLOAT4& _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	w = _v.w;
}

_tagVector4::_tagVector4(int _i)
{
	x = (float)_i;
	y = (float)_i;
	z = (float)_i;
}

_tagVector4::_tagVector4(int _arrI[4])
{
	x = (float)_arrI[0];
	y = (float)_arrI[1];
	z = (float)_arrI[2];
	w = (float)_arrI[4];
}

_tagVector4::_tagVector4(int _x, int _y, int _z, int _w)
{
	x = (float)_x;
	y = (float)_y;
	z = (float)_z;
	w = (float)_w;
}

_tagVector4::_tagVector4(float _f)
{
	x = _f;
	y = _f;
	z = _f;
	w = _f;
}

_tagVector4::_tagVector4(float _arrF[4])
{
	x = _arrF[0];
	y = _arrF[1];
	z = _arrF[2];
	w = _arrF[4];
}

_tagVector4::_tagVector4(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

// operator[]
float& _tagVector4::operator[](unsigned int _index)
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
	case 3:
	{
		return w;
	}
	}
	return x;
}

// =
void _tagVector4::operator=(const _tagVector4& _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	w = _v.w;
}

void _tagVector4::operator=(const XMVECTOR& _v)
{
	XMStoreFloat4((XMFLOAT4*)this, _v);
}

void _tagVector4::operator=(const XMFLOAT4& _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	w = _v.w;
}

void _tagVector4::operator=(int _i)
{
	x = (float)_i;
	y = (float)_i;
	z = (float)_i;
	w = (float)_i;
}

void _tagVector4::operator=(int _arrI[4])
{
	for (int j = 0; j < 4; ++j)
	{
		(*this)[j] = (float)_arrI[j];
	}
}

void _tagVector4::operator=(float _f)
{
	x = _f;
	y = _f;
	z = _f;
	w = _f;
}

void _tagVector4::operator=(float _arrF[4])
{
	x = _arrF[0];
	y = _arrF[1];
	z = _arrF[2];
	w = _arrF[4];
}

// +
_tagVector4 _tagVector4::operator+(const _tagVector4& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	v = v1 + v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator+(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v = v1 + _v;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator+(const XMFLOAT4& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = XMLoadFloat4(&_v);
	v = v1 + v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator+(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_i);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 + v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator+(int _arrI[4]) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_arrI);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 + v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator+(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_f);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 + v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator+(float _arrF[4]) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_arrF);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 + v2;

	return _tagVector4(v);
}

// +=
void _tagVector4::operator+=(const _tagVector4& _v)
{
	XMVECTOR v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = _v.Convert();
	v1 += v2;
	this->Convert(v1);
}

void _tagVector4::operator+=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 += _v;
	this->Convert(v1);
}

void _tagVector4::operator+=(const XMFLOAT4& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector4::operator+=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector4::operator+=(int _arrI[4])
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector4::operator+=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

void _tagVector4::operator+=(float _arrF[4])
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 += v2;
	this->Convert(v1);
}

// -
_tagVector4 _tagVector4::operator-(const _tagVector4& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	v = v1 - v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator-(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v = v1 - _v;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator-(const XMFLOAT4& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = XMLoadFloat4(&_v);
	v = v1 - v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator-(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_i);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator-(int _arrI[4]) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_arrI);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator-(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_f);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator-(float _arrF[4]) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_arrF);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 - v2;

	return _tagVector4(v);
}

// -=
void _tagVector4::operator-=(const _tagVector4& _v)
{
	XMVECTOR v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = _v.Convert();
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector4::operator-=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 -= _v;
	this->Convert(v1);
}

void _tagVector4::operator-=(const XMFLOAT4& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector4::operator-=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector4::operator-=(int _arrI[4])
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector4::operator-=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

void _tagVector4::operator-=(float _arrF[4])
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 -= v2;
	this->Convert(v1);
}

// *
_tagVector4 _tagVector4::operator*(const _tagVector4& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	v = v1 * v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator*(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v = v1 * _v;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator*(const XMFLOAT4& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = XMLoadFloat4(&_v);
	v = v1 * v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator*(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_i);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator*(int _arrI[4]) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_arrI);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator*(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_f);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator*(float _arrF[4]) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_arrF);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 * v2;

	return _tagVector4(v);
}

// *=
void _tagVector4::operator*=(const _tagVector4& _v)
{
	XMVECTOR v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = _v.Convert();
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector4::operator*=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 *= _v;
	this->Convert(v1);
}

void _tagVector4::operator*=(const XMFLOAT4& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector4::operator*=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector4::operator*=(int _arrI[4])
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector4::operator*=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

void _tagVector4::operator*=(float _arrF[4])
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 *= v2;
	this->Convert(v1);
}

// /
_tagVector4 _tagVector4::operator/(const _tagVector4& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	v = v1 / v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator/(const XMVECTOR& _v) const
{
	XMVECTOR v, v1;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v = v1 / _v;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator/(const XMFLOAT4& _v) const
{
	XMVECTOR v, v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = XMLoadFloat4(&_v);
	v = v1 / v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator/(int _i) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_i);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator/(int _arrI[4]) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_arrI);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator/(float _f) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_f);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector4(v);
}

_tagVector4 _tagVector4::operator/(float _arrF[4]) const
{
	XMVECTOR v, v1, v2;
	_tagVector4 tagVec(_arrF);
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = tagVec.Convert();
	v = v1 / v2;

	return _tagVector4(v);
}

// /=
void _tagVector4::operator/=(const _tagVector4& _v)
{
	XMVECTOR v1, v2;
	v1 = XMLoadFloat4((XMFLOAT4*)this);
	v2 = _v.Convert();
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector4::operator/=(const XMVECTOR& _v)
{
	XMVECTOR v1;
	v1 = this->Convert();
	v1 /= _v;
	this->Convert(v1);
}

void _tagVector4::operator/=(const XMFLOAT4& _v)
{
	XMVECTOR v1, v2;
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&_v);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector4::operator/=(int _i)
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_i);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector4::operator/=(int _arrI[4])
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_arrI);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector4::operator/=(float _f)
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_f);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

void _tagVector4::operator/=(float _arrF[4])
{
	XMVECTOR v1, v2;
	_tagVector4 tagVec(_arrF);
	v1 = this->Convert();
	v2 = XMLoadFloat4((XMFLOAT4*)&tagVec);
	v1 /= v2;
	this->Convert(v1);
}

// ==
bool _tagVector4::operator==(const _tagVector4& _v) const
{
	return x == _v.x && y == _v.y && z == _v.z;
}

bool _tagVector4::operator==(const XMVECTOR& _v) const
{
	_tagVector4 tagVec(_v);
	return x == tagVec.x && y == tagVec.y && z == tagVec.z;
}

bool _tagVector4::operator==(const XMFLOAT4& _v) const
{
	return x == _v.x && y == _v.y && z == _v.z;
}

bool _tagVector4::operator==(int _i) const
{
	return x == (float)_i && y == (float)_i && z == (float)_i;
}

bool _tagVector4::operator==(int _arrI[4]) const
{
	return x == (float)_arrI[0] && y == (float)_arrI[1] && z == (float)_arrI[2];
}

bool _tagVector4::operator==(float _f) const
{
	return x == _f && y == _f && z == _f;
}

bool _tagVector4::operator==(float _arrF[4]) const
{
	return x == _arrF[0] && y == _arrF[1] && z == _arrF[2];
}

// !=
bool _tagVector4::operator!=(const _tagVector4& _v) const
{
	return x != _v.x || y != _v.y || z != _v.z;
}

bool _tagVector4::operator!=(const XMVECTOR& _v) const
{
	_tagVector4 tagVec(_v);
	return x != tagVec.x || y != tagVec.y || z != tagVec.z;
}

bool _tagVector4::operator!=(const XMFLOAT4& _v) const
{
	return x != _v.x || y != _v.y || z == _v.z;
}

bool _tagVector4::operator!=(int _i) const
{
	return x != (float)_i || y != (float)_i || z != (float)_i;
}

bool _tagVector4::operator!=(int _arrI[4]) const
{
	return x != (float)_arrI[0] || y != (float)_arrI[1] || z != (float)_arrI[2];
}

bool _tagVector4::operator!=(float _f) const
{
	return x != _f || y != _f || z != _f;
}

bool _tagVector4::operator!=(float _arrF[4]) const
{
	return x != _arrF[0] || y != _arrF[1] || z != _arrF[2];
}

// 정규화
void _tagVector4::Normalize()
{
	// 자신의 _tagVector4 값을 XMVECTOR로 변환 후
	// XMVector3Normalize() 함수를 이용하여 정규화 시킨다.
	// this = const point 이기 때문에 값을 변경할 수 없다.
	XMVECTOR v = this->Convert();
	*this = XMVector3Normalize(v);
}

_tagVector4 _tagVector4::Normalize(const _tagVector4& _v)
{
	XMVECTOR v = _v.Convert();
	return _tagVector4(XMVector3Normalize(v));
}

// 변환
XMVECTOR _tagVector4::Convert() const
{
	return XMLoadFloat4((XMFLOAT4*)this);
}

void _tagVector4::Convert(const XMVECTOR& _v)
{
	XMStoreFloat4((XMFLOAT4*)this, _v);
}