#pragma once

typedef union MATH_DLL _tagVector3
{
	// 정적 멤버 기저, Zero, One 벡터 정의
	static _tagVector3 Axis[3];
	static _tagVector3 Zero;
	static _tagVector3 One;

	struct
	{
		float x, y, z;
	};

	struct
	{
		float r, g, b;
	};

	_tagVector3();
	_tagVector3(const _tagVector3& _v);
	_tagVector3(const XMVECTOR& _v);
	_tagVector3(const XMFLOAT3& _v);
	_tagVector3(int _i);
	_tagVector3(int _arrI[3]);
	_tagVector3(int _x, int _y, int _z);
	_tagVector3(float _f);
	_tagVector3(float _arrF[3]);
	_tagVector3(float _x, float _y, float _z);

	// []
	float& operator[] (unsigned int _index);

	// =
	void operator= (const _tagVector3& _v);
	void operator= (const XMVECTOR& _v);
	void operator= (const XMFLOAT3& _v);
	void operator= (int _i);
	void operator= (int _arrI[3]);
	void operator= (float _f);
	void operator= (float _arrF[3]);

	// +
	_tagVector3 operator+ (const _tagVector3& _v)	const;
	_tagVector3 operator+ (const XMVECTOR& _v)		const;
	_tagVector3 operator+ (const XMFLOAT3& _v)		const;
	_tagVector3 operator+ (int _i)					const;
	_tagVector3 operator+ (int _arrI[3])			const;
	_tagVector3 operator+ (float _f)				const;
	_tagVector3 operator+ (float _arrF[3])			const;

	// +=
	void operator+= (const _tagVector3& _v);
	void operator+= (const XMVECTOR& _v);
	void operator+= (const XMFLOAT3& _v);
	void operator+= (int _i);
	void operator+= (int _arrI[3]);
	void operator+= (float _f);
	void operator+= (float _arrF[3]);

	// -
	_tagVector3 operator- (const _tagVector3& _v)	const;
	_tagVector3 operator- (const XMVECTOR& _v)		const;
	_tagVector3 operator- (const XMFLOAT3& _v)		const;
	_tagVector3 operator- (int _i)					const;
	_tagVector3 operator- (int _arrI[3])			const;
	_tagVector3 operator- (float _f)				const;
	_tagVector3 operator- (float _arrF[3])			const;

	// -=
	void operator-= (const _tagVector3& _v);
	void operator-= (const XMVECTOR& _v);
	void operator-= (const XMFLOAT3& _v);
	void operator-= (int _i);
	void operator-= (int _arrI[3]);
	void operator-= (float _f);
	void operator-= (float _arrF[3]);

	// *
	_tagVector3 operator* (const _tagVector3& _v)	const;
	_tagVector3 operator* (const XMVECTOR& _v)		const;
	_tagVector3 operator* (const XMFLOAT3& _v)		const;
	_tagVector3 operator* (int _i)					const;
	_tagVector3 operator* (int _arrI[3])			const;
	_tagVector3 operator* (float _f)				const;
	_tagVector3 operator* (float _arrF[3])			const;

	// *=
	void operator*= (const _tagVector3& _v);
	void operator*= (const XMVECTOR& _v);
	void operator*= (const XMFLOAT3& _v);
	void operator*= (int _i);
	void operator*= (int _arrI[3]);
	void operator*= (float _f);
	void operator*= (float _arrF[3]);

	// /
	_tagVector3 operator/ (const _tagVector3& _v)	const;
	_tagVector3 operator/ (const XMVECTOR& _v)		const;
	_tagVector3 operator/ (const XMFLOAT3& _v)		const;
	_tagVector3 operator/ (int _i)					const;
	_tagVector3 operator/ (int _arrI[3])			const;
	_tagVector3 operator/ (float _f)				const;
	_tagVector3 operator/ (float _arrF[3])			const;

	// /=
	void operator/= (const _tagVector3& _v);
	void operator/= (const XMVECTOR& _v);
	void operator/= (const XMFLOAT3& _v);
	void operator/= (int _i);
	void operator/= (int _arrI[3]);
	void operator/= (float _f);
	void operator/= (float _arrF[3]);

	// ==
	bool operator== (const _tagVector3& _v)	const;
	bool operator== (const XMVECTOR& _v)	const;
	bool operator== (const XMFLOAT3& _v)	const;
	bool operator== (int _i)				const;
	bool operator== (int _arrI[3])			const;
	bool operator== (float _f)				const;
	bool operator== (float _arrF[3])		const;

	// !=
	bool operator!= (const _tagVector3& _v)	const;
	bool operator!= (const XMVECTOR& _v)	const;
	bool operator!= (const XMFLOAT3& _v)	const;
	bool operator!= (int _i)				const;
	bool operator!= (int _arrI[3])			const;
	bool operator!= (float _f)				const;
	bool operator!= (float _arrF[3])		const;

	// 함수
	// 길이
	float Length() const;

	// 거리
	float Distance(const _tagVector3& _v) const;

	// 정규화
	void Normalize();
	static _tagVector3 Normalize(const _tagVector3& _v);

	// 타입 변환
	XMVECTOR Convert() const;
	void Convert(const XMVECTOR& _v);

	// 각도
	float Angle(const _tagVector3& _v)	const;
	float Angle(const XMVECTOR& _v)		const;
	float Angle(const XMFLOAT3& _v)		const;
	float Angle(int _arrI[3])			const;
	float Angle(float _arrF[3])			const;

	// 내적
	float Dot(const _tagVector3& _v) const;
	float Dot(const XMVECTOR& _v)	 const;
	float Dot(const XMFLOAT3& _v)	 const;
	float Dot(int _arrI[3])			 const;
	float Dot(float _arrF[3])		 const;

	// 외적
	_tagVector3 Cross(const _tagVector3& _v) const;
	_tagVector3 Cross(const XMVECTOR& _v)	 const;
	_tagVector3 Cross(const XMFLOAT3& _v)	 const;
	_tagVector3 Cross(int _arrI[3])			 const;
	_tagVector3 Cross(float _arrF[3])		 const;

	// 벡터 * 행렬 곱
	// union _tagMatrix& : 전방선언
	_tagVector3 TransformNormal(const union _tagMatrix& _m);
	_tagVector3 TransformNormal(const XMMATRIX& _m);
	_tagVector3 TransformCoord(const union _tagMatrix& _m);
	_tagVector3 TransformCoord(const XMMATRIX& _m);

}Vector3, *PVector3;