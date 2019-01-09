#include "PCHEngine.h"
#include "ObjectManager.h"
#include "ObjectManager.h"
#include "Scene/Scene.h"
#include "Scene/Layer.h"
#include "GameObject.h"

ENGINE_USING
DEFINITION_SINGLE(CObjectManager)

CObjectManager::CObjectManager()
{
}

CObjectManager::~CObjectManager()
{
	Safe_Release_VecList(m_DontDestroyObj);
}

void CObjectManager::AddDontDestroyObj(CGameObject * pObj)
{
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_DontDestroyObj.end();
	for (iter = m_DontDestroyObj.begin(); iter != iterEnd; ++iter)
	{
		if (pObj == *iter)
		{
			return;
		}
	}
	pObj->AddRef();
	m_DontDestroyObj.push_back(pObj);
}

void CObjectManager::ChangeScene(CScene* pScene)
{
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_DontDestroyObj.end();
	for (iter = m_DontDestroyObj.begin(); iter != iterEnd; ++iter)
	{
		CLayer*	pLayer = pScene->FindLayer((*iter)->GetLayerName());
		if (pLayer == nullptr)
		{
			pScene->AddLayer((*iter)->GetLayerName(), (*iter)->GetLayerZOrder());
			pLayer = pScene->FindLayer((*iter)->GetLayerName());
		}
		// 레이어에 오브젝트가 없다면 등록
		CGameObject* pObj = pLayer->FindObject((*iter)->GetTag());
		if (pObj == nullptr)
		{
			pLayer->AddObject(*iter);
		}
		SAFE_RELEASE(pObj);
		SAFE_RELEASE(pLayer);
	}
}

bool CObjectManager::CheckDontDestroyObj(const string & strTag)
{
	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_DontDestroyObj.end();

	for (iter = m_DontDestroyObj.begin(); iter != iterEnd; ++iter)
	{
		if (strTag == (*iter)->GetTag())
			return true;
	}

	return false;
}

CGameObject* CObjectManager::FindDontDestroyObj(const string & strTag)
{
	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_DontDestroyObj.end();

	for (iter = m_DontDestroyObj.begin(); iter != iterEnd; ++iter)
	{
		if (strTag == (*iter)->GetTag())
		{
			(*iter)->AddRef();
			return *iter;
		}
	}

	return nullptr;
}

bool CObjectManager::Init()
{
	return true;
}
