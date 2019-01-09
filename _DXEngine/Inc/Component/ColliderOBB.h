#pragma once
#include "Collider.h"

ENGINE_BEGIN
class ENGINE_DLL CColliderOBB : public CCollider
{
private:
	friend class CGameObject;

protected:
	CColliderOBB();
	CColliderOBB(const CColliderOBB& _obb);
	~CColliderOBB();

private:
	OBB2DInfo m_tRelativeInfo;
	OBB2DInfo m_tInfo;

public:
	void SetInfo(const Vector3& _vCetner, Vector3 _vAxis[2], float _fLength[2]);
	OBB2DInfo GetInfo() const;

public:
	virtual bool Collision(CCollider* _pDest, float _fTime);

public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	CColliderOBB* Clone()			override;
};
ENGINE_END