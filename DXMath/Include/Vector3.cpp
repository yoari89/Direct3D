#include "PCHMath.h"
#include "CommonMath.h"

_tagVector3	_tagVector3::Axis[3] = { Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) };
_tagVector3	_tagVector3::Zero = Vector3(0.0f, 0.0f, 0.0f);
_tagVector3	_tagVector3::One = Vector3(1.0f, 1.0f, 1.0f);

// ������
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
	// SIMD�� ����ϱ� ���ؼ��� XMVECTOR Ÿ���� �̿��ؾ� �Ѵ�.
	// XMStore = XMVECTOR Ÿ���� XMFLOAT* Ÿ������ ��ȯ�ϴµ� ���
	// XMFLOAT3 = ����ü�� x, y, z ����� ������ �ִ�. 
	//            this�� XMFLOAT3 ���·� ����ȯ�� ������ ����. (�޸� ũ�� ��ġ)
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

// ������ �����ε�
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
	// XMLoad = XMFLOAT*�� XMVECTOR�� ��ȯ�ϴµ� ���
	// XMFLOAT3 = ����ü�� x, y, z ����� ������ �ִ�. 
	//            this�� XMFLOAT3 ���·� ����ȯ�� ������ ����. (�޸� ũ�� ��ġ)
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

	// _tagVec.Convert() = _tagVec(_tagVector3 Ÿ��)�� XMVECTOR Ÿ������ ����ȯ
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
	// v1�� �Լ� ȣ����(this)�� ������ ��ü�� �ȴ�. (���� ����ȴ�.)
	// operator+= ������ �����ε� �Լ��� v1�� �Լ� ȣ���ڷ� ����ȴ�.
	XMVECTOR v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = _v.Convert();
	v1 += v2;

	// Convert(XMVECTOR Ÿ��) -> XMFLOAT3* Ÿ������ ��ȯ�ȴ�.
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
	// v1�� �Լ� ȣ����(this)�� ������ ��ü�� �ȴ�. (���� ����ȴ�.)
	// operator+= ������ �����ε� �Լ��� v1�� �Լ� ȣ���ڷ� ����ȴ�.
	XMVECTOR v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = _v.Convert();
	v1 -= v2;

	// Convert(XMVECTOR Ÿ��) -> XMFLOAT3* Ÿ������ ��ȯ�ȴ�.
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
	// v1�� �Լ� ȣ����(this)�� ������ ��ü�� �ȴ�. (���� ����ȴ�.)
	// operator+= ������ �����ε� �Լ��� v1�� �Լ� ȣ���ڷ� ����ȴ�.
	XMVECTOR v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = _v.Convert();
	v1 *= v2;

	// Convert(XMVECTOR Ÿ��) -> XMFLOAT3* Ÿ������ ��ȯ�ȴ�.
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
	// v1�� �Լ� ȣ����(this)�� ������ ��ü�� �ȴ�. (���� ����ȴ�.)
	// operator+= ������ �����ε� �Լ��� v1�� �Լ� ȣ���ڷ� ����ȴ�.
	XMVECTOR v1, v2;
	v1 = XMLoadFloat3((XMFLOAT3*)this);
	v2 = _v.Convert();
	v1 /= v2;

	// Convert(XMVECTOR Ÿ��) -> XMFLOAT3* Ÿ������ ��ȯ�ȴ�.
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

// ����
float _tagVector3::Length() const
{
	XMVECTOR v = this->Convert();

	// XMVector3Length : 3D ������ ���̸� ����մϴ�.
	return XMVectorGetX(XMVector3Length(v));
}

// �Ÿ�
float _tagVector3::Distance(const _tagVector3& _v) const
{
	return (*this - _v).Length();
}

// ����ȭ
void _tagVector3::Normalize()
{
	// �ڽ��� _tagVector3 ���� XMVECTOR�� ��ȯ ��
	// XMVector3Normalize() �Լ��� �̿��Ͽ� ����ȭ ��Ų��.
	// this = const point �̱� ������ ���� ������ �� ����.
	XMVECTOR v = this->Convert();
	*this = XMVector3Normalize(v);
}

_tagVector3 _tagVector3::Normalize(const _tagVector3& _v)
{
	// ���ڷ� �Ѿ�� _tagVector3& ���۷��� ������ 
	// ���������� XMVECTOR v ������ ���� ����ȭ �� v ���� ��ȯ
	XMVECTOR v = _v.Convert();
	return _tagVector3(XMVector3Normalize(v));
}

// ��ȯ
XMVECTOR _tagVector3::Convert() const
{
	return XMLoadFloat3((XMFLOAT3*)this);
}

void _tagVector3::Convert(const XMVECTOR& _v)
{
	// XMVECTORŸ�� _v ������ XMFLOAT3* Ÿ������ ��ȯ
	// _v�� ���۷��� Ÿ���̹Ƿ� _v�� ���� ���� ����ȴ�.
	XMStoreFloat3((XMFLOAT3*)this, _v);
}

// ����
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

// ����
float _tagVector3::Dot(const _tagVector3& _v) const
{
	return Dot(_v.Convert());
}

float _tagVector3::Dot(const XMVECTOR& _v) const
{
	// XMVector3Dot : 3D ���Ͱ��� ������ ���
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

// ����
_tagVector3 _tagVector3::Cross(const _tagVector3& _v) const
{
	return Cross(_v.Convert());
}

_tagVector3 _tagVector3::Cross(const XMVECTOR& _v) const
{
	// XMVector3Cross : 3D ���� ������ ������ ���
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

// ���� * ��� ��
_tagVector3 _tagVector3::TransformNormal(const _tagMatrix& _m)
{
	// Normal : ���� ����
	return _tagVector3(XMVector3TransformNormal(this->Convert(), _m.m));
}

_tagVector3 _tagVector3::TransformNormal(const XMMATRIX& _m)
{
	return _tagVector3(XMVector3TransformNormal(this->Convert(), _m));
}

_tagVector3 _tagVector3::TransformCoord(const _tagMatrix& _m)
{
	// Coord : w���� 1�̸�, �̵������ ���� ����Ѵ�.
	//         w���� 0�̸� ��� ��� ����� ��ġ�� ����ϴ� ���� ��� 0�� �ȴ�.
	return _tagVector3(XMVector3TransformCoord(this->Convert(), _m.m));
}

_tagVector3 _tagVector3::TransformCoord(const XMMATRIX& _m)
{
	return _tagVector3(XMVector3TransformCoord(this->Convert(), _m));
}