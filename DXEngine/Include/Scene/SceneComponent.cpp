#include "PCHEngine.h"
#include "SceneComponent.h"

ENGINE_USING

CSceneComponent::CSceneComponent() : 
	m_pScene(nullptr)
{
}

CSceneComponent::~CSceneComponent()
{
}

void CSceneComponent::Start()
{
}

int CSceneComponent::Input(float _fTime)
{
	return 0;
}

int CSceneComponent::Update(float _fTime)
{
	return 0;
}

int CSceneComponent::LateUpdate(float _fTime)
{
	return 0;
}

void CSceneComponent::Collision(float _fTime)
{
}

void CSceneComponent::Render(float _fTime)
{
}