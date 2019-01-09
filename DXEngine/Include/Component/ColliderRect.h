#pragma once
#include "Collider.h"

ENGINE_BEGIN
class ENGINE_DLL CColliderRect : public CCollider
{
private:
	friend class CGameObject;

protected:
	CColliderRect();
	CColliderRect(const CColliderRect& _com);
	virtual ~CColliderRect();

private:
	BoxInfo	m_tInfo;		 // 크기
	BoxInfo	m_tRelativeInfo; // 상대적 크기

public:
	void SetInfo(const Vector3& _vMin, const Vector3& _vMax);
	BoxInfo GetInfo() const;

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
	CColliderRect* Clone()			override;
};
ENGINE_END