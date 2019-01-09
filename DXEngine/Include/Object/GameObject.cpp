#include "PCHEngine.h"
#include "GameObject.h"
#include "Scene/Scene.h"
#include "Scene/Layer.h"
#include "Component/Transform.h"
#include "Scene/SceneManager.h"
#include "Component/Renderer.h"
#include "Utility/Collision/CollisionManager.h"
#include "ObjectManager.h"

ENGINE_USING

unordered_map<CScene*, unordered_map<string, CGameObject*>> CGameObject::m_mapPrototype;

CGameObject::CGameObject() :
	m_pScene(nullptr),
	m_pLayer(nullptr),
	m_pTransform(nullptr),
	m_pParent(nullptr)
{
	// �±׸�
	SetTag("GameObject");
}

CGameObject::CGameObject(const CGameObject& _obj)
{
 	*this		= _obj;
	m_iRefCount = 1;
	m_findList.clear();

	// ����
	// > ������Ÿ�� ������Ʈ�� Clone()�� ȣ���Ͽ� �ڽ��� ������ �� 
	//   *this(�����Ÿ�� ��ü)�� �Ѱܹ����� ������Ÿ���� ������ ������ �����ϴ� �۾�

	// Transform Copy
	// > ������Ÿ���� ������ �ִ� Transform ������ �����Ͽ� ���� ������� ������Ʈ���� ����
	m_pTransform = _obj.m_pTransform->Clone();
	m_pTransform->m_pTransform = m_pTransform;

	// Component Copy
	//  > �� Transform ������Ʈ �۾��� ����
 	m_comList.clear();
	list<CComponent*>::const_iterator iter;
	list<CComponent*>::const_iterator iterEnd = _obj.m_comList.end();
	for (iter = _obj.m_comList.begin(); iter != iterEnd; ++iter)
	{
		CComponent*	pCom	= (*iter)->Clone();
		pCom->m_pObject		= this;			// ������ ������Ʈ�� �ڽ��� ������.
		pCom->m_pTransform	= m_pTransform;	// ������ ������Ʈ�� Transform���� �����Ѵ�.
		m_comList.push_back(pCom);			// ������Ÿ���� ������ �ִ� ������Ʈ�� �����Ѵ�.
	}

	CRenderer* pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);
	if (pRenderer)
	{
		pRenderer->CheckComponent();
		// pRenderer->SetMaterial
		SAFE_RELEASE(pRenderer);
	}

	m_childList.clear();
	list<CGameObject*>::const_iterator	iter1;
	list<CGameObject*>::const_iterator	iter1End = _obj.m_childList.end();
	m_pTransform->m_childTransformList.clear();

	for (iter1 = _obj.m_childList.begin(); iter1 != iter1End; ++iter1)
	{
		CGameObject* pChild = (*iter1)->Clone();
		pChild->m_pParent = this;
		pChild->m_pTransform->m_pParentTransform = m_pTransform;
		pChild->m_pTransform->SetParentFlag(TPF_ROT | TPF_POS);
		pChild->m_pTransform->AddRef();
		m_pTransform->m_childTransformList.push_back(pChild->m_pTransform);

		m_childList.push_back(pChild);
	}
}

CGameObject::~CGameObject()
{
	// ������Ʈ �Ҹ�
	// > ���� �Ҵ�� Transform ����
	// > ������Ʈ ��� �޸� ����
	Safe_Release_VecList<list<CComponent*>>(m_findList);
	SAFE_RELEASE(m_pTransform);
	Safe_Release_VecList<list<CComponent*>>(m_comList);
	Safe_Release_VecList(m_childList);
}

// ========== �� ==========

void CGameObject::SetScene(CScene* _scene)
{
	m_pScene = _scene;
	m_pTransform->m_pScene = _scene;

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_comList.end();

	for (iter = m_comList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->m_pScene = _scene;
	}
}

CScene* CGameObject::GetScene() const
{
	return m_pScene;
}

void CGameObject::DontDestroyOnLoad()
{
	GET_SINGLE(CObjectManager)->AddDontDestroyObj(this);
}

// ========== ���̾� ==========

void CGameObject::SetLayer(CLayer* _layer)
{
	m_pLayer		= _layer;
	m_strLayerName	= _layer->GetTag();
	m_iLayerZOrder	= _layer->GetZOrder();

	m_pTransform->m_pLayer = _layer;

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_comList.end();
	for (iter = m_comList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->m_pLayer = _layer;
	}
}

CLayer* CGameObject::GetLayer() const
{
	return m_pLayer;
}

string CGameObject::GetLayerName() const
{
	return m_strLayerName;
}

int CGameObject::GetLayerZOrder() const
{
	return m_iLayerZOrder;
}

// ========== ������Ʈ ==========








