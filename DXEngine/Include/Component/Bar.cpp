#include "PCHEngine.h"
#include "Bar.h"
#include "Component/Renderer.h"
#include "Component/Material.h"
#include "Component/ColliderRect.h"
#include "Component/Animation2D.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"

ENGINE_USING

CBar::CBar() :
	m_pCollider(nullptr)
{
	SetTag("UIBar");
}

CBar::CBar(const CBar & bar) :
	CComponent(bar)
{
	m_eDir = bar.m_eDir;
	m_fValue = bar.m_fValue;
	m_fMinValue = bar.m_fMinValue;
	m_fMaxValue = bar.m_fMaxValue;
	m_tCBuffer = bar.m_tCBuffer;
	m_fValueLength = bar.m_fValueLength;
	m_vScale = bar.m_vScale;
}

CBar::~CBar()
{
	SAFE_RELEASE(m_pCollider);
}

void CBar::SetBarDir(BAR_DIR eDir)
{
	m_eDir = eDir;
	m_tCBuffer.iMoveDir = eDir;
	Vector3	vPos;

	switch (m_eDir)
	{
	case BD_LEFT:
		m_pTransform->SetWorldPivot(0.f, 0.f, 0.f);
		break;
	case BD_RIGHT:
		m_pTransform->SetWorldPivot(1.f, 0.f, 0.f);
		vPos.x = m_pTransform->GetWorldScale().x;
		m_pTransform->Move(vPos);
		break;
	case BD_UP:
		m_pTransform->SetWorldPivot(0.f, 1.f, 0.f);
		vPos.y = m_pTransform->GetWorldScale().y;
		m_pTransform->Move(vPos);
		break;
	case BD_DOWN:
		m_pTransform->SetWorldPivot(0.f, 0.f, 0.f);
		break;
	}
}

void CBar::SetValue(float fValue)
{
	m_fValue = fValue;

	if (m_fValue < m_fMinValue)
		m_fValue = m_fMinValue;

	else if (m_fValue > m_fMaxValue)
		m_fValue = m_fMaxValue;

	m_tCBuffer.fPercent = (m_fValue - m_fMinValue) / m_fValueLength;
}

void CBar::AddValue(float fValue)
{
	m_fValue += fValue;

	if (m_fValue < m_fMinValue)
		m_fValue = m_fMinValue;

	else if (m_fValue > m_fMaxValue)
		m_fValue = m_fMaxValue;

	m_tCBuffer.fPercent = (m_fValue - m_fMinValue) / m_fValueLength;
}

void CBar::SetMinMaxValue(float fMin, float fMax)
{
	m_fMinValue = fMin;
	m_fMaxValue = fMax;

	if (m_fValue < m_fMinValue)
		m_fValue = m_fMinValue;

	else if (m_fValue > m_fMaxValue)
		m_fValue = m_fMaxValue;

	m_fValueLength = m_fMaxValue - m_fMinValue;

	m_tCBuffer.fPercent = (m_fValue - m_fMinValue) / m_fValueLength;
}

void CBar::SetScale(const Vector3 & vScale)
{
	m_vScale = vScale;

	m_pTransform->SetWorldScale(m_vScale);
}

void CBar::SetScale(float x, float y, float z)
{
	m_vScale = Vector3(x, y, z);

	m_pTransform->SetWorldScale(m_vScale);
}

void CBar::LightOn()
{
	m_tCBuffer.vLight = Vector4::White * 2.f;
}

void CBar::LightOff()
{
	m_tCBuffer.vLight = Vector4::White;
}

void CBar::AfterClone()
{
	m_pCollider = (CColliderRect*)FindComponentFromType<CCollider>(CT_COLLIDER);
}

bool CBar::Init()
{
	m_fMinValue = 0.f;
	m_fMaxValue = 100.f;
	m_fValue = 100.f;
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

	pRC->SetCollisionGroup("BackGround");
	pRC->SetInfo(Vector3(0.f, 0.f, 0.f), Vector3(200.f, 30.f, 0.f));
	pRC->SetCollisionCallback(CCT_ENTER, this, &CBar::Hit);
	pRC->SetCollisionCallback(CCT_LEAVE, this, &CBar::MouseOut);

	SetBarDir(BD_LEFT);

	return true;
}

int CBar::Input(float fTime)
{
	return 0;
}

int CBar::Update(float fTime)
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

int CBar::LateUpdate(float fTime)
{
	return 0;
}

void CBar::Collision(float fTime)
{
}

void CBar::Render(float fTime)
{
	CRenderer*	pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);

	pRenderer->UpdateRendererCBuffer("Bar", &m_tCBuffer, sizeof(m_tCBuffer));

	SAFE_RELEASE(pRenderer);
}

CBar * CBar::Clone()
{
	return new CBar(*this);
}

void CBar::Hit(CCollider * pSrc, CCollider * pDest, float fTime)
{
}

void CBar::MouseOut(CCollider * pSrc, CCollider * pDest, float fTime)
{
}
