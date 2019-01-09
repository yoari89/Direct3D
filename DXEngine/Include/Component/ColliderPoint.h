#pragma once
#include "Collider.h"

ENGINE_BEGIN
class ENGINE_DLL CColliderPoint : public CCollider
{
private:
	friend class CGameObject;

protected:
	CColliderPoint();
	CColliderPoint(const CColliderPoint& _com);
	virtual ~CColliderPoint();

private:
	Vector3 m_vRelativePos;
	Vector3 m_vPos;

public:
	void SetInfo(const Vector3& _vRelativePos);
	Vector3 GetInfo() const;

/* 실제 충돌 */
public:
	bool Collision(CCollider* _pDest, float _fTime) override;

/* 상속 받은 재정의 함수 */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	CColliderPoint* Clone()			override;
};
ENGINE_END