// ========== �θ�, �ڽ� ������Ʈ ==========

void CGameObject::AddChild(CGameObject* _pChild)
{
	// this		: �θ� ������Ʈ
	// _pChild	: �ڽ� ������Ʈ

	_pChild->m_pParent = this;										
	_pChild->m_pTransform->m_pParentTransform = m_pTransform;

	// ��) �ڽ� ������Ʈ�� ��������, �θ𿡼� �ڽ��� �����Կ� �־ ������ �޸𸮸�
	//     �����Ͽ� ������ �߻��� �� �ִ� ��Ȳ�� �����Ѵ�.
	//     Transform ��ü�� ���� �����ϴ�.
	_pChild->AddRef();
	_pChild->m_pTransform->AddRef();								 

	// �ڽ� Transform ��ü�� �θ� Transform ��ü�� � ������ ������ �������ΰ�? (�ʱ�ȭ)
	_pChild->m_pTransform->SetParentFlag(TPF_ROT | TPF_POS);		

	// �θ� ������Ʈ, �θ� ������Ʈ�� Transform ���� �ڽ��� ������Ʈ �� Transform ������Ʈ�� �����Ѵ�.
	m_pTransform->m_childTransformList.push_back(_pChild->m_pTransform);
	m_childList.push_back(_pChild);									

	// ���� ���̾ ������Ʈ �߰�
	if (_pChild->m_pLayer == nullptr)
	{
		m_pLayer->AddObject(_pChild);
	}
}






















void CGameObject::AfterClone()
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_comList.end();

	for (iter = m_comList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->AfterClone();
	}
}

CGameObject* CGameObject::FindPrototype(CScene* _pScene, const string& _strName)
{
	// ������ ������Ÿ�� ������Ʈ ã��
	unordered_map<CScene*, unordered_map<string, CGameObject*>>
		::iterator iter = m_mapPrototype.find(_pScene);
	if (iter == m_mapPrototype.end())
	{
		return nullptr;
	}

	// iter->second = map<string, CGameObject*>
	unordered_map<string, CGameObject*>::iterator Objiter = iter->second.find(_strName);
	if (Objiter == iter->second.end())
	{
		return nullptr;
	}
	
	return Objiter->second;
}

CGameObject* CGameObject::CreateObject(const string& _strTag, CLayer* _pLayer, bool _isDontDestroy)
{
	// DontDestroy ������Ʈ���?
	CGameObject* pObj = GET_SINGLE(CObjectManager)->FindDontDestroyObj(_strTag);
	if (pObj != nullptr)
	{
		return pObj;
	}

	// ����
	pObj = new CGameObject;
	pObj->SetTag(_strTag);
	if (_isDontDestroy == true)
	{
		pObj->DontDestroyOnLoad();
	}

	// �ʱ�ȭ ����
	if (pObj->Init() == false)
	{
		SAFE_RELEASE(pObj);
		return nullptr;
	}

	if (_pLayer != nullptr)
	{
		if (_isDontDestroy == false)
		{
			// �����Ǵ� ������Ʈ
			// > ���̾ ������� ������Ʈ �߰�
			_pLayer->AddObject(pObj);
		}
		else
		{
			// �������� �ʴ� ������Ʈ
			// > ������Ʈ�� ���ο� ��, ���̾ ���
			class CScene* pScene = _pLayer->GetScene();
			pObj->SetScene(pScene);
			pObj->SetLayer(_pLayer);
			if (_pLayer->FindObject(pObj->GetTag()) == nullptr)
			{
				_pLayer->AddObject(pObj);
			}
			SAFE_RELEASE(pScene);
		}
	}

	return pObj;
}

CGameObject* CGameObject::CreatePrototype(const string& _strTag, bool _isCurrent)
{
	// ������Ÿ�� ����
	// �� ����
 	CScene*	pScene = nullptr;
	if (_isCurrent == true)
	{
		pScene = GET_SINGLE(CSceneManager)->GetScene();
	} 
	else
	{
		pScene = GET_SINGLE(CSceneManager)->GetNextScene();
	}

	// ������ ������Ÿ���� �ִ��� �˻�
	CGameObject* pPrototype = FindPrototype(pScene, _strTag);
	if (pPrototype != nullptr)
	{
		return nullptr;
	}

	// ã��
	unordered_map<CScene*, unordered_map<string, CGameObject*>>::iterator 
		iter = m_mapPrototype.find(pScene);
	
	// Scene* Ű ������ ���ٸ� ����
	unordered_map<string, CGameObject*>* pMap = nullptr;
	if (iter == m_mapPrototype.end())
	{
		unordered_map<string, CGameObject*>	map;
		m_mapPrototype.insert(make_pair(pScene, map));
		iter = m_mapPrototype.find(pScene);
	}
	pMap = &iter->second;
	SAFE_RELEASE(pScene);

	// ������Ÿ�� ����
	pPrototype = new CGameObject();
	pPrototype->SetTag(_strTag);
	if (!pPrototype->Init())
	{
		SAFE_RELEASE(pPrototype);
		return nullptr;
	}
	
	// �߰�
	pPrototype->AddRef();
	pMap->insert(make_pair(_strTag, pPrototype));
	
	return pPrototype;
}

