#include "PCHEngine.h"
#include "Text.h"
#include "Device.h"
#include "Utility/Font/FontManager.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/RenderState.h"
#include "Rendering/Shader.h"
#include "Resources/ResourcesManager.h"
#include "Resources/Mesh.h"
#include "Scene/Scene.h"
#include "Component/Camera.h"
#include "Component/Transform.h"

ENGINE_USING

CText::CText() :
	m_p2DTarget(nullptr),
	m_pTextFormat(nullptr),

	// ��Ʈ
	m_isAlpha(false),
	m_fontSize(0.f),
	m_fontOpacity(1.f),
	m_pBrush(nullptr),

	// �׸���
	m_isShadow(false),
	m_isAlphaShadow(false),
	m_shadowOpacity(1.f),
	m_pShadowBrush(nullptr)
{
	SetTag("Text");
	m_eComType = CT_TEXT;

	// ��Ʈ
	// > ��Ʈ Ű ���� "��ü+��Ʈ ũ��"�� ���Ѵ�.
	m_pText			= L"None";
	m_pFont			= L"�ü�ü";
	m_fontSize		= 10.f;
	m_vColor		= Vector4::White;
	m_pTextFormat	= GET_SINGLE(CFontManager)->FindFont("�ü�ü15");
	m_pBrush		= GET_SINGLE(CFontManager)->CreateBrush(m_vColor);

	// �׸���
	m_vShadowColor	= Vector4::Black;
	m_pShadowBrush	= GET_SINGLE(CFontManager)->CreateBrush(m_vShadowColor);

	// IDWriteTextFormat
	m_eTextType		= eTextRenderType::TYPE_UI;
	m_eAlignH		= eTextAlignH::ALIGNH_LEFT;
	m_eAlignV		= eTextAlignV::ALIGNV_CENTER;
	m_p2DTarget		= GET_SINGLE(CDevice)->Get2DRenderTarget();
}

CText::CText(const CText& _text) : CComponent(_text)
{
	*this = _text;
}

CText::~CText()
{
}

bool CText::Init()
{
	return true;
}

int CText::Input(float _fTime)
{
	return 0;
}

int CText::Update(float _fTime)
{
	return 0;
}

int CText::LateUpdate(float _fTime)
{
	return 0;
}

void CText::Collision(float _fTime)
{
}

void CText::Render(float _fTime)
{
	// ������ġ�� ���´�.
	Vector3	vPos = m_pTransform->GetWorldPos();
	float fHeignt = m_tRenderArea.bottom - m_tRenderArea.top;

	m_p2DTarget->BeginDraw();

	if (m_eTextType != eTextRenderType::TYPE_UI)
	{
		// UI Text�� ī�޶��� ������ ���� �ʴ´�.
		// > ī�޶� �̵��Ѹ�ŭ ��ġ�� ���ش�.
		CTransform* pCameraTr = m_pScene->GetMainCameraTransform();
		vPos -= pCameraTr->GetWorldPos();
		SAFE_RELEASE(pCameraTr);
	}
	
	// �׸���
	if (m_isShadow == true)
	{
		// > ���� ��
		Vector3	vShadowPos = vPos + m_vShadowOffset;

		// > ���� ��
		if (m_isAlphaShadow == true)
		{
			m_pShadowBrush->SetOpacity(m_shadowOpacity);
		}
		else
		{
			m_pShadowBrush->SetOpacity(1.f);
		}

		// ������ IDWriteTextLayout ��ü�� �ؽ�Ʈ �׸��ڸ� �׸���.
		UINT fontLength = ((wstring)m_pText).length();
		m_p2DTarget->DrawTextW(m_pText, fontLength, m_pTextFormat, m_tShadowRenderArea, m_pShadowBrush);
	}

	// ��Ʈ
	// > ������ IDWriteTextLayout ��ü�� �ؽ�Ʈ �Ǵ� �׸��ڸ� �׸���.
	UINT fontLength = ((wstring)m_pText).length();
	m_p2DTarget->DrawTextW(m_pText, fontLength, m_pTextFormat, m_tRenderArea, m_pBrush);

	m_p2DTarget->EndDraw();
}

void CText::AfterClone()
{
}

CText* CText::Clone()
{
	return new CText(*this);
}

void CText::SetTextType(eTextRenderType _eType)
{
	m_eTextType = _eType;
}

void CText::SetAlignH(eTextAlignH _eAlign)
{
	m_eAlignH = _eAlign;

	// ���̾ƿ� ���� �ؽ�Ʈ ���� ���� ���
	if (m_pTextFormat)
	{
		switch (m_eAlignH)
		{
			case eTextAlignH::ALIGNH_LEFT:
			{
				m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				break;
			}
			case eTextAlignH::ALIGNH_CENTER:
			{
				m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				break;
			}
			case eTextAlignH::ALIGNH_RIGHT:
			{
				m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
				break;
			}
		}
	}
}

void CText::SetAlignV(eTextAlignV _eAlign)
{
	m_eAlignV = _eAlign;

	// ���̾ƿ� ���� �ؽ�Ʈ ���� ���� ���
	switch (m_eAlignV)
	{
		case eTextAlignV::ALIGNV_TOP:
		{
			m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			break;
		}
		case eTextAlignV::ALIGNV_CENTER:
		{
			m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			break;
		}
		case eTextAlignV::ALIGNV_BOTTOM:
		{
			m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
			break;
		}
	}
}

