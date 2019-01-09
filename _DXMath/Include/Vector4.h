#pragma once

typedef union MATH_DLL _tagVector4
{
	// 정적 멤버 기저, Zero, One 벡터 정의
	static _tagVector4	Zero;
	static _tagVector4	One;

	// Color
	static _tagVector4 Red;
	static _tagVector4 Green;
	static _tagVector4 Blue;
	static _tagVector4 Black;
	static _tagVector4 White;
	static _tagVector4 Yellow;
	static _tagVector4 Magenta;

	struct
	{
		float	x, y, z, w;
	};

	struct
	{
		float	r, g, b, a;
	};

	_tagVector4();
	_tagVector4(const _tagVector4& _v);
	_tagVector4(const XMVECTOR& _v);
	_tagVector4(const XMFLOAT4& _v);
	_tagVector4(int _i);
	_tagVector4(int _arrI[4]);
	_tagVector4(int _x, int _y, int _z, int _w);
	_tagVector4(float _f);
	_tagVector4(float _arrF[4]);
	_tagVector4(float _x, float _y, float _z, float _w);

	// []
	float& operator[] (unsigned int _index);

	// =
	void operator= (const _tagVector4& _v);
	void operator= (const XMVECTOR& _v);
	void operator= (const XMFLOAT4& _v);
	void operator= (int _i);
	void operator= (int _arrI[4]);
	void operator= (float _f);
	void operator= (float _arrF[4]);

	// +
	_tagVector4 operator+ (const _tagVector4& _v)	const;
	_tagVector4 operator+ (const XMVECTOR& _v)		const;
	_tagVector4 operator+ (const XMFLOAT4& _v)		const;
	_tagVector4 operator+ (int _i)					const;
	_tagVector4 operator+ (int _arrI[4])			const;
	_tagVector4 operator+ (float _f)				const;
	_tagVector4 operator+ (float _arrF[4])			const;

	// +=
	void operator+= (const _tagVector4& _v);
	void operator+= (const XMVECTOR& _v);
	void operator+= (const XMFLOAT4& _v);
	void operator+= (int _i);
	void operator+= (int _arrI[4]);
	void operator+= (float _f);
	void operator+= (float _arrF[4]);

	// -
	_tagVector4 operator- (const _tagVector4& _v)	const;
	_tagVector4 operator- (const XMVECTOR& _v)		const;
	_tagVector4 operator- (const XMFLOAT4& _v)		const;
	_tagVector4 operator- (int _i)					const;
	_tagVector4 operator- (int _arrI[4])			const;
	_tagVector4 operator- (float _f)				const;
	_tagVector4 operator- (float _arrF[4])			const;

	// -=
	void operator-= (const _tagVector4& _v);
	void operator-= (const XMVECTOR& _v);
	void operator-= (const XMFLOAT4& _v);
	void operator-= (int _i);
	void operator-= (int _arrI[4]);
	void operator-= (float _f);
	void operator-= (float _arrF[4]);

	// *
	_tagVector4 operator* (const _tagVector4& _v)	const;
	_tagVector4 operator* (const XMVECTOR& _v)		const;
	_tagVector4 operator* (const XMFLOAT4& _v)		const;
	_tagVector4 operator* (int _i)					const;
	_tagVector4 operator* (int _arrI[4])			const;
	_tagVector4 operator* (float _f)				const;
	_tagVector4 operator* (float _arrF[4])			const;

	// *=
	void operator*= (const _tagVector4& _v);
	void operator*= (const XMVECTOR& _v);
	void operator*= (const XMFLOAT4& _v);
	void operator*= (int _i);
	void operator*= (int _arrI[4]);
	void operator*= (float _f);
	void operator*= (float _arrF[4]);

	// /
	_tagVector4 operator/ (const _tagVector4& _v)	const;
	_tagVector4 operator/ (const XMVECTOR& _v)		const;
	_tagVector4 operator/ (const XMFLOAT4& _v)		const;
	_tagVector4 operator/ (int _i)					const;
	_tagVector4 operator/ (int _arrI[4])			const;
	_tagVector4 operator/ (float _f)				const;
	_tagVector4 operator/ (float _arrF[4])			const;

	// /=
	void operator/= (const _tagVector4& _v);
	void operator/= (const XMVECTOR& _v);
	void operator/= (const XMFLOAT4& _v);
	void operator/= (int _i);
	void operator/= (int _arrI[4]);
	void operator/= (float _f);
	void operator/= (float _arrF[4]);

	// ==
	bool operator== (const _tagVector4& _v)	const;
	bool operator== (const XMVECTOR& _v)	const;
	bool operator== (const XMFLOAT4& _v)	const;
	bool operator== (int _i)				const;
	bool operator== (int _arrI[4])			const;
	bool operator== (float _f)				const;
	bool operator== (float _arrF[4])		const;

	// !=
	bool operator!= (const _tagVector4& _v)	const;
	bool operator!= (const XMVECTOR& _v)	const;
	bool operator!= (const XMFLOAT4& _v)	const;
	bool operator!= (int _i)				const;
	bool operator!= (int _arrI[4])			const;
	bool operator!= (float _f)				const;
	bool operator!= (float _arrF[4])		const;

	// 함수
	// 정규화
	void Normalize();
	static _tagVector4 Normalize(const _tagVector4& _v);

	// 타입 변환
	XMVECTOR Convert() const;
	void Convert(const XMVECTOR& _v);

}Vector4, *PVector4;