CGameObject* CGameObject::CreateClone(const string& _strTag, const string& _strProto, CLayer* _pLayer, bool _isCurrent)
{
	// ���� ������Ʈ ����
	CScene*	pScene = nullptr;
	if (_isCurrent == true)
	{
		pScene = GET_SINGLE(CSceneManager)->GetScene();
	}
	else
	{
		pScene = GET_SINGLE(CSceneManager)->GetNextScene();
	}

	// ���� ����� ������Ÿ�� ã��
	CGameObject* pProto = FindPrototype(pScene, _strProto);
	SAFE_RELEASE(pScene);
	if (pProto == nullptr)
	{
		return nullptr;
	}

	// ã�� ������Ÿ���� �̿��Ͽ� ���� ������Ʈ ����
	// ���̾ ������ ������Ʈ �߰�
	CGameObject* pObj = pProto->Clone();
	pObj->AfterClone();
	pObj->SetTag(_strTag);
	if (_pLayer != nullptr)
	{
		_pLayer->AddObject(pObj);
	}
	return pObj;
}

CGameObject* CGameObject::FindObject(const string& _strTag)
{
	return GET_SINGLE(CSceneManager)->FindObject(_strTag);
}

CGameObject* CGameObject::Clone()
{
	// ������Ÿ��(ȣ����, this)�� �̿��Ͽ� ������Ʈ ����
	// Unity3D�� �����հ� ����� �뵵
	return new CGameObject(*this);
}

void CGameObject::DestroyPrototype()
{
	// ������Ÿ�� ������Ʈ ���� ����
	unordered_map<CScene*, unordered_map<string, CGameObject*>>::iterator iter;
	unordered_map<CScene*, unordered_map<string, CGameObject*>>::iterator iterEnd = m_mapPrototype.end();
	for (iter = m_mapPrototype.begin(); iter != iterEnd; ++iter)
	{
		Safe_Release_Map(iter->second);
	}
	m_mapPrototype.clear();
}

void CGameObject::DestroyPrototype(CScene* _pScene)
{
	// Ư�� ���� �����ϴ� ������Ÿ�� ����
	unordered_map<CScene*, unordered_map<string, CGameObject*>>
		::iterator iter = m_mapPrototype.find(_pScene);
	if (iter == m_mapPrototype.end())
	{
		return;
	}
	Safe_Release_Map(iter->second);
	m_mapPrototype.erase(iter);
}

void CGameObject::DestroyPrototype(CScene* _pScene, const string& _strName)
{
	// Ư�� ���� �����ϴ� Ư�� ������Ÿ�� ����
	unordered_map<CScene*, unordered_map<string, CGameObject*>>
		::iterator iter = m_mapPrototype.find(_pScene);
	if (iter == m_mapPrototype.end())
	{
		return;
	}
	unordered_map<string, CGameObject*>::iterator ObjIter = iter->second.find(_strName);
	if (ObjIter == iter->second.end())
	{
		return;
	}
	SAFE_RELEASE(ObjIter->second);
	iter->second.erase(ObjIter);
}

CTransform* CGameObject::GetTransform() const
{
	m_pTransform->AddRef();
	return m_pTransform;
}

CComponent* CGameObject::AddComponent(CComponent* _pCom)
{
	// ������Ʈ �߰�
	_pCom->m_pScene		= m_pScene;		// ���̾ ������ �ִ� ��		��
	_pCom->m_pLayer		= m_pLayer;		// ������Ʈ�� ������ �ִ� ���̾�	��
	_pCom->m_pTransform = m_pTransform;
	_pCom->m_pObject	= this;			// �ڽ��� ������ �ִ� ������Ʈ		��
	_pCom->AddRef();					
	m_comList.push_back(_pCom);

	CRenderer* pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);
	if (pRenderer)
	{
		pRenderer->CheckComponent();
		SAFE_RELEASE(pRenderer);
	}
	
	return _pCom;
}

const list<CComponent*>* CGameObject::FindComponentsFromTag(const string& _strTag)
{
	// ���� Ÿ���� ������Ʈ�� ã��(�±׸�)
	Safe_Release_VecList(m_findList);
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_comList.end();
	for (iter = m_comList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == _strTag)
		{
			(*iter)->AddRef();
			m_findList.push_back(*iter);
		}
	}
	return &m_findList;
}

