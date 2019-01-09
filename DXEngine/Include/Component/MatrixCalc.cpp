#include "PCHEngine.h"
#include "Transform.h"
#include "Object/GameObject.h"

ENGINE_USING

Matrix CTransform::GetParentMatrix() const
{
	return m_matParent;
}

Matrix CTransform::GetParentMatrixFromNoScale() const
{
	if (m_pParentTransform == nullptr)
	{
		return m_matWorldRot * m_matWorldPos;
	}

	return m_matWorldRot * m_matWorldPos * m_pParentTransform->GetParentMatrixFromNoScale();
}

Matrix CTransform::GetParentRotMatrix() const
{
	if (m_pParentTransform == nullptr)
	{
		return m_matWorldRot;
	}

	return m_matWorldRot * m_pParentTransform->GetParentRotMatrix();
}

Matrix CTransform::GetParentPosMatrix() const
{
	if (m_pParentTransform == nullptr)
	{
		return m_matWorldPos;
	}

	return m_matWorldPos * m_pParentTransform->GetParentPosMatrix();
}

void CTransform::Move(AXIS _eAxis, float _fSpeed)
{
	Move(Vector3(m_vecWorldAxis[_eAxis] * _fSpeed));
}

void CTransform::Move(AXIS _eAxis, float _fSpeed, float _fTime)
{
	Move(Vector3(m_vecWorldAxis[_eAxis] * _fSpeed * _fTime));
}

void CTransform::Move(const Vector3& _vecDir, float _fSpeed)
{
	Move(Vector3(_vecDir * _fSpeed));
}

void CTransform::Move(const Vector3& _vecDir, float _fSpeed, float _fTime)
{
	Move(Vector3(_vecDir * _fSpeed * _fTime));
}

void CTransform::Move(const Vector3& _vecMove)
{
	m_vWorldRelativePos += _vecMove;
	m_vecWorldPos += _vecMove;
	m_vecWorldMove = _vecMove;
	Check_Rot_Pos_Parent();
}

void CTransform::RotationX(float _x)
{
	Rotation(Vector3(_x, 0.0f, 0.0f));
}

void CTransform::RotationX(float _x, float _fTime)
{
	Rotation(Vector3(_x * _fTime, 0.0f, 0.0f));
}

void CTransform::RotationY(float _y)
{
	Rotation(Vector3(0.0f, _y, 0.0f));
}

void CTransform::RotationY(float _y, float _fTime)
{
	Rotation(Vector3(0.0f, _y * _fTime, 0.0f));
}

void CTransform::RotationZ(float _z)
{
	Rotation(Vector3(0.0f, 0.0f, _z));
}

void CTransform::RotationZ(float _z, float _fTime)
{
	Rotation(Vector3(0.0f, 0.0f, _z * _fTime));
}

void CTransform::Rotation(const Vector3& _vecRot, float _fTime)
{
	Rotation(_vecRot * _fTime);
}

void CTransform::Rotation(const Vector3& _vecRot)
{
	m_vecWorldRot += _vecRot;
	m_matWorldRot.Rotation(m_vecWorldRot);
	ComputeWorldAxis();
	Check_Rot_Pos_Parent();
}

void CTransform::LookAt(CGameObject* _pObj, AXIS _eAxis)
{
	CTransform* ptr = _pObj->GetTransform();
	LookAt(ptr->GetWorldPos(), _eAxis);
	SAFE_RELEASE(ptr);
}

void CTransform::LookAt(CComponent* _pCom, AXIS _eAxis)
{
	CTransform* ptr = _pCom->GetTransform();
	LookAt(ptr->GetWorldPos(), _eAxis);
	SAFE_RELEASE(ptr);
}

void CTransform::LookAt(const Vector3& _vPos, AXIS _eAxis)
{
	// 바라보는 방향
	Vector3 vView = _vPos - m_vecWorldPos;
	vView.Normalize();					   // 0.? 0.? 0
	Vector3 vAxis = Vector3::Axis[_eAxis]; // 0 1 0

										   // 각도
	float fAngle = vAxis.Angle(vView);
	Vector3	vRotAxis = vAxis.Cross(vView);
	vRotAxis.Normalize();
	if (vRotAxis == Vector3::Zero)
	{
		return;
	}

	m_matWorldRot.RotationAxis(fAngle, vRotAxis);
	ComputeWorldAxis();
	UpdateTransform();
}

