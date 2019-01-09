#pragma once
#include "Collider.h"

ENGINE_BEGIN
class ENGINE_DLL CColliderCircle : public CCollider
{
private:
	friend class CGameObject;

protected:
	CColliderCircle();
	CColliderCircle(const CColliderCircle& _com);
	virtual ~CColliderCircle();

private:
	CircleInfo m_tInfo;
	CircleInfo m_tRelativeInfo;

public:
	void SetInfo(const Vector3& _vCenter, float _fRadius);
	CircleInfo GetInfo() const;

/* 실제 충돌 */
public:
	bool Collision(CCollider* _pDest, float _fTime) override;

/* 상속 받은 재정의 함수 */
public:
	bool Init()						override;
	int  Input(float fTime)			override;
	int  Update(float fTime)		override;
	int  LateUpdate(float fTime)	override;
	void Collision(float fTime)		override;
	void Render(float fTime)		override;
	CColliderCircle* Clone()		override;
};
ENGINE_END