const list<CComponent*>* CGameObject::FindComponentsFromType(COMPONENT_TYPE _eType)
{
	// ���� Ÿ���� ������Ʈ�� ã��(Ÿ��)
	Safe_Release_VecList(m_findList);
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_comList.end();
	for (iter = m_comList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetComponentType() == _eType)
		{
			(*iter)->AddRef();
			m_findList.push_back(*iter);
		}
	}
	return &m_findList;
}

const list<CComponent*>* CGameObject::GetComponentList() const
{
	return &m_comList;
}

bool CGameObject::CheckComponentFromType(COMPONENT_TYPE _eType)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_comList.end();
	for (iter = m_comList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetComponentType() == _eType)
		{
			return true;
		}
	}
	return false;
}

bool CGameObject::EmptyComponent() const
{
	return m_comList.empty();
}

void CGameObject::Start()
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_comList.end();
	for (iter = m_comList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Start();
	}
}

bool CGameObject::Init()
{
	// ������Ʈ ������ Transfom ������Ʈ �ʼ�
	m_pTransform = new CTransform();
	m_pTransform->Init();
	m_pTransform->m_pTransform = m_pTransform;
	return true;
}

int CGameObject::Input(float _fTime)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_comList.end();
	for (iter = m_comList.begin(); iter != iterEnd;)
	{
		// �ı�
		if ((*iter)->GetActive() == false)
		{
			CRenderer*	pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);
			if (pRenderer)
			{
				pRenderer->DeleteComponentCBuffer(*iter);
				SAFE_RELEASE(pRenderer);
			}

			SAFE_RELEASE((*iter));
			iter = m_comList.erase(iter);
			continue;
		}
		// ��Ȱ��ȭ
		else if ((*iter)->GetEnable() == false)
		{
			++iter;
			continue;
		}
		// Component ��ü Input() ȣ�� 
		(*iter)->Input(_fTime);
		++iter;
	}

	// Transform Component Input
	m_pTransform->Input(_fTime);
	return 0;
}

int CGameObject::Update(float _fTime)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_comList.end();
	for (iter = m_comList.begin(); iter != iterEnd;)
	{
		if ((*iter)->GetActive() == false)
		{
			CRenderer*	pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);
			if (pRenderer)
			{
				pRenderer->DeleteComponentCBuffer(*iter);
				SAFE_RELEASE(pRenderer);
			}

			SAFE_RELEASE((*iter));
			iter = m_comList.erase(iter);
			continue;
		}
		else if ((*iter)->GetEnable() == false)
		{
			++iter;
			continue;
		}
		(*iter)->Update(_fTime);
		++iter;
	}

	m_pTransform->Update(_fTime);
	return 0;
}

int CGameObject::LateUpdate(float _fTime)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_comList.end();
	for (iter = m_comList.begin(); iter != iterEnd;)
	{
		if ((*iter)->GetActive() == false)
		{
			CRenderer*	pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);
			if (pRenderer)
			{
				pRenderer->DeleteComponentCBuffer(*iter);
				SAFE_RELEASE(pRenderer);
			}

			SAFE_RELEASE((*iter));
			iter = m_comList.erase(iter);
			continue;
		}
		else if ((*iter)->GetEnable() == false)
		{
			++iter;
			continue;
		}
		(*iter)->LateUpdate(_fTime);
		++iter;
	}

	m_pTransform->LateUpdate(_fTime);
	return 0;
}

void CGameObject::Collision(float _fTime)
{
	// �浹�����ڿ� ������Ʈ�� ����Ѵ�.
	GET_SINGLE(CCollisionManager)->AddCollision(this);
}

void CGameObject::Render(float _fTime)
{
	m_pTransform->Render(_fTime);
	CRenderer* pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);
	if (pRenderer)
	{
		if (!pRenderer->GetActive())
		{
			pRenderer->Release();
			m_comList.remove(pRenderer);
		}
		else if (pRenderer->GetEnable())
		{
			pRenderer->Render(_fTime);
		}
		SAFE_RELEASE(pRenderer);
	}

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_comList.end();
	for (iter = m_comList.begin(); iter != iterEnd;)
	{
		if ((*iter)->GetComponentType() == CT_RENDERER)
		{
			++iter;
			continue;
		}
		else if ((*iter)->GetActive() == false)
		{
			CRenderer*	pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);
			if (pRenderer)
			{
				pRenderer->DeleteComponentCBuffer(*iter);
				SAFE_RELEASE(pRenderer);
			}

			SAFE_RELEASE((*iter));
			iter = m_comList.erase(iter);
			continue;
		}
		else if ((*iter)->GetEnable() == false)
		{
			++iter;
			continue;
		}
		(*iter)->Render(_fTime);
		++iter;
	}
}