void CTransform::UpdateTransform()
{
	m_isUpdate = true;

	if (!m_childTransformList.empty())
	{
		list<CTransform*>::iterator	iter;
		list<CTransform*>::iterator	iterEnd = m_childTransformList.end();

		for (iter = m_childTransformList.begin(); iter != iterEnd; ++iter)
		{
			(*iter)->UpdateTransform();
		}
	}
}

void CTransform::CheckScaleParent()
{
	// 부모 Transform가 없다면 상대적 크기는 자신의 월드 크기가 된다.
	if (m_pParentTransform == nullptr)
	{
		m_vWorldRelativeScale = m_vecWorldScale;
	}
	// 부모의 크기 영향 받지 않는다면 상대적 크기는 자신의 월드 크기가 된다.
	else if ((m_iParentFlag & TPF_SCALE) == 0)
	{
		m_vWorldRelativeScale = m_vecWorldScale / m_pParentTransform->m_vecWorldScale;
	}
	// 부모의 크기에 영향을 받는다.
	// > 사용시에는 월드 크기 지정처럼 사용하지만, 실제 계산은 부모 크기에 비례하여 계산되야한다.
	else
	{
		m_vWorldRelativeScale = m_vecWorldScale / m_pParentTransform->m_vecWorldScale;
	}

	// m_vecWorldScale 값을 이용하여 크기 행렬 계산
	m_matWorldScale.Scaling(m_vecWorldScale);

	// 자식 Transform가 있다면?
	// > 자식 Transform의 크기 값을 부모 크기에 따라서 크기 값을 계산한다.
	if (!m_childTransformList.empty())
	{
		list<CTransform*>::iterator	iter;
		list<CTransform*>::iterator	iterEnd = m_childTransformList.end();

		for (iter = m_childTransformList.begin(); iter != iterEnd; ++iter)
		{
			(*iter)->CheckScaleParent();
		}
	}

	m_isUpdate = true;
}

void CTransform::Check_Rot_Pos_Parent()
{
	// 부모 Transform가 없다면 상대적 위치는 자신의 월드 위치가 된다.
	if (m_pParentTransform == nullptr)
	{
		m_vWorldRelativePos = m_vecWorldPos;
	}
	// 부모의 위치의 영향 받지 않는다면 상대적 위치는 자신의 월드 위치가 된다.
	else if ((m_iParentFlag & TPF_POS) == 0)
	{
		m_vWorldRelativePos = m_vecWorldPos;
	}

	// m_vWorldRelativePos 값을 이용하여 위치 행렬 계산
	m_matWorldPos.Translation(m_vWorldRelativePos);

	// 부모 오브젝트가 있다면?
	// 부모에게서 조건에 따른 행렬 값을 가지고 m_vWorldRelativePos와 행렬 곱을하여 위치 벡터를 구한다.
	// > (즉, 부모에게서 영향을 받는 상태에 따라서 부모 행렬을 만들고, 부모 행렬값을 이용하여 
	//        자신의 월드 위치 값을 계산한다.)
	if (m_pParentTransform)
	{
		// 부모 회전시, 자식이 부모의 회전 상태에 영향을 받는다면 당연히 위치가 변경된다. (공전)
		Matrix	matParent;
		if (m_iParentFlag & TPF_POS && m_iParentFlag & TPF_ROT)
		{
			// 크기 제외
			matParent = m_pParentTransform->GetParentMatrixFromNoScale();
		}
		else if (m_iParentFlag & TPF_POS)
		{
			// 위치
			matParent = m_pParentTransform->GetParentPosMatrix();
		}
		else if (m_iParentFlag & TPF_ROT)
		{
			// 크기
			matParent = m_pParentTransform->GetParentRotMatrix();
		}
		else
		{
			// 항등 행렬
			matParent.Identity();
		}

		// 최종 위치 값
		Vector3 vPos = m_vWorldRelativePos.TransformCoord(matParent);
		m_vecWorldMove = vPos - m_vecWorldPos;
		m_vecWorldPos = vPos;
	}

	// 자식 Transform가 있다면?
	// > 자식 Transform의 위치 값을 부모의 영향 여부에 따라서 위치 값을 계산한다.
	if (m_childTransformList.empty() == false)
	{
		list<CTransform*>::iterator	iter;
		list<CTransform*>::iterator	iterEnd = m_childTransformList.end();
		for (iter = m_childTransformList.begin(); iter != iterEnd; ++iter)
		{
			(*iter)->Check_Rot_Pos_Parent();
		}
	}

	m_isUpdate = true;
}