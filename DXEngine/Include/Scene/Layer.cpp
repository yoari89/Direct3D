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
	// ���̾� �Ҹ�� ���� ������Ʈ ��� �޸� ����
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
		// ������Ʈ�� �ı��� ����
		if (!(*iter)->GetActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_objList.erase(iter);
			continue;
		}

		// ������Ʈ�� ��Ȱ��ȭ ����
		else if (!(*iter)->GetEnable())
		{
			++iter;
			continue;
		}

		// ������Ʈ Input() ȣ�� 
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
		// ������Ʈ�� �ı��� ����
		if (!(*iter)->GetActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_objList.erase(iter);
			continue;
		}

		// ������Ʈ�� ��Ȱ��ȭ ����
		else if (!(*iter)->GetEnable())
		{
			++iter;
			continue;
		}

		// ������Ʈ Update() ȣ�� 
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
		// ������Ʈ�� �ı��� ����
		if (!(*iter)->GetActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_objList.erase(iter);
			continue;
		}

		// ������Ʈ�� ��Ȱ��ȭ ����
		else if (!(*iter)->GetEnable())
		{
			++iter;
			continue;
		}

		// ������Ʈ LateUpdate() ȣ�� 
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
		// ������Ʈ�� �ı��� ����
		if (!(*iter)->GetActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_objList.erase(iter);
			continue;
		}

		// ������Ʈ�� ��Ȱ��ȭ ����
		else if (!(*iter)->GetEnable())
		{
			++iter;
			continue;
		}

		// ������Ʈ Collision() ȣ�� 
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
		// ������Ʈ�� �ı��� ����
		if (!(*iter)->GetActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_objList.erase(iter);
			continue;
		}

		// ������Ʈ�� ��Ȱ��ȭ ����
		else if (!(*iter)->GetEnable())
		{
			++iter;
			continue;
		}

		// ������Ʈ Render() ȣ�� 
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
	// �ڽ��� ������ �ִ� ���̾ ������ �ִ� �� ���	��
	// �ڽ��� ������ �ִ� ���̾� ���					��
	_pObj->SetScene(m_pScene);
	_pObj->SetLayer(this);
	_pObj->AddRef();
	m_objList.push_back(_pObj);
}

void CLayer::EraseObject(const string& _strTag)
{
	// ������Ʈ ����
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
	// ã��
	CGameObject* pObj = FindObject(_strTag);

	// ����
	EraseObject(_strTag);

	// �߰�
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