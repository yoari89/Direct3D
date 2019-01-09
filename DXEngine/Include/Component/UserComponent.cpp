#include "PCHEngine.h"
#include "UserComponent.h"

ENGINE_USING

CUserComponent::CUserComponent()
{
}

CUserComponent::CUserComponent(const CUserComponent& _com) :
	CComponent(_com)
{
}

CUserComponent::~CUserComponent()
{
}

bool CUserComponent::Init()
{
	return true;
}

int CUserComponent::Input(float _fTime)
{
	return 0;
}

int CUserComponent::Update(float _fTime)
{
	return 0;
}

int CUserComponent::LateUpdate(float _fTime)
{
	return 0;
}

void CUserComponent::Collision(float _fTime)
{
}

void CUserComponent::Render(float _fTime)
{
}

void CUserComponent::AfterClone()
{
}