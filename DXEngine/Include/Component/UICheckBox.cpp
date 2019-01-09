#include "PCHEngine.h"
#include "UICheckBox.h"
#include "Component/Renderer.h"
#include "Component/Material.h"
#include "Component/Transform.h"
#include "Component/ColliderRect.h"
#include "Utility/Input/Input.h"
#include "Object/GameObject.h"
#include "Resources/Texture.h"
#include "Component/UIRadioButton.h"
#include "Scene/SceneManager.h"

ENGINE_USING

CUICheckBox::CUICheckBox() :
	m_isActive(false)
{
	SetTag("UICheckBox");
	m_eUIType = UT_CHECKBOX;
}

CUICheckBox::CUICheckBox(const CUICheckBox& _checkBox) :
	CUIButton(_checkBox)
{
}

CUICheckBox::~CUICheckBox()
{
}

/* 컴포넌트 클래스 가상함수 재정의 */

bool CUICheckBox::Init()
{
	m_eState = BS_NORMAL;

	CTexture* pCheckBoxTex = GET_SINGLE(CResourcesManager)->FindTexture("DefaultCheckBox_N");
	int texWidth  = pCheckBoxTex->GetWidth();
	int texHeight = pCheckBoxTex->GetHeight();
	SAFE_RELEASE(pCheckBoxTex);

	// Transform
	m_pTransform->SetWorldScale((float)texWidth, (float)texHeight, 1.f);
	m_pTransform->SetWorldPivot(0.5f, 0.5f, 0.f);

	// Renderer
	CRenderer* pRenderer = m_pObject->AddComponent<CRenderer>("ButtonRenderer");
	pRenderer->SetMesh("TexRect");
	pRenderer->SetRenderState(ALPHA_BLEND);
	SAFE_RELEASE(pRenderer);

	// Material 
	CMaterial* pMaterial = m_pObject->FindComponentFromType<CMaterial>(CT_MATERIAL);
	pMaterial->SetDiffuseTex(0, "DefaultCheckBox_N", TEXT("DefaultCheckBox_N.png"));
	SAFE_RELEASE(pMaterial);

	// Collider
	CColliderRect* pRC = AddComponent<CColliderRect>("CheckBoxCollider");
	pRC->SetCollisionGroup("UI");
	pRC->SetInfo(Vector3(0.f, 0.f, 0.f), Vector3(texWidth, texHeight, 0));
	pRC->SetCollisionCallback(CCT_ENTER, this, &CUICheckBox::MouseOn);
	pRC->SetCollisionCallback(CCT_LEAVE, this, &CUICheckBox::MouseLeave);
	SAFE_RELEASE(pRC);

	return true;
}

int CUICheckBox::Input(float _fTime)
{
	bool isBtnState = (m_eState == BS_MOUSEON || m_eState == BS_CLICK);
	if (isBtnState)
	{
		if (KEYPUSH("LButton"))
		{
			m_eState = BS_CLICK;
		}
		else if (KEYUP("LButton"))
		{
			// 상태 변경
			// RadioButton CallBack
			CGameObject* pObjRadioBtn = GET_SINGLE(CSceneManager)->FindObject("RadioButton");
			if (pObjRadioBtn == nullptr)
			{
				return 0;
			}
			CUIRadioButton* pRadioBtn = pObjRadioBtn->FindComponentFromTag<CUIRadioButton>("C_RadioButton");
			pRadioBtn->CheckBoxStateInit("Default");
			SAFE_RELEASE(pRadioBtn);
			SAFE_RELEASE(pObjRadioBtn);

			m_clickCallBack(_fTime);
		}
	}

	return 0;
}

int CUICheckBox::Update(float _fTime)
{
	return 0;
}

int CUICheckBox::LateUpdate(float _fTime)
{
	return 0;
}

void CUICheckBox::Collision(float _fTime)
{
}

void CUICheckBox::Render(float _fTime)
{
}

void CUICheckBox::AfterClone()
{
}

CUICheckBox* CUICheckBox::Clone()
{
	return nullptr;
}

void CUICheckBox::SetActive(float _fTime)
{
	// Active 상태
	m_isActive = true;
	if (m_pObject == nullptr)
	{
		return;
	}
	CMaterial* pMaterial = m_pObject->FindComponentFromType<CMaterial>(CT_MATERIAL);
	pMaterial->SetDiffuseTex(0, "DefaultCheckBox_A", TEXT("DefaultCheckBox_A.png"));
	SAFE_RELEASE(pMaterial);
}

void CUICheckBox::MouseOn(CCollider* _pSrc, CCollider* _pDest, float _fTime)
{
	if (_pDest->GetTag() == "MouseWindow")
	{
		m_eState = BS_MOUSEON;
	}
}

void CUICheckBox::MouseLeave(CCollider* _pSrc, CCollider* _pDest, float _fTime)
{
	if (_pDest->GetTag() == "MouseWindow")
	{
		m_eState = BS_NORMAL;
	}
}