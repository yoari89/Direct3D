#pragma once
#include "ReferenceCnt.h"
#include "Utility/EnumFlag.h"

ENGINE_BEGIN
class CScene;
class CLayer;
class CComponent;
class CTransform;
class ENGINE_DLL CGameObject : public CReferenceCnt
{
private:
	CGameObject();
	CGameObject(const CGameObject& _obj);
	~CGameObject();

// 씬
private:
	CScene* m_pScene; 

public:
	void	SetScene(CScene* _scene);
	CScene* GetScene() const;
	void	DontDestroyOnLoad();

// 레이어
private:
	CLayer* m_pLayer; 
	string	m_strLayerName;
	int		m_iLayerZOrder;

public:
	void	SetLayer(CLayer* _layer);
	CLayer* GetLayer()			const;
	string	GetLayerName()		const;
	int		GetLayerZOrder()	const;

// 부모, 자식 오브젝트
private:
	CGameObject*		m_pParent;
	list<CGameObject*>	m_childList;

public:
	// > 자식 오브젝트 추가
	// > 호출자(부모가 될 오브젝트), 인자(자식 오브젝트)
	void AddChild(CGameObject* _pChild); 
























// 오브젝트
private:
	// > 씬 별 프로토타입 오브젝트 관리
	static unordered_map<CScene*, unordered_map<string, CGameObject*>> m_mapPrototype;

private:
	// > 프로토타입 오브젝트 찾기
	static CGameObject* FindPrototype(CScene* _pScene, const string& _strName);

public:
	// > 찾기
	static CGameObject* FindObject(const string& _strTag);

	// > 오브젝트, 프로토타입 오브젝트, 복제 오브젝트 생성
	CGameObject*		Clone();
	static CGameObject* CreateObject(const string& _strTag = "GameObject", CLayer* _pLayer = nullptr, bool _isDontDestroy = false);
	static CGameObject* CreatePrototype(const string& _strTag, bool _isCurrent = true);
	static CGameObject* CreateClone(const string& _strTag, const string& _strProto, CLayer* _pLayer = nullptr, bool _isCurrent = true);

	// > 프로토타입 오브젝트 삭제
	static void DestroyPrototype();
	static void DestroyPrototype(CScene* _pScene);
	static void DestroyPrototype(CScene* _pScene, const string& _strName);

// [[컴포넌트]]
private:
	list<CComponent*>	m_comList;		// 관리컴포넌트
	list<CComponent*>	m_findList;		// 찾기 컴포넌트
	CTransform*			m_pTransform;
	

public:
	void AfterClone();

public:
	// > 추가
	CComponent* AddComponent(CComponent* _pCom);

	// > 찾기
	CTransform* GetTransform() const;
	const list<CComponent*>* FindComponentsFromTag(const string& _strTag);
	const list<CComponent*>* FindComponentsFromType(COMPONENT_TYPE _eType);
	const list<CComponent*>* GetComponentList() const;

	// 비교
	bool CheckComponentFromType(COMPONENT_TYPE _eType);
	bool EmptyComponent()	const;
	
public:
	// > 템플릿 타입 추가
	template<typename T>
	T* AddComponent(const string& _strTag)
	{
		T* pCom = new T();
		pCom->SetTag(_strTag);
		pCom->m_pScene		= m_pScene;
		pCom->m_pLayer		= m_pLayer;
		pCom->m_pTransform  = m_pTransform;
		pCom->m_pObject		= this;

		if (pCom->Init() == false)
		{
			SAFE_RELEASE(pCom);
			return nullptr;
		}

		// 초기화에서 실수로 속성이 변경될 수 있기 때문에 한번 더 호출
		return (T*)AddComponent(pCom);
	}

	// > 템플릿 타입 찾기
	template <typename T>
	T* FindComponentFromTag(const string& strTag)
	{
		// Tag 
		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_comList.end();
		for (iter = m_comList.begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->GetTag() == strTag)
			{
				(*iter)->AddRef();
				return (T*)*iter;
			}
		}
		return nullptr;
	}

	template <typename T>
	T* FindComponentFromType(COMPONENT_TYPE eType)
	{
		// Type
		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_comList.end();
		for (iter = m_comList.begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->GetComponentType() == eType)
			{
				(*iter)->AddRef();
				return (T*)*iter;
			}
		}
		return nullptr;
	}

// [[Game Logic]]
public:
	void Start();
	bool Init();
	int	 Input(float _fTime);
	int	 Update(float _fTime);
	int	 LateUpdate(float _fTime);
	void Collision(float _fTime);
	void Render(float _fTime);
};
ENGINE_END