#include "PCHEngine.h"
#include "UIButton.h"
#include "Component/Transform.h"
#include "Component/Renderer.h"
#include "Component/Material.h"
#include "Component/ColliderRect.h"
#include "Utility/Input/Input.h"
#include "Object/GameObject.h"
#include "Resources/Texture.h"

ENGINE_USING

CUIButton::CUIButton()
{
	SetTag("UIButton");
	m_eUIType = UT_BUTTON;

	// ��ư ���� ���� ��
	m_vBSColor[BS_NORMAL]	= Vector4(0.9f, 0.9f, 0.9f, 1.0f);
	m_vBSColor[BS_MOUSEON]	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_vBSColor[BS_CLICK]	= Vector4(0.7f, 0.7f, 0.7f, 1.0f);
	m_vBSColor[BS_DISABLE]	= Vector4(0.5f, 0.5f, 0.5f, 1.0f);
}

CUIButton::CUIButton(const CUIButton& _button) :
	CUI(_button)
{
	m_eState = BS_NORMAL;
	memcpy(m_vBSColor, _button.m_vBSColor, sizeof(Vector4) * BS_END);
}

CUIButton::~CUIButton()
{
}

/* ������Ʈ Ŭ���� �����Լ� ������ */

bool CUIButton::Init()
{
	// �⺻������ �����Ǵ� ��ư ����
	CreateDefaultButton();
	
	return true;
}

int CUIButton::Input(float _fTime)
{
	bool isBtnState = (m_eState == BS_MOUSEON || m_eState == BS_CLICK);
	if (isBtnState == true)
	{
		if (KEYPUSH("LButton"))
		{
			m_eState = BS_CLICK;
		}
		else if (KEYUP("LButton"))
		{
			// �ݹ� �Լ� ȣ��
			m_clickCallBack(_fTime);
		}
	}

	return 0;
}

int CUIButton::Update(float _fTime)
{
	return 0;
}

int CUIButton::LateUpdate(float _fTime)
{
	return 0;
}

void CUIButton::Collision(float _fTime)
{
}

void CUIButton::Render(float _fTime)
{
	// ��ư ��� ���� ��� �� ����
	m_tCBuffer.vColor = m_vBSColor[m_eState];

	// ��ư ��� ���� ������Ʈ
	CRenderer* pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);
	pRenderer->UpdateRendererCBuffer("Button", &m_tCBuffer, sizeof(m_tCBuffer));
	SAFE_RELEASE(pRenderer);
}

void CUIButton::AfterClone()
{
	CColliderRect* pColRect = (CColliderRect*)FindComponentFromTag<CCollider>("ButtonCollider");
	pColRect->SetCollisionCallback(CCT_ENTER, this, &CUIButton::MouseOn);
	SAFE_RELEASE(pColRect);
}

CUIButton* CUIButton::Clone()
{
	return new CUIButton(*this);
}

/* ��ư ���� */

void CUIButton::Enable()
{
	m_eState = BS_NORMAL;
}

void CUIButton::Disable()
{
	m_eState = BS_DISABLE;
}

void CUIButton::CreateDefaultButton()
{
	m_eState = BS_NORMAL;
	
	// Renderer
	CRenderer* pRenderer = m_pObject->AddComponent<CRenderer>("ButtonRenderer");
	pRenderer->SetMesh("TexRect");
	pRenderer->SetShader(BUTTON_SHADER);
	pRenderer->SetRenderState(ALPHA_BLEND);
	pRenderer->CreateRendererCBuffer("Button", sizeof(ButtonCBuffer));
	SAFE_RELEASE(pRenderer);

	// Material 
	CMaterial* pMaterial = m_pObject->FindComponentFromType<CMaterial>(CT_MATERIAL);
	CTexture*  pBtnTex	 = GET_SINGLE(CResourcesManager)->FindTexture("DefaultButton");
	int texWidth  = pBtnTex->GetWidth();
	int texHeight = pBtnTex->GetHeight();
	pMaterial->SetDiffuseTex(0, pBtnTex);
	SAFE_RELEASE(pBtnTex);
	SAFE_RELEASE(pMaterial);

	// Transform
	Vector3 vPos	= Vector3(300.f, 300.f, 1.0f);
	Vector3 vScale	= { texWidth, texHeight, 0 };
	m_pTransform->SetWorldPos(vPos);
	m_pTransform->SetWorldScale(vScale);

	// Collider Rect
	CColliderRect* pColRect = AddComponent<CColliderRect>("ButtonCollider");
	pColRect->SetCollisionGroup("UI");
	pColRect->SetInfo(Vector3::Zero, Vector3(texWidth, texHeight, 0));
	// > Collider CallBack Type - Enter -> MouseOn �Լ� ȣ��
	//                          - Leave -> MouseLeave �Լ� ȣ��
	pColRect->SetCollisionCallback(CCT_ENTER, this, &CUIButton::MouseOn);
	pColRect->SetCollisionCallback(CCT_LEAVE, this, &CUIButton::MouseLeave);
	SAFE_RELEASE(pColRect);
}

void CUIButton::SetStateColor(BUTTON_STATE _eState, const Vector4& _vColor)
{
	m_vBSColor[_eState] = _vColor;
}

void CUIButton::SetStateColor(BUTTON_STATE _eState, unsigned char _r, unsigned char _g, 
							  unsigned char _b, unsigned char _a)
{
	m_vBSColor[_eState] = Vector4(_r, _g, _b, _a);
}

BUTTON_STATE CUIButton::GetState()
{
	return m_eState;
}

/* Collision */

void CUIButton::MouseOn(CCollider* _pSrc, CCollider* _pDest, float _fTime)
{
	if (_pDest->GetTag() == "MouseWindow")
	{
		m_eState = BS_MOUSEON;
	}
}

void CUIButton::MouseLeave(CCollider* _pSrc, CCollider* _pDest, float _fTime)
{
	if (_pDest->GetTag() == "MouseWindow")
	{
		m_eState = BS_NORMAL;
	}
}

/* CallBack */

void CUIButton::SetCallBack(void(*_pFunc)(float))
{
	m_clickCallBack = bind(_pFunc, placeholders::_1);
}