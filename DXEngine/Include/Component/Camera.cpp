#include "PCHEngine.h"
#include "Camera.h"
#include "Transform.h"
#include "Object/GameObject.h"

ENGINE_USING

CCamera::CCamera()
{
	m_eComType = CT_CAMERA;
	m_eCameraType = CT_PERSPECTIVE;
	m_pTarget = nullptr;
}

CCamera::CCamera(const CCamera& _camera) :
	CComponent(_camera)
{
	m_eCameraType = _camera.m_eCameraType;
	m_matView = _camera.m_matView;
	m_matProj = _camera.m_matProj;
	m_pTarget = nullptr;
}

CCamera::~CCamera()
{
	SAFE_RELEASE(m_pTarget);
}

void* CCamera::operator new(size_t _iSize)
{
	return _aligned_malloc(_iSize, 16);
}

void CCamera::operator delete(void* _p)
{
	_aligned_free(_p);
}

void CCamera::SetTarget(CGameObject * pTarget)
{
	m_pTarget = pTarget->GetTransform();
}

void CCamera::SetTarget(CComponent * pTarget)
{
	m_pTarget = pTarget->GetTransform();
}

Matrix CCamera::GetViewMatrix() const
{
	return m_matView;
}

Matrix CCamera::GetProjMatrix() const
{
	return m_matProj;
}

void CCamera::SetCameraType(CAMERA_TYPE _eType)
{
	m_eCameraType = _eType;
	switch (_eType)
	{
		// 원근
		case CT_PERSPECTIVE:
		{
			m_matProj = XMMatrixPerspectiveFovLH(DegreeToRadian(m_fViewAngle),
												 m_fWidth / m_fHeight, m_fNear, m_fFar);
			break;
		}
		// 직교
		case CT_ORTHO:
		{
			m_matProj = XMMatrixOrthographicOffCenterLH(0.f, m_fWidth,
														0.f, m_fHeight, m_fNear, m_fFar);
			break;
		}
	}
}

void CCamera::SetCameraInfo(CAMERA_TYPE _eType, float _fWidth, float _fHeight, float _fViewAngle, float _fNear, float _fFar)
{
	m_fWidth	 = _fWidth;
	m_fHeight	 = _fHeight;
	m_fViewAngle = _fViewAngle;
	m_fNear		 = _fNear;
	m_fFar		 = _fFar;
	SetCameraType(_eType);
}

void CCamera::SetWidth(float _fWidth)
{
	m_fWidth = _fWidth;
	SetCameraType(m_eCameraType);
}

void CCamera::SetHeight(float _fHeight)
{
	m_fHeight = _fHeight;
	SetCameraType(m_eCameraType);
}

void CCamera::SetViewAngle(float _fAngle)
{
	m_fViewAngle = _fAngle;
	SetCameraType(m_eCameraType);
}

void CCamera::SetNear(float _fNear)
{
	m_fNear = _fNear;
	SetCameraType(m_eCameraType);
}

void CCamera::SetFar(float _fFar)
{
	m_fFar = _fFar;
	SetCameraType(m_eCameraType);
}

bool CCamera::Init()
{
	return true;
}

int CCamera::Input(float _fTime)
{
	return 0;
}

int CCamera::Update(float _fTime)
{
	m_matView.Identity();

	if (m_pTarget)
	{
		Vector3	vMove = m_pTarget->GetWorldMove();

		if (vMove != Vector3::Zero)
		{
			m_pTransform->Move(vMove);
		}
	}


	Vector3	vPos = m_pTransform->GetWorldPos();
	vPos *= -1.f;
	memcpy(&m_matView[3][0], &vPos, sizeof(Vector3));
	return 0;
}

int CCamera::LateUpdate(float _fTime)
{
	return 0;
}

void CCamera::Collision(float _fTime)
{
}

void CCamera::Render(float _fTime)
{
}

CCamera* CCamera::Clone()
{
	return new CCamera(*this);
}