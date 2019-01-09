#pragma once
#include "Component/Component.h"

ENGINE_BEGIN
class CGameObject;
class CTransform;
class CComponent;
class ENGINE_DLL ENGINE_ALIGN CCamera : public CComponent
{
private:
	friend CGameObject;

public:
	CCamera();
	CCamera(const CCamera& _camera);
	virtual ~CCamera();

/* new, delete 재정의 */
public:
	void* operator new(size_t _iSize);
	void  operator delete(void* _p);

/* 카메라 */
private:
	float  m_fWidth;		// 넒이
	float  m_fHeight;		// 높이
	float  m_fViewAngle;	// 시야각
	float  m_fNear;			// 카메라에서 가장 가까운 시야
	float  m_fFar;			// 카메라에서 가장 먼 시야
	Matrix m_matView;		// 뷰 행렬
	Matrix m_matProj;		// 투영 행렬
	CAMERA_TYPE m_eCameraType;
	CTransform*	m_pTarget;

public:
	void SetTarget(CGameObject* pTarget);
	void SetTarget(CComponent* pTarget);

public:
	Matrix GetViewMatrix()	const;
	Matrix GetProjMatrix()	const;
	void SetCameraType(CAMERA_TYPE _eType);
	void SetCameraInfo(CAMERA_TYPE _eType, float _fWidth, float _fHeight, 
					   float _fViewAngle, float _fNear, float _fFar);
	void SetWidth(float _fWidth);
	void SetHeight(float _fHeight);
	void SetViewAngle(float _fAngle);
	void SetNear(float _fNear);
	void SetFar(float _fFar);

/* 상속 받은 재정의 함수 */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	CCamera* Clone()				override;
};
ENGINE_END