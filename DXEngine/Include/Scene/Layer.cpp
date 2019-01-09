#include "PCHEngine.h"
#include "Layer.h"
#include "Scene.h"
#include "Object/GameObject.h"

ENGINE_USING

CLayer::CLayer() :
	m_iZorder(-1),
	m_pScene(nullptr)
{
}

CLayer::~CLayer()
{
	// 레이어 소멸시 관리 오브젝트 목록 메모리 해제
	Safe_Release_VecList<list<CGameObject*>>(m_objList);
}

void CLayer::Start()
{
	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_objList.end();
	for (iter = m_objList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Start();
	}
}

bool CLayer::Init()
{
	return true;
}

int CLayer::Input(float _fTime)
{
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_objList.end();
	for (iter = m_objList.begin(); iter != iterEnd;)
	{
		// 오브젝트가 파괴된 상태
		if (!(*iter)->GetActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_objList.erase(iter);
			continue;
		}

		// 오브젝트가 비활성화 상태
		else if (!(*iter)->GetEnable())
		{
			++iter;
			continue;
		}

		// 오브젝트 Input() 호출 
		(*iter)->Input(_fTime);
		++iter;
	}
	return 0;
}

int CLayer::Update(float _fTime)
{
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_objList.end();
	for (iter = m_objList.begin(); iter != iterEnd;)
	{
		// 오브젝트가 파괴된 상태
		if (!(*iter)->GetActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_objList.erase(iter);
			continue;
		}

		// 오브젝트가 비활성화 상태
		else if (!(*iter)->GetEnable())
		{
			++iter;
			continue;
		}

		// 오브젝트 Update() 호출 
		(*iter)->Update(_fTime);
		++iter;
	}
	return 0;
}

int CLayer::LateUpdate(float _fTime)
{
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_objList.end();
	for (iter = m_objList.begin(); iter != iterEnd;)
	{
		// 오브젝트가 파괴된 상태
		if (!(*iter)->GetActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_objList.erase(iter);
			continue;
		}

		// 오브젝트가 비활성화 상태
		else if (!(*iter)->GetEnable())
		{
			++iter;
			continue;
		}

		// 오브젝트 LateUpdate() 호출 
		(*iter)->LateUpdate(_fTime);
		++iter;
	}
	return 0;
}

void CLayer::Collision(float _fTime)
{
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_objList.end();
	for (iter = m_objList.begin(); iter != iterEnd;)
	{
		// 오브젝트가 파괴된 상태
		if (!(*iter)->GetActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_objList.erase(iter);
			continue;
		}

		// 오브젝트가 비활성화 상태
		else if (!(*iter)->GetEnable())
		{
			++iter;
			continue;
		}

		// 오브젝트 Collision() 호출 
		(*iter)->Collision(_fTime);
		++iter;
	}
}

void CLayer::Render(float _fTime)
{
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_objList.end();
	for (iter = m_objList.begin(); iter != iterEnd;)
	{
		// 오브젝트가 파괴된 상태
		if (!(*iter)->GetActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_objList.erase(iter);
			continue;
		}

		// 오브젝트가 비활성화 상태
		else if (!(*iter)->GetEnable())
		{
			++iter;
			continue;
		}

		// 오브젝트 Render() 호출 
		(*iter)->Render(_fTime);
		++iter;
	}
}

void CLayer::SetZOrder(int _iZorder)
{
	m_iZorder = _iZorder;
	m_pScene->SortLayer();
}

int CLayer::GetZOrder() const
{
	return m_iZorder;
}

CScene* CLayer::GetScene() const
{
	m_pScene->AddRef();
	return m_pScene;
}

void CLayer::AddObject(CGameObject* _pObj)
{
	// 자신을 가지고 있는 레이어가 가지고 있는 씬 등록	↓
	// 자신을 가지고 있는 레이어 등록					↓
	_pObj->SetScene(m_pScene);
	_pObj->SetLayer(this);
	_pObj->AddRef();
	m_objList.push_back(_pObj);
}

void CLayer::EraseObject(const string& _strTag)
{
	// 오브젝트 삭제
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_objList.end();
	for (iter = m_objList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == _strTag)
		{
			m_objList.erase(iter);
			break;
		}
	}
}

void CLayer::SetObjLastZOrder(const string& _strTag)
{
	// 찾기
	CGameObject* pObj = FindObject(_strTag);

	// 삭제
	EraseObject(_strTag);

	// 추가
	AddObject(pObj);
	SAFE_RELEASE(pObj);
}

CGameObject* CLayer::FindObject(const string& _strTag)
{
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_objList.end();
	for (iter = m_objList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == _strTag)
		{
			(*iter)->AddRef();
			return (*iter);
		}
	}
	return nullptr;
}