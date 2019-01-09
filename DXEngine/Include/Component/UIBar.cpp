#include "PCHEngine.h"
#include "UIBar.h"
#include "Component/Renderer.h"
#include "Component/Material.h"
#include "Component/Transform.h"
#include "Component/ColliderRect.h"
#include "Component/Animation2D.h"
#include "Object/GameObject.h"

ENGINE_USING

CUIBar::CUIBar() :
	m_pCollider(nullptr)
{
	SetTag("UIBar");
	m_eUIType = UT_BAR;
}

CUIBar::CUIBar(const CUIBar& _uiBar) :
	CUI(_uiBar)
{
	m_eDir		= _uiBar.m_eDir;
	m_fValue	= _uiBar.m_fValue;
	m_fMinValue = _uiBar.m_fMinValue;
	m_fMaxValue = _uiBar.m_fMaxValue;
	m_tCBuffer = _uiBar.m_tCBuffer;
	m_fValueLength = _uiBar.m_fValueLength;
	m_vScale = _uiBar.m_vScale;
}

CUIBar::~CUIBar()
{
	SAFE_RELEASE(m_pCollider);
}

void CUIBar::SetBarDir(BAR_DIR _eDir)
{
	m_eDir = _eDir;
	m_tCBuffer.iMoveDir = _eDir;
	Vector3	vPos;

	switch (m_eDir)
	{
		case BD_LEFT:
		{
			m_pTransform->SetWorldPivot(0.f, 0.f, 0.f);
			break;
		}
		case BD_RIGHT:
		{
			m_pTransform->SetWorldPivot(1.f, 0.f, 0.f);
			vPos.x = m_pTransform->GetWorldScale().x;
			m_pTransform->Move(vPos);
			break;
		}
		case BD_UP:
		{
			m_pTransform->SetWorldPivot(0.f, 1.f, 0.f);
			vPos.y = m_pTransform->GetWorldScale().y;
			m_pTransform->Move(vPos);
			break;
		}
		case BD_DOWN:
		{
			m_pTransform->SetWorldPivot(0.f, 0.f, 0.f);
			break;
		}
	}
}

void CUIBar::SetValue(float _fValue)
{
	m_fValue = _fValue;

	if (m_fValue < m_fMinValue)
		m_fValue = m_fMinValue;

	else if (m_fValue > m_fMaxValue)
		m_fValue = m_fMaxValue;

	m_tCBuffer.fPercent = (m_fValue - m_fMinValue) / m_fValueLength;
}

void CUIBar::AddValue(float _fValue)
{
	m_fValue += _fValue;

	if (m_fValue < m_fMinValue)
	{
		m_fValue = m_fMinValue;
	}
	else if (m_fValue > m_fMaxValue)
	{
		m_fValue = m_fMaxValue;
	}

	m_tCBuffer.fPercent = (m_fValue - m_fMinValue) / m_fValueLength;
}

void CUIBar::SetMinMaxValue(float _fMin, float _fMax)
{
	m_fMinValue = _fMin;
	m_fMaxValue = _fMax;
	if (m_fValue < m_fMinValue)
	{
		m_fValue = m_fMinValue;
	}
	else if (m_fValue > m_fMaxValue)
	{
		m_fValue = m_fMaxValue;
	}

	m_fValueLength = m_fMaxValue - m_fMinValue;

	m_tCBuffer.fPercent = (m_fValue - m_fMinValue) / m_fValueLength;
}

void CUIBar::SetScale(const Vector3 & vScale)
{
	m_vScale = vScale;

	m_pTransform->SetWorldScale(m_vScale);
}

void CUIBar::SetScale(float x, float y, float z)
{
	m_vScale = Vector3(x, y, z);

	m_pTransform->SetWorldScale(m_vScale);
}

void CUIBar::LightOn()
{
	m_tCBuffer.vLight = Vector4::White * 2.f;
}

void CUIBar::LightOff()
{
	m_tCBuffer.vLight = Vector4::White;
}

void CUIBar::Hit(CCollider * pSrc, CCollider * pDest, float fTime)
{
}

void CUIBar::MouseOut(CCollider * pSrc, CCollider * pDest, float fTime)
{
}

bool CUIBar::Init()
{
	m_fMinValue = 0.f;
	m_fMaxValue = 100.f;
	m_fValue	= 100.f;
	m_tCBuffer.fPercent = 1.f;
	m_tCBuffer.vLight = Vector4::White;

	m_fValueLength = 100.f;

	CRenderer*	pRenderer = m_pObject->AddComponent<CRenderer>("BarRenderer");

	pRenderer->SetMesh("TexRect");
	pRenderer->SetRenderState(ALPHA_BLEND);
	pRenderer->SetShader(BAR_SHADER);

	pRenderer->CreateRendererCBuffer("Bar", sizeof(BarCBuffer));

	SAFE_RELEASE(pRenderer);

	CMaterial*	pMaterial = m_pObject->FindComponentFromType<CMaterial>(CT_MATERIAL);

	pMaterial->SetDiffuseTex(0, "Bar", TEXT("Bar.png"));

	SAFE_RELEASE(pMaterial);

	m_pTransform->SetWorldScale(200.f, 30.f, 1.f);
	m_pTransform->SetWorldPivot(0.5f, 0.5f, 0.f);

	CColliderRect*	pRC = AddComponent<CColliderRect>("BarBody");

	m_pCollider = pRC;

	pRC->SetCollisionGroup("UI");
	pRC->SetInfo(Vector3(0.f, 0.f, 0.f), Vector3(200.f, 30.f, 0.f));
	pRC->SetCollisionCallback(CCT_ENTER, this, &CUIBar::Hit);
	pRC->SetCollisionCallback(CCT_LEAVE, this, &CUIBar::MouseOut);

	SetBarDir(BD_LEFT);

	return true;
}

int CUIBar::Input(float fTime)
{
	return 0;
}

int CUIBar::Update(float fTime)
{
	Vector3	vScale = m_vScale;

	Vector3	vMin;

	switch (m_eDir)
	{
	case BD_LEFT:
		vScale.x *= m_tCBuffer.fPercent;
		m_pTransform->SetWorldScale(vScale);
		break;
	case BD_RIGHT:
		vScale.x *= m_tCBuffer.fPercent;
		vMin = vScale - m_vScale;
		m_pTransform->SetWorldScale(vScale);
		m_pCollider->SetInfo(vMin, vScale);
		break;
	case BD_UP:
		vScale.y *= m_tCBuffer.fPercent;
		vMin = vScale - m_vScale;
		m_pTransform->SetWorldScale(vScale);
		m_pCollider->SetInfo(vMin, vScale);
		break;
	case BD_DOWN:
		vScale.y *= m_tCBuffer.fPercent;
		m_pTransform->SetWorldScale(vScale);
		break;
	}

	return 0;
}

int CUIBar::LateUpdate(float fTime)
{
	return 0;
}

void CUIBar::Collision(float fTime)
{
}

void CUIBar::Render(float fTime)
{
	CRenderer*	pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);

	pRenderer->UpdateRendererCBuffer("Bar", &m_tCBuffer, sizeof(m_tCBuffer));

	SAFE_RELEASE(pRenderer);
}

void CUIBar::AfterClone()
{
	m_pCollider = (CColliderRect*)FindComponentFromType<CCollider>(CT_COLLIDER);
}

CUIBar* CUIBar::Clone()
{
	return new CUIBar(*this);
}