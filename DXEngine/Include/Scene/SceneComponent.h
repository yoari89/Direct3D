#pragma once
#include "../ReferenceCnt.h"

ENGINE_BEGIN
class ENGINE_DLL CSceneComponent : public CReferenceCnt
{
private:
	friend class CScene;

protected:
	CSceneComponent();
	virtual ~CSceneComponent() = 0; // ��� �뵵

protected:
	CScene* m_pScene;

// [[������ �Լ� ���]]
public:
	virtual void Start();
	virtual bool Init() = 0;					
	virtual int  Input(float _fTime);
	virtual int  Update(float _fTime);
	virtual int  LateUpdate(float _fTime);
	virtual void Collision(float _fTime);
	virtual void Render(float _fTime);
};
ENGINE_END