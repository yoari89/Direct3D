#pragma once
#pragma warning(disable : 4251)

typedef union MATH_DLL MATH_ALIGN _tagMatrix
{
	// XMMATRIX = __declspec(align(16)) struct XMMATRIX
	XMMATRIX m;

	struct
	{
		float	_11, _12, _13, _14;
		float	_21, _22, _23, _24;
		float	_31, _32, _33, _34;
		float	_41, _42, _43, _44;
	};

	struct
	{
		Vector4 v[4];
	};

	_tagMatrix();
	_tagMatrix(const _tagMatrix& _m);
	_tagMatrix(const XMMATRIX& _m);

	// []
	Vector4& operator[] (unsigned int _idx);

	// =
	void operator= (const _tagMatrix& _m);
	void operator= (const XMMATRIX& _m);

	// 행렬은 나눗셈 연산이 없다.
	// XMMATRIX 구조체에서 * 연산자 오버로딩을 제공해준다.
	_tagMatrix operator* (const _tagMatrix& _m) const;
	_tagMatrix operator* (const XMMATRIX& _m)   const;
	_tagMatrix operator* (int _i)				const;
	_tagMatrix operator* (float _f)				const;

	// *=
	// XMMATRIX 구조체에서 *= 연산자 오버로딩을 제공해준다.
	_tagMatrix& operator*= (const _tagMatrix& _m);
	_tagMatrix& operator*= (const XMMATRIX& _m);
	_tagMatrix& operator*= (int _i);
	_tagMatrix& operator*= (float _f);

	// 단위행렬(항등행렬) 
	XMMATRIX Identity();

	// 전치행렬
	XMMATRIX Transpose();

	// 역행렬
	XMMATRIX Inverse();

	// 행렬 곱
	XMMATRIX Scaling(float _x, float _y, float _z);
	XMMATRIX Scaling(const union _tagVector3& _vScale);
	XMMATRIX Rotation(float _x, float _y, float _z);
	XMMATRIX Rotation(const union _tagVector3& _vRot);
	XMMATRIX RotationX(float _x);
	XMMATRIX RotationY(float _y);
	XMMATRIX RotationZ(float _z);
	XMMATRIX RotationAxis(float _fAngle, const union _tagVector3& _vAxis);
	XMMATRIX Translation(float _x, float _y, float _z);
	XMMATRIX Translation(const union _tagVector3& _vPos);

}Matrix, *PMatrix;