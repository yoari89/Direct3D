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

/* new, delete ������ */
public:
	void* operator new(size_t _iSize);
	void  operator delete(void* _p);

/* ī�޶� */
private:
	float  m_fWidth;		// ����
	float  m_fHeight;		// ����
	float  m_fViewAngle;	// �þ߰�
	float  m_fNear;			// ī�޶󿡼� ���� ����� �þ�
	float  m_fFar;			// ī�޶󿡼� ���� �� �þ�
	Matrix m_matView;		// �� ���
	Matrix m_matProj;		// ���� ���
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

/* ��� ���� ������ �Լ� */
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