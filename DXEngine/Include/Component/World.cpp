#include "PCHEngine.h"
#include "Transform.h"

ENGINE_USING

void CTransform::SetWorldScale(const Vector3& _vecScale)
{
	// Vector3 타입을 이용한 월드 크기 행렬 계산
	m_vecWorldScale = _vecScale;
	CheckScaleParent();
}

void CTransform::SetWorldScale(float _x, float _y, float _z)
{
	// x, y, z 값을 이용한 월드 크기 행렬 계산
	m_vecWorldScale = Vector3(_x, _y, _z);
	CheckScaleParent();
}

void CTransform::SetWorldRot(const Vector3& _vecRot)
{
	// Vector3 타입을 이용한 월드 회전 행렬 계산
	m_vecWorldRot = _vecRot;
	m_matWorldRot.Rotation(m_vecWorldRot);
	m_isUpdate = true;
	ComputeWorldAxis();
}

void CTransform::SetWorldRot(float _x, float _y, float _z)
{
	// x, y, z 값을 이용한 월드 크기 행렬 계산
	m_vecWorldRot = Vector3(_x, _y, _z);
	m_matWorldRot.Rotation(m_vecWorldRot);
	m_isUpdate = true;
	ComputeWorldAxis();
}

void CTransform::SetWorldRotX(float _x)
{
	// x축 월드 회전
	m_vecWorldRot.x = _x;
	m_matWorldRot.Rotation(m_vecWorldRot);
	m_isUpdate = true;
	ComputeWorldAxis();
}

void CTransform::SetWorldRotY(float _y)
{
	// y축 월드 회전
	m_vecWorldRot.y = _y;
	m_matWorldRot.Rotation(m_vecWorldRot);
	m_isUpdate = true;
	ComputeWorldAxis();
}

void CTransform::SetWorldRotZ(float _z)
{
	// z축 월드 회전
	m_vecWorldRot.z = _z;
	m_matWorldRot.Rotation(m_vecWorldRot);
	m_isUpdate = true;
	ComputeWorldAxis();
}

void CTransform::SetWorldPos(const Vector3& _vecPos)
{
	// vecMove : 이동할 벡터 - 현재 자신의 월드 위치
	m_vecWorldMove = _vecPos - m_vecWorldPos;
	m_vecWorldPos  = _vecPos;

	// 상대 객체에서의 상대적 월드 위치
	// > 계산된 vecMove 방향에서 상대적 월드 위치를 더한 위치
	m_vWorldRelativePos = m_vWorldRelativePos + m_vecWorldPos;

	// 
	Check_Rot_Pos_Parent();
}

void CTransform::SetWorldPos(float _x, float _y, float _z)
{
	Vector3	vPos(_x, _y, _z);
	m_vecWorldMove = vPos - m_vecWorldPos;
	m_vWorldRelativePos += m_vecWorldMove;

	m_vecWorldPos = vPos;
	Check_Rot_Pos_Parent();
}

void CTransform::SetWorldRelativePos(const Vector3& _vPos)
{
	m_vWorldRelativePos = _vPos;
	Check_Rot_Pos_Parent();
}

void CTransform::SetWorldRelativePos(float _x, float _y, float _z)
{
	m_vWorldRelativePos = Vector3(_x, _y, _z);
	Check_Rot_Pos_Parent();
}

void CTransform::SetWorldPivot(const Vector3& _vecPivot)
{
	m_vecPivot = _vecPivot;
}

void CTransform::SetWorldPivot(float _x, float _y, float _z)
{
	m_vecPivot = Vector3(_x, _y, _z);
}

void CTransform::ComputeWorldAxis()
{
	// 자신의 월드 회전 행렬을 만들때 자신의 부모 오브젝트가 존재한다면?
	// 자신의 월드 회전 행렬과 부모의 월드 회전 행렬을 곱한다.
	Matrix matRot = m_matWorldRot;
	if (m_pParentTransform != nullptr)
	{
		matRot = matRot * m_pParentTransform->GetWorldRotMatrix();
	}

	// 기저 축 계산
	for (int i = 0; i < AXIS_END; ++i)
	{
		m_vecWorldAxis[i] = Vector3::Axis[i].TransformNormal(matRot);
		m_vecWorldAxis[i].Normalize();
	}
}

Vector3 CTransform::GetWorldScale() const
{
	return m_vecWorldScale;
}

Vector3 CTransform::GetWorldRot() const
{
	return m_vecWorldRot;
}

Vector3 CTransform::GetWorldPos() const
{
	return m_vecWorldPos;
}

Vector3 CTransform::GetWorldMove() const
{
	return m_vecWorldMove;
}

Vector3 CTransform::GetPivot() const
{
	return m_vecPivot;
}

Vector3 CTransform::GetWorldAxis(AXIS _eAxis) const
{
	return m_vecWorldAxis[_eAxis];
}

const Vector3* CTransform::GetWorldAxis() const
{
	return m_vecWorldAxis;
}

Matrix CTransform::GetWorldMatrix() const
{
	return m_matWorld;
}

Matrix CTransform::GetWorldRotMatrix() const
{
	return m_matWorldRot;
}