#include "PCHEngine.h"
#include "UIRadioButton.h"
#include "UICheckBox.h"
#include "Component/Transform.h"
#include "Component/Material.h"
#include "Object/GameObject.h"

ENGINE_USING

CUIRadioButton::CUIRadioButton()
{
}

CUIRadioButton::CUIRadioButton(const CUIRadioButton& _radioBtn) :
	CUIButton(_radioBtn)
{
}

CUIRadioButton::~CUIRadioButton()
{
	map<string, list<CGameObject*>>::iterator iter;
	map<string, list<CGameObject*>>::iterator iterEnd = m_mapGroupList.end();
	for (iter = m_mapGroupList.begin(); iter != iterEnd; ++iter)
	{
		Safe_Release_VecList(iter->second);
	}
	m_mapGroupList.clear();
}

bool CUIRadioButton::Init()
{
	list<CGameObject*> defaultList;
	m_mapGroupList.insert(make_pair("Default", defaultList));

	return true;
}

int CUIRadioButton::Input(float _fTime)
{
	return 0;
}

int CUIRadioButton::Update(float _fTime)
{
	


	return 0;
}

int CUIRadioButton::LateUpdate(float _fTime)
{
	return 0;
}

void CUIRadioButton::Collision(float _fTime)
{
}

void CUIRadioButton::Render(float _fTime)
{
}

void CUIRadioButton::AfterClone()
{
}

CUIRadioButton* CUIRadioButton::Clone()
{
	return nullptr;
}

list<CGameObject*>* CUIRadioButton::FindGroup(string _groupName)
{
	map<string, list<CGameObject*>>::iterator iter;
	map<string, list<CGameObject*>>::iterator iterEnd = m_mapGroupList.end();
	for (iter = m_mapGroupList.begin(); iter != iterEnd; ++iter)
	{
		if (iter->first == _groupName)
		{
			return &(iter->second);
		}
	}
	return nullptr;
}

void CUIRadioButton::AddObject(string _groupName, CGameObject* _pObj)
{
	map<string, list<CGameObject*>>::iterator findIter;
	findIter = m_mapGroupList.find(_groupName);
	if (findIter == m_mapGroupList.end())
	{
		return;
	}

	CUI* pUi = _pObj->FindComponentFromType<CUI>(CT_UI);
	if (pUi != nullptr && pUi->GetUIType() == UT_CHECKBOX)
	{
		// 좌표 설정(y축 정렬)
		if (findIter->second.empty() == false)
		{
			// 리스트에 존재하는 마지막 오브젝트
			CTransform* pLastObjTr	= findIter->second.back()->GetTransform();
			Vector3 vLastObjPos		= pLastObjTr->GetWorldPos();
			SAFE_RELEASE(pLastObjTr);

			// 추가될 오브젝트
			float distanceY		= 50.f;
			CTransform* pObjTr	= _pObj->GetTransform();
			Vector3 vPos		= pObjTr->GetWorldPos();
			vPos.y				= vLastObjPos.y + distanceY;
			pObjTr->SetWorldPos(vPos);
			SAFE_RELEASE(pObjTr);
		}

		// 추가
		findIter->second.push_back(_pObj);
	}
	SAFE_RELEASE(pUi);
}

void CUIRadioButton::CheckBoxStateInit(string _groupName)
{
	map<string, list<CGameObject*>>::iterator findIter = m_mapGroupList.find(_groupName);
	if (findIter == m_mapGroupList.end())
	{
		return;
	}
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = findIter->second.end();
	for (iter = findIter->second.begin(); iter != iterEnd; ++iter)
	{
		CGameObject* pObj = (*iter);
		CUICheckBox* pCheckBox	= pObj->FindComponentFromTag<CUICheckBox>("C_CheckBox");
		CMaterial* pMaterial	= pObj->FindComponentFromType<CMaterial>(CT_MATERIAL);
		pMaterial->SetDiffuseTex(0, "DefaultCheckBox_N", TEXT("DefaultCheckBox_N.png"));
		SAFE_RELEASE(pCheckBox);
		SAFE_RELEASE(pMaterial);
	}
}