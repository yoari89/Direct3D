#include "PCHMath.h"
#include "CommonMath.h"

// 생성자
_tagMatrix::_tagMatrix()
{
	// XMMatrixIdentity() : 항등행렬을 리턴해주는 함수이다.
	m = XMMatrixIdentity();
}

_tagMatrix::_tagMatrix(const _tagMatrix& _m)
{
	m = _m.m;
}

_tagMatrix::_tagMatrix(const XMMATRIX& _m)
{
	m = _m;
}

Vector4& _tagMatrix::operator[](unsigned int _idx)
{
	return v[_idx];
}

// 연산자 오버로딩
// =
void _tagMatrix::operator=(const _tagMatrix& _m)
{
	m = _m.m;
}

void _tagMatrix::operator=(const XMMATRIX& _m)
{
	m = _m;
}

// *
_tagMatrix _tagMatrix::operator*(const _tagMatrix& _m) const
{
	return _tagMatrix(m * _m.m);
}

_tagMatrix _tagMatrix::operator*(const XMMATRIX& _m) const
{
	return _tagMatrix(m * _m);
}

_tagMatrix _tagMatrix::operator*(int _i) const
{
	return _tagMatrix(m * (float)_i);
}

_tagMatrix _tagMatrix::operator*(float _f) const
{
	return _tagMatrix(m * _f);
}

// *=
_tagMatrix& _tagMatrix::operator*=(const _tagMatrix& _m)
{
	m *= _m.m;
	return *this;
}

_tagMatrix& _tagMatrix::operator*=(const XMMATRIX& _m)
{
	m *= _m;
	return *this;
}

_tagMatrix& _tagMatrix::operator*=(int _i)
{
	m *= (float)_i;
	return *this;
}

_tagMatrix& _tagMatrix::operator*=(float _f)
{
	m *= _f;
	return *this;
}

// 단위행렬
XMMATRIX _tagMatrix::Identity()
{
	m = XMMatrixIdentity();
	return m;
}

// 전치행렬
XMMATRIX _tagMatrix::Transpose()
{
	m = XMMatrixTranspose(m);
	return m;
}

// 역행렬
XMMATRIX _tagMatrix::Inverse()
{
	m = XMMatrixInverse(&XMMatrixDeterminant(m), m);
	return m;
}

// 크기
XMMATRIX _tagMatrix::Scaling(float _x, float _y, float _z)
{
	m = XMMatrixScaling(_x, _y, _z);
	return m;
}

// 크기
XMMATRIX _tagMatrix::Scaling(const _tagVector3& _vScale)
{
	m = XMMatrixScaling(_vScale.x, _vScale.y, _vScale.z);
	return m;
}

// 회전(자전)
XMMATRIX _tagMatrix::Rotation(float _x, float _y, float _z)
{
	XMMATRIX matRotX, matRotY, matRotZ;
	matRotX = XMMatrixRotationX(DegreeToRadian(_x));
	matRotY = XMMatrixRotationY(DegreeToRadian(_y));
	matRotZ = XMMatrixRotationZ(DegreeToRadian(_z));
	m = matRotX * matRotY * matRotZ;
	return m;
}

// 회전(자전)
XMMATRIX _tagMatrix::Rotation(const _tagVector3& _vRot)
{
	XMMATRIX matRotX, matRotY, matRotZ;
	matRotX = XMMatrixRotationX(DegreeToRadian(_vRot.x));
	matRotY = XMMatrixRotationY(DegreeToRadian(_vRot.y));
	matRotZ = XMMatrixRotationZ(DegreeToRadian(_vRot.z));
	m = matRotX * matRotY * matRotZ;
	return m;
}

XMMATRIX _tagMatrix::RotationX(float _x)
{
	m = XMMatrixRotationX(DegreeToRadian(_x));
	return m;
}

XMMATRIX _tagMatrix::RotationY(float _y)
{
	m = XMMatrixRotationY(DegreeToRadian(_y));
	return m;
}

XMMATRIX _tagMatrix::RotationZ(float _z)
{
	m = XMMatrixRotationZ(DegreeToRadian(_z));
	return m;
}

XMMATRIX _tagMatrix::RotationAxis(float _fAngle, const _tagVector3 & _vAxis)
{
	m = XMMatrixRotationAxis(_vAxis.Convert(), DegreeToRadian(_fAngle));
	return m;
}

// 이동
XMMATRIX _tagMatrix::Translation(float _x, float _y, float _z)
{
	m = XMMatrixTranslation(_x, _y, _z);
	return m;
}

// 이동
XMMATRIX _tagMatrix::Translation(const _tagVector3& _vPos)
{
	m = XMMatrixTranslation(_vPos.x, _vPos.y, _vPos.z);
	return m;
}