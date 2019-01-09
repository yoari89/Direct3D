#pragma once
#include "Component/Component.h"

ENGINE_BEGIN
class ENGINE_DLL CUserComponent : public CComponent
{
private:
	friend CGameObject;

protected:
	CUserComponent();
	CUserComponent(const CUserComponent& com);
	virtual ~CUserComponent() = 0;

public:
	virtual bool Init();
	virtual int  Input(float _fTime);
	virtual int  Update(float _fTime);
	virtual int  LateUpdate(float _fTime);
	virtual void Collision(float _fTime);
	virtual void Render(float _fTime);
	virtual void AfterClone();
	virtual CUserComponent* Clone() = 0;
};
ENGINE_END