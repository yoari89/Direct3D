#pragma once
#include "Component/Component.h"

ENGINE_BEGIN
enum eTextRenderType
{
	TYPE_UI,
	TYPE_2D,
	TYPE_MAX,
};

enum eTextAlignH
{
	ALIGNH_LEFT,
	ALIGNH_CENTER,
	ALIGNH_RIGHT,
	ALIGNH_MAX,
};

enum eTextAlignV
{
	ALIGNV_TOP,
	ALIGNV_CENTER,
	ALIGNV_BOTTOM,
	ALIGNV_MAX,
};

class ENGINE_DLL CText : public CComponent
{
private:
	friend class CGameObject;

public:
	CText();
	CText(const CText& _text);
	virtual ~CText();

// 컴포넌트 클래스 가상함수 재정의
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CText* Clone()					override;

// 텍스트 레이아웃
// > 텍스트 레이아웃은 고정 텍스트이므로 사용하지 않는다.
// > ID2D1RenderTarget을 통하여 텍스트를 그리는 방식을 사용한다.
private:
	eTextAlignH				m_eAlignH;
	eTextAlignV				m_eAlignV;
	eTextRenderType			m_eTextType;
	D2D1_RECT_F				m_tRenderArea;
	ID2D1RenderTarget*		m_p2DTarget;
	IDWriteTextFormat*		m_pTextFormat;
	// IDWriteTextLayout*	m_pTextLayout;
/*
private:
	void CreateLayout();
*/
public:
	void SetTextType(eTextRenderType _eType);
	void SetAlignH(eTextAlignH _eAlign);
	void SetAlignV(eTextAlignV _eAlign);
	void SetRenderArea(float _left, float _top, float _right, float _bottom);

// 폰트
private:
	bool					m_isAlpha;
	float					m_fontSize;
	float					m_fontOpacity;
	const wchar_t*			m_pFont;
	const wchar_t*			m_pText;
	Vector4					m_vColor;
	ID2D1SolidColorBrush*	m_pBrush;

public:
	void AlphaBlend(bool _isAlpha);
	void SetFont(wchar_t* _pFont);
	void SetText(const wchar_t* _pText);
	void SetFontSize(float _fontSize);
	void SetColor(const Vector4& _vColor);
	void SetColor(float _r, float _g, float _b, float _a);
	void SetColor(unsigned char _r, unsigned char _g,
				  unsigned char _b, unsigned char _a);
	void SetFontOpacity(float _fontOpacity);

// 그림자
private:
	bool					m_isShadow;
	bool					m_isAlphaShadow;
	float					m_shadowOpacity;
	Vector3					m_vShadowOffset;
	Vector4					m_vShadowColor;
	D2D1_RECT_F				m_tShadowRenderArea;
	ID2D1SolidColorBrush*	m_pShadowBrush;

public:
	void Shadow(bool _isShadow);
	void ShadowAlphaBlend(bool _isAlpha);
	void SetShadowOpacity(float _shadowOpacity);
	void SetShadowOffset(const Vector3& _vOffset);
	void SetShadowColor(const Vector4& _vColor);
	void SetShadowColor(float _r, float _g, float _b, float _a);
	void SetShadowColor(unsigned char _r, unsigned char _g,
						unsigned char _b, unsigned char _a);
};
ENGINE_END