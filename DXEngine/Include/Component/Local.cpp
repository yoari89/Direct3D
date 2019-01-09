#include "PCHEngine.h"
#include "Transform.h"

ENGINE_USING

void CTransform::SetLocalScale(const Vector3& _vecScale)
{
	// Vector3 타입을 이용한 크기 행렬 계산
	m_vecLocalScale = _vecScale;
	m_matLocalScale.Scaling(m_vecLocalScale);
	m_isUpdate = true;
}

void CTransform::SetLocalScale(float _x, float _y, float _z)
{
	// x, y, z 값을 이용한 크기 행렬 계산
	m_vecLocalScale = Vector3(_x, _y, _z);
	m_matLocalScale.Scaling(m_vecLocalScale);
	m_isUpdate = true;
}

void CTransform::SetLocalRot(const Vector3& _vecRot)
{
	// Vector3 타입을 이용한 회전(자전) 행렬 계산
	m_vecLocalRot = _vecRot;
	m_matLocalRot.Rotation(m_vecLocalRot);
	m_isUpdate = true;

	// 회전하면 각 축이 달라진다. 
	// 최종 회전 행렬(m_matLocalRot) 값을 이용하여 정규화된 축 계산
	ComputeLocalAxis();
}

void CTransform::SetLocalRot(float _x, float _y, float _z)
{
	// x, y, z 값을 이용한 회전 행렬 계산
	m_vecLocalRot = Vector3(_x, _y, _z);
	m_matLocalRot.Rotation(m_vecLocalRot);
	m_isUpdate = true;
	ComputeLocalAxis();
}

void CTransform::SetLocalRotX(float _x)
{
	// x축 회전
	m_vecLocalRot.x = _x;
	m_matLocalRot.Rotation(m_vecLocalRot);
	m_isUpdate = true;
	ComputeLocalAxis();
}

void CTransform::SetLocalRotY(float _y)
{
	// y축 회전
	m_vecLocalRot.y = _y;
	m_matLocalRot.Rotation(m_vecLocalRot);
	m_isUpdate = true;
	ComputeLocalAxis();
}

void CTransform::SetLocalRotZ(float _z)
{
	// z축 회전
	m_vecLocalRot.z = _z;
	m_matLocalRot.Rotation(m_vecLocalRot);
	m_isUpdate = true;
	ComputeLocalAxis();
}

void CTransform::SetLocalPos(const Vector3& _vecPos)
{
	// Vector3 타입을 이용한 이동 행렬 계산
	m_vecLocalPos = _vecPos;
	m_matLocalPos.Translation(m_vecLocalPos);
	m_isUpdate = true;
}

void CTransform::SetLocalPos(float _x, float _y, float _z)
{
	// x, y, z 값을 이용한 이동 행렬 계산
	m_vecLocalPos = Vector3(_x, _y, _z);
	m_matLocalPos.Translation(m_vecLocalPos);
	m_isUpdate = true;
	ComputeLocalAxis();
}

void CTransform::ComputeLocalAxis()
{
	// 자신의 로컬 회전 행렬을 만들때 자신의 부모 오브젝트가 존재한다면?
	// 자신의 로컬 회전 행렬과 부모의 로컬 회전 행렬을 곱한다.
	Matrix matRot = m_matLocalRot;
	if (m_pParentTransform != nullptr)
	{
		matRot = matRot* m_pParentTransform->GetLocalRotMatrix();
	}

	// 기저 축 계산
	for (int i = 0; i < AXIS_END; ++i)
	{
		// XMVector3TransformNormal
		// MSDN : 지정된 행렬로 3D 벡터 법선을 변형합니다.
		m_vecLocalAxis[i] = Vector3::Axis[i].TransformNormal(matRot);
		m_vecLocalAxis[i].Normalize();
	}
}

Vector3 CTransform::GetLocalScale() const
{
	return m_vecLocalScale;
}

Vector3 CTransform::GetLocalRot() const
{
	return m_vecLocalRot;
}

Vector3 CTransform::GetLocalPos() const
{
	return m_vecLocalPos;
}

const Vector3* CTransform::GetLocalAxis() const
{
	return m_vecLocalAxis;
}

Vector3 CTransform::GetLocalAxis(AXIS _eAxis) const
{
	return m_vecLocalAxis[_eAxis];
}

Matrix CTransform::GetLocalRotMatrix() const
{
	return m_matLocalRot;
}

Matrix CTransform::GetLocalMatrix() const
{
	return m_matLocal;
}