void CText::SetRenderArea(float _left, float _top, float _right, float _bottom)
{
	// �ؽ�Ʈ ���̾ƿ� ���� ����
	// > ������ �������� ���̾ƿ��� �����.
	// > ������ ��ǥ�� -> ���ϴ� ���� ��ǥ�� �ٲ��ش�.
	Vector3 vPos = m_pTransform->GetWorldPos();
	_left	= _left;
	_top	= (float)_RESOLUTION.iHeight;
	_right  = _right;
	_bottom = (float)(_RESOLUTION.iHeight - _bottom);
	m_tRenderArea = D2D1::RectF(_left, _top, _right, _bottom);

	// �׸��� ����
	if (m_isShadow == true)
	{
		m_tShadowRenderArea.left	= m_tRenderArea.left   + m_vShadowOffset.x;
		m_tShadowRenderArea.top		= m_tRenderArea.top    - m_vShadowOffset.y;
		m_tShadowRenderArea.right	= m_tRenderArea.right  + m_vShadowOffset.x;
		m_tShadowRenderArea.bottom	= m_tRenderArea.bottom - m_vShadowOffset.y;
	}
}

void CText::AlphaBlend(bool _isAlpha)
{
	m_isAlpha = _isAlpha;
}

void CText::SetFont(wchar_t* _pFont)
{
	// ��Ʈ ����
	m_pFont = _pFont;

	// �� ������ ������ IDWriteTextFormat ��ü�� �����. 
	// > pfont �̸����� ������� ��Ʈ�� ������ MakeFont() �Լ������� IDWriteTextFormat ��ü�� �Ѱ��ش�.
	m_pTextFormat = GET_SINGLE(CFontManager)->MakeFont(m_pFont, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_WEIGHT_NORMAL,
													   DWRITE_FONT_STRETCH_ULTRA_EXPANDED, m_fontSize, L"ko");
}

void CText::SetText(const wchar_t* _pText)
{
	// �ؽ�Ʈ ����
	m_pText = _pText;
}

void CText::SetFontSize(float _fontSize)
{
	m_fontSize = _fontSize;
}

void CText::SetColor(const Vector4& _vColor)
{
	m_vColor = _vColor;
	m_pBrush = GET_SINGLE(CFontManager)->CreateBrush(m_vColor.x, m_vColor.y, m_vColor.z, m_vColor.w);
}

void CText::SetColor(float _r, float _g, float _b, float _a)
{
	m_vColor = Vector4(_r, _g, _b, _a);
	m_pBrush = GET_SINGLE(CFontManager)->CreateBrush(_r, _g, _b, _a);
}

void CText::SetColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
{
	m_vColor = Vector4(_r / 255.f, _g / 255.f, _b / 255.f, _a / 255.f);
	m_pBrush = GET_SINGLE(CFontManager)->CreateBrush(_r, _g, _b, _a);
}

void CText::SetFontOpacity(float _fOpacity)
{
	m_fontOpacity = _fOpacity;
}

void CText::Shadow(bool _isShadow)
{
	m_isShadow = _isShadow;
}

void CText::ShadowAlphaBlend(bool _isAlpha)
{
	m_isAlphaShadow = _isAlpha;
}

void CText::SetShadowOpacity(float _shadowOpacity)
{
	m_shadowOpacity = _shadowOpacity;
}

void CText::SetShadowOffset(const Vector3& _vOffset)
{
	m_vShadowOffset = _vOffset;
}

void CText::SetShadowColor(const Vector4& _vColor)
{
	m_vShadowColor = _vColor;
	m_pShadowBrush = GET_SINGLE(CFontManager)->CreateBrush(m_vShadowColor);
}

void CText::SetShadowColor(float _r, float _g, float _b, float _a)
{
	m_vShadowColor = Vector4(_r, _g, _b, _a);
	m_pShadowBrush = GET_SINGLE(CFontManager)->CreateBrush(_r, _g, _b, _a);
}

void CText::SetShadowColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
{
	m_vShadowColor = Vector4(_r / 255.f, _g / 255.f, _b / 255.f, _a / 255.f);
	m_pShadowBrush = GET_SINGLE(CFontManager)->CreateBrush(_r, _g, _b, _a);
}

/*
void CText::CreateLayout()
{
	// �ؽ�Ʈ ���̾ƿ� ����
	SAFE_RELEASE(m_pTextLayout);
	m_pTextLayout = GET_SINGLE(CFontManager)->
		CreateTextLayout(m_pText, m_pTextFormat, m_tRenderArea.right - m_tRenderArea.left,
						 m_tRenderArea.bottom - m_tRenderArea.top);

	DWRITE_TEXT_RANGE tRange;
	tRange.startPosition = 0;
	tRange.length = lstrlen(m_pText);

	// ������ �ؽ�Ʈ ���̾ƿ� ���� ���� �ؽ�Ʈ�� �۲� ũ�⸦ �����Ѵ�.
	m_pTextLayout->SetFontSize(m_fontSize, tRange);
}

void CText::SetRenderArea(float _left, float _bottom, float _right, float _top)
{
	// �ؽ�Ʈ ���̾ƿ� ���� ����
	// > ������ �������� ���̾ƿ��� �����.
	// > ������ ��ǥ�� -> ���ϴ� ���� ��ǥ�� �ٲ��ش�.
	m_tRenderArea = D2D1::RectF(_left, _bottom, _right, _top);
	CreateLayout();
}
*/