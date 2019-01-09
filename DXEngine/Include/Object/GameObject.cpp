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
	// 태그명
	SetTag("GameObject");
}

CGameObject::CGameObject(const CGameObject& _obj)
{
 	*this		= _obj;
	m_iRefCount = 1;
	m_findList.clear();

	// 복제
	// > 프로토타입 오브젝트가 Clone()을 호출하여 자신을 복제할 때 
	//   *this(프토로타입 객체)를 넘겨받으며 프로토타입이 가지고 정보를 복제하는 작업

	// Transform Copy
	// > 프로토타입이 가지고 있는 Transform 정보를 복제하여 새로 만들어질 오브젝트에게 대입
	m_pTransform = _obj.m_pTransform->Clone();
	m_pTransform->m_pTransform = m_pTransform;

	// Component Copy
	//  > 위 Transform 컴포넌트 작업과 동일
 	m_comList.clear();
	list<CComponent*>::const_iterator iter;
	list<CComponent*>::const_iterator iterEnd = _obj.m_comList.end();
	for (iter = _obj.m_comList.begin(); iter != iterEnd; ++iter)
	{
		CComponent*	pCom	= (*iter)->Clone();
		pCom->m_pObject		= this;			// 복제된 오브젝트가 자신을 가진다.
		pCom->m_pTransform	= m_pTransform;	// 복제된 오브젝트의 Transform으로 변경한다.
		m_comList.push_back(pCom);			// 프로토타입이 가지고 있던 컴포넌트를 복사한다.
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
	// 오브젝트 소멸
	// > 동적 할당된 Transform 해제
	// > 컴포넌트 목록 메모리 해제
	Safe_Release_VecList<list<CComponent*>>(m_findList);
	SAFE_RELEASE(m_pTransform);
	Safe_Release_VecList<list<CComponent*>>(m_comList);
	Safe_Release_VecList(m_childList);
}

// ========== 씬 ==========

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

// ========== 레이어 ==========

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

// ========== 오브젝트 ==========








// ========== 부모, 자식 오브젝트 ==========

void CGameObject::AddChild(CGameObject* _pChild)
{
	// this		: 부모 오브젝트
	// _pChild	: 자식 오브젝트

	_pChild->m_pParent = this;										
	_pChild->m_pTransform->m_pParentTransform = m_pTransform;

	// 예) 자식 오브젝트가 지워져도, 부모에서 자식을 관리함에 있어서 지워진 메모리를
	//     참조하여 에러가 발생할 수 있는 상황을 방지한다.
	//     Transform 객체도 위와 동일하다.
	_pChild->AddRef();
	_pChild->m_pTransform->AddRef();								 

	// 자식 Transform 객체가 부모 Transform 객체의 어떤 상태의 영향을 받을것인가? (초기화)
	_pChild->m_pTransform->SetParentFlag(TPF_ROT | TPF_POS);		

	// 부모 오브젝트, 부모 오브젝트의 Transform 에서 자식의 오브젝트 및 Transform 컴포넌트를 관리한다.
	m_pTransform->m_childTransformList.push_back(_pChild->m_pTransform);
	m_childList.push_back(_pChild);									

	// 현재 레이어에 오브젝트 추가
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
	// 씬에서 프로토타입 오브젝트 찾기
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
	// DontDestroy 오브젝트라면?
	CGameObject* pObj = GET_SINGLE(CObjectManager)->FindDontDestroyObj(_strTag);
	if (pObj != nullptr)
	{
		return pObj;
	}

	// 생성
	pObj = new CGameObject;
	pObj->SetTag(_strTag);
	if (_isDontDestroy == true)
	{
		pObj->DontDestroyOnLoad();
	}

	// 초기화 실패
	if (pObj->Init() == false)
	{
		SAFE_RELEASE(pObj);
		return nullptr;
	}

	if (_pLayer != nullptr)
	{
		if (_isDontDestroy == false)
		{
			// 삭제되는 오브젝트
			// > 레이어에 만들어진 오브젝트 추가
			_pLayer->AddObject(pObj);
		}
		else
		{
			// 삭제되지 않는 오브젝트
			// > 오브젝트에 새로운 씬, 레이어를 등록
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
	// 프로토타입 생성
	// 씬 설정
 	CScene*	pScene = nullptr;
	if (_isCurrent == true)
	{
		pScene = GET_SINGLE(CSceneManager)->GetScene();
	} 
	else
	{
		pScene = GET_SINGLE(CSceneManager)->GetNextScene();
	}

	// 생성된 프로토타입이 있는지 검사
	CGameObject* pPrototype = FindPrototype(pScene, _strTag);
	if (pPrototype != nullptr)
	{
		return nullptr;
	}

	// 찾기
	unordered_map<CScene*, unordered_map<string, CGameObject*>>::iterator 
		iter = m_mapPrototype.find(pScene);
	
	// Scene* 키 값으로 없다면 생성
	unordered_map<string, CGameObject*>* pMap = nullptr;
	if (iter == m_mapPrototype.end())
	{
		unordered_map<string, CGameObject*>	map;
		m_mapPrototype.insert(make_pair(pScene, map));
		iter = m_mapPrototype.find(pScene);
	}
	pMap = &iter->second;
	SAFE_RELEASE(pScene);

	// 프로토타입 생성
	pPrototype = new CGameObject();
	pPrototype->SetTag(_strTag);
	if (!pPrototype->Init())
	{
		SAFE_RELEASE(pPrototype);
		return nullptr;
	}
	
	// 추가
	pPrototype->AddRef();
	pMap->insert(make_pair(_strTag, pPrototype));
	
	return pPrototype;
}

CGameObject* CGameObject::CreateClone(const string& _strTag, const string& _strProto, CLayer* _pLayer, bool _isCurrent)
{
	// 복제 오브젝트 생성
	CScene*	pScene = nullptr;
	if (_isCurrent == true)
	{
		pScene = GET_SINGLE(CSceneManager)->GetScene();
	}
	else
	{
		pScene = GET_SINGLE(CSceneManager)->GetNextScene();
	}

	// 복제 대상인 프로토타입 찾기
	CGameObject* pProto = FindPrototype(pScene, _strProto);
	SAFE_RELEASE(pScene);
	if (pProto == nullptr)
	{
		return nullptr;
	}

	// 찾은 프로토타입을 이용하여 복제 오브젝트 생성
	// 레이어에 복제된 오브젝트 추가
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
	// 프로토타입(호출자, this)를 이용하여 오브젝트 복제
	// Unity3D의 프리팹과 비슷한 용도
	return new CGameObject(*this);
}

void CGameObject::DestroyPrototype()
{
	// 프로토타입 오브젝트 전부 삭제
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
	// 특정 씬에 존재하는 프로토타입 삭제
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
	// 특정 씬에 존재하는 특정 프로토타입 삭제
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
	// 컴포넌트 추가
	_pCom->m_pScene		= m_pScene;		// 레이어를 가지고 있는 씬		↑
	_pCom->m_pLayer		= m_pLayer;		// 오브젝트를 가지고 있는 레이어	↑
	_pCom->m_pTransform = m_pTransform;
	_pCom->m_pObject	= this;			// 자신을 가지고 있는 오브젝트		↑
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
	// 같은 타입의 컴포넌트를 찾기(태그명)
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
	// 같은 타입의 컴포넌트를 찾기(타입)
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
	// 오브젝트 생성시 Transfom 컴포넌트 필수
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
		// 파괴
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
		// 비활성화
		else if ((*iter)->GetEnable() == false)
		{
			++iter;
			continue;
		}
		// Component 객체 Input() 호출 
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
	// 충돌관리자에 오브젝트를 등록한다.
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