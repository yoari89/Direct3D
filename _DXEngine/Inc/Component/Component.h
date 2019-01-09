#pragma once#
#include "Utility/EnumFlag.h"
#include "../ReferenceCnt.h"

ENGINE_BEGIN
class CScene;
class CLayer;
class CGameObject;
class CTransform;
class ENGINE_DLL CComponent : public CReferenceCnt
{
private:
	friend CGameObject;

protected:
	CComponent();
	CComponent(const CComponent& _com);
	virtual ~CComponent() = 0; 

/* 컴포넌트 상속 사용가능 멤버 */
protected:
	CScene*			m_pScene;		
	CLayer*			m_pLayer;		
	CGameObject*	m_pObject;		
	CTransform*		m_pTransform;	

public:
	CScene*			GetScene()		const;
	CLayer*			GetLayer()		const;
	CGameObject*	GetGameObject() const;
	CTransform*		GetTransform()	const;

protected:
	COMPONENT_TYPE	m_eComType;

public:
	void SetComponentType(COMPONENT_TYPE _eType);
	COMPONENT_TYPE	GetComponentType() const;

/* 컴포넌트 추가, 찾기 기능 추가(편의) */ 
public:
	const list<CComponent*>* FindComponentsFromTag(const string& _strTag);
	const list<CComponent*>* FindComponentsFromType(COMPONENT_TYPE _eType);
	bool CheckComponentFromType(COMPONENT_TYPE eType);

	template <typename T>
	T* AddComponent(const string& _strTag)
	{
		return m_pObject->AddComponent<T>(_strTag);
	}

	template <typename T>
	T* FindComponentFromTag(const string& _strTag)
	{
		return m_pObject->FindComponentFromTag<T>(_strTag);
	}

	template <typename T>
	T* FindComponentFromType(COMPONENT_TYPE _eType)
	{
		return m_pObject->FindComponentFromType<T>(_eType);
	}


/* 재정의 (순수)가상 함수 */
public:
	virtual void Start();
	virtual bool Init() = 0;
	virtual int  Input(float _fTime);
	virtual int  Update(float _fTime);
	virtual int  LateUpdate(float _fTime);
	virtual void Collision(float _fTime);
	virtual void Render(float _fTime);
	virtual void AfterClone();
	virtual CComponent* Clone() = 0;
};
ENGINE_END