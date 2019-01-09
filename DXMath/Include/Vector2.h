#pragma once

typedef union MATH_DLL _tagVector2
{
	// 정적 멤버 기저, Zero, One 벡터 정의
	static _tagVector2 Zero;
	static _tagVector2 One;

	struct
	{
		float x, y;
	};

	_tagVector2();
	_tagVector2(const _tagVector2& _v);
	_tagVector2(const XMVECTOR& _v);
	_tagVector2(const XMFLOAT3& _v);
	_tagVector2(int _i);
	_tagVector2(int _arrI[2]);
	_tagVector2(int _x, int _y);
	_tagVector2(float _f);
	_tagVector2(float _arrF[2]);
	_tagVector2(float _x, float _y);

	// []
	float& operator[] (unsigned int _index);

	// =
	void operator= (const _tagVector2& _v);
	void operator= (const XMVECTOR& _v);
	void operator= (const XMFLOAT2& _v);
	void operator= (int _i);
	void operator= (int _arrI[2]);
	void operator= (float _f);
	void operator= (float _arrF[2]);

	// +
	_tagVector2 operator+ (const _tagVector2& _v)	const;
	_tagVector2 operator+ (const XMVECTOR& _v)		const;
	_tagVector2 operator+ (const XMFLOAT2& _v)		const;
	_tagVector2 operator+ (int _i)					const;
	_tagVector2 operator+ (int _arrI[2])			const;
	_tagVector2 operator+ (float _f)				const;
	_tagVector2 operator+ (float _arrF[2])			const;

	// +=
	void operator+= (const _tagVector2& _v);
	void operator+= (const XMVECTOR& _v);
	void operator+= (const XMFLOAT2& _v);
	void operator+= (int _i);
	void operator+= (int _arrI[2]);
	void operator+= (float _f);
	void operator+= (float _arrF[2]);

	// -
	_tagVector2 operator- (const _tagVector2& _v)	const;
	_tagVector2 operator- (const XMVECTOR& _v)		const;
	_tagVector2 operator- (const XMFLOAT2& _v)		const;
	_tagVector2 operator- (int _i)					const;
	_tagVector2 operator- (int _arrI[2])			const;
	_tagVector2 operator- (float _f)				const;
	_tagVector2 operator- (float _arrF[2])			const;

	// -=
	void operator-= (const _tagVector2& _v);
	void operator-= (const XMVECTOR& _v);
	void operator-= (const XMFLOAT2& _v);
	void operator-= (int _i);
	void operator-= (int _arrI[2]);
	void operator-= (float _f);
	void operator-= (float _arrF[2]);

	// *
	_tagVector2 operator* (const _tagVector2& _v)	const;
	_tagVector2 operator* (const XMVECTOR& _v)		const;
	_tagVector2 operator* (const XMFLOAT2& _v)		const;
	_tagVector2 operator* (int _i)					const;
	_tagVector2 operator* (int _arrI[2])			const;
	_tagVector2 operator* (float _f)				const;
	_tagVector2 operator* (float _arrF[2])			const;

	// *=
	void operator*= (const _tagVector2& _v);
	void operator*= (const XMVECTOR& _v);
	void operator*= (const XMFLOAT2& _v);
	void operator*= (int _i);
	void operator*= (int _arrI[2]);
	void operator*= (float _f);
	void operator*= (float _arrF[2]);

	// /
	_tagVector2 operator/ (const _tagVector2& _v)	const;
	_tagVector2 operator/ (const XMVECTOR& _v)		const;
	_tagVector2 operator/ (const XMFLOAT2& _v)		const;
	_tagVector2 operator/ (int _i)					const;
	_tagVector2 operator/ (int _arrI[2])			const;
	_tagVector2 operator/ (float _f)				const;
	_tagVector2 operator/ (float _arrF[2])			const;

	// /=
	void operator/= (const _tagVector2& _v);
	void operator/= (const XMVECTOR& _v);
	void operator/= (const XMFLOAT2& _v);
	void operator/= (int _i);
	void operator/= (int _arrI[2]);
	void operator/= (float _f);
	void operator/= (float _arrF[2]);

	// ==
	bool operator== (const _tagVector2& _v)	const;
	bool operator== (const XMVECTOR& _v)	const;
	bool operator== (const XMFLOAT2& _v)	const;
	bool operator== (int _i)				const;
	bool operator== (int _arrI[2])			const;
	bool operator== (float _f)				const;
	bool operator== (float _arrF[2])		const;

	// !=
	bool operator!= (const _tagVector2& _v)	const;
	bool operator!= (const XMVECTOR& _v)	const;
	bool operator!= (const XMFLOAT2& _v)	const;
	bool operator!= (int _i)				const;
	bool operator!= (int _arrI[2])			const;
	bool operator!= (float _f)				const;
	bool operator!= (float _arrF[2])		const;

	// 함수
	// 길이
	float Length() const;

	// 정규화
	void Normalize();
	static _tagVector2 Normalize(const _tagVector2& _v);

	// 타입 변환
	XMVECTOR Convert() const;
	void Convert(const XMVECTOR& _v);

}Vector2, *PVector2;