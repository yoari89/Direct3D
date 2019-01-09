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

// ��
private:
	CScene* m_pScene; 

public:
	void	SetScene(CScene* _scene);
	CScene* GetScene() const;
	void	DontDestroyOnLoad();

// ���̾�
private:
	CLayer* m_pLayer; 
	string	m_strLayerName;
	int		m_iLayerZOrder;

public:
	void	SetLayer(CLayer* _layer);
	CLayer* GetLayer()			const;
	string	GetLayerName()		const;
	int		GetLayerZOrder()	const;

// �θ�, �ڽ� ������Ʈ
private:
	CGameObject*		m_pParent;
	list<CGameObject*>	m_childList;

public:
	// > �ڽ� ������Ʈ �߰�
	// > ȣ����(�θ� �� ������Ʈ), ����(�ڽ� ������Ʈ)
	void AddChild(CGameObject* _pChild); 
























// ������Ʈ
private:
	// > �� �� ������Ÿ�� ������Ʈ ����
	static unordered_map<CScene*, unordered_map<string, CGameObject*>> m_mapPrototype;

private:
	// > ������Ÿ�� ������Ʈ ã��
	static CGameObject* FindPrototype(CScene* _pScene, const string& _strName);

public:
	// > ã��
	static CGameObject* FindObject(const string& _strTag);

	// > ������Ʈ, ������Ÿ�� ������Ʈ, ���� ������Ʈ ����
	CGameObject*		Clone();
	static CGameObject* CreateObject(const string& _strTag = "GameObject", CLayer* _pLayer = nullptr, bool _isDontDestroy = false);
	static CGameObject* CreatePrototype(const string& _strTag, bool _isCurrent = true);
	static CGameObject* CreateClone(const string& _strTag, const string& _strProto, CLayer* _pLayer = nullptr, bool _isCurrent = true);

	// > ������Ÿ�� ������Ʈ ����
	static void DestroyPrototype();
	static void DestroyPrototype(CScene* _pScene);
	static void DestroyPrototype(CScene* _pScene, const string& _strName);

// [[������Ʈ]]
private:
	list<CComponent*>	m_comList;		// ����������Ʈ
	list<CComponent*>	m_findList;		// ã�� ������Ʈ
	CTransform*			m_pTransform;
	

public:
	void AfterClone();

public:
	// > �߰�
	CComponent* AddComponent(CComponent* _pCom);

	// > ã��
	CTransform* GetTransform() const;
	const list<CComponent*>* FindComponentsFromTag(const string& _strTag);
	const list<CComponent*>* FindComponentsFromType(COMPONENT_TYPE _eType);
	const list<CComponent*>* GetComponentList() const;

	// ��
	bool CheckComponentFromType(COMPONENT_TYPE _eType);
	bool EmptyComponent()	const;
	
public:
	// > ���ø� Ÿ�� �߰�
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

		// �ʱ�ȭ���� �Ǽ��� �Ӽ��� ����� �� �ֱ� ������ �ѹ� �� ȣ��
		return (T*)AddComponent(pCom);
	}

	// > ���ø� Ÿ�� ã��
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