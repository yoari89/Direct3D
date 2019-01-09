#include "PCHEngine.h"
#include "Component.h"
#include "Component/Transform.h"
#include "../Scene/Scene.h"
#include "../Scene/Layer.h"
#include "Object/GameObject.h"

ENGINE_USING

CComponent::CComponent() :
	m_pScene(nullptr),
	m_pLayer(nullptr),
	m_pObject(nullptr),
	m_pTransform(nullptr)
{
}

CComponent::CComponent(const CComponent& _com)
{
	*this = _com;
	m_iRefCount = 1;
}

CComponent::~CComponent()
{
}

CScene* CComponent::GetScene() const
{
	return m_pScene;
}

CLayer* CComponent::GetLayer() const
{
	return m_pLayer;
}

CGameObject* CComponent::GetGameObject() const
{
	m_pObject->AddRef();
	return m_pObject;
}

CTransform* CComponent::GetTransform() const
{
	m_pTransform->AddRef();
	return m_pTransform;
}

void CComponent::SetComponentType(COMPONENT_TYPE _eType)
{
	m_eComType = _eType;
}

COMPONENT_TYPE CComponent::GetComponentType() const
{
	return m_eComType;
}

const list<CComponent*>* CComponent::FindComponentsFromTag(const string& _strTag)
{
	return m_pObject->FindComponentsFromTag(_strTag);
}

const list<CComponent*>* CComponent::FindComponentsFromType(COMPONENT_TYPE _eType)
{
	return m_pObject->FindComponentsFromType(_eType);
}

bool CComponent::CheckComponentFromType(COMPONENT_TYPE eType)
{
	return m_pObject->CheckComponentFromType(eType);
}

void CComponent::Start()
{
}

int CComponent::Input(float _fTime)
{
	return 0;
}

int CComponent::Update(float _fTime)
{
	return 0;
}

int CComponent::LateUpdate(float _fTime)
{
	return 0;
}

void CComponent::Collision(float _fTime)
{
}

void CComponent::Render(float _fTime)
{
}

void CComponent::AfterClone()
{
}