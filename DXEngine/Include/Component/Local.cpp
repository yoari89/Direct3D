#include "PCHEngine.h"
#include "Transform.h"

ENGINE_USING

void CTransform::SetLocalScale(const Vector3& _vecScale)
{
	// Vector3 Ÿ���� �̿��� ũ�� ��� ���
	m_vecLocalScale = _vecScale;
	m_matLocalScale.Scaling(m_vecLocalScale);
	m_isUpdate = true;
}

void CTransform::SetLocalScale(float _x, float _y, float _z)
{
	// x, y, z ���� �̿��� ũ�� ��� ���
	m_vecLocalScale = Vector3(_x, _y, _z);
	m_matLocalScale.Scaling(m_vecLocalScale);
	m_isUpdate = true;
}

void CTransform::SetLocalRot(const Vector3& _vecRot)
{
	// Vector3 Ÿ���� �̿��� ȸ��(����) ��� ���
	m_vecLocalRot = _vecRot;
	m_matLocalRot.Rotation(m_vecLocalRot);
	m_isUpdate = true;

	// ȸ���ϸ� �� ���� �޶�����. 
	// ���� ȸ�� ���(m_matLocalRot) ���� �̿��Ͽ� ����ȭ�� �� ���
	ComputeLocalAxis();
}

void CTransform::SetLocalRot(float _x, float _y, float _z)
{
	// x, y, z ���� �̿��� ȸ�� ��� ���
	m_vecLocalRot = Vector3(_x, _y, _z);
	m_matLocalRot.Rotation(m_vecLocalRot);
	m_isUpdate = true;
	ComputeLocalAxis();
}

void CTransform::SetLocalRotX(float _x)
{
	// x�� ȸ��
	m_vecLocalRot.x = _x;
	m_matLocalRot.Rotation(m_vecLocalRot);
	m_isUpdate = true;
	ComputeLocalAxis();
}

void CTransform::SetLocalRotY(float _y)
{
	// y�� ȸ��
	m_vecLocalRot.y = _y;
	m_matLocalRot.Rotation(m_vecLocalRot);
	m_isUpdate = true;
	ComputeLocalAxis();
}

void CTransform::SetLocalRotZ(float _z)
{
	// z�� ȸ��
	m_vecLocalRot.z = _z;
	m_matLocalRot.Rotation(m_vecLocalRot);
	m_isUpdate = true;
	ComputeLocalAxis();
}

void CTransform::SetLocalPos(const Vector3& _vecPos)
{
	// Vector3 Ÿ���� �̿��� �̵� ��� ���
	m_vecLocalPos = _vecPos;
	m_matLocalPos.Translation(m_vecLocalPos);
	m_isUpdate = true;
}

void CTransform::SetLocalPos(float _x, float _y, float _z)
{
	// x, y, z ���� �̿��� �̵� ��� ���
	m_vecLocalPos = Vector3(_x, _y, _z);
	m_matLocalPos.Translation(m_vecLocalPos);
	m_isUpdate = true;
	ComputeLocalAxis();
}

void CTransform::ComputeLocalAxis()
{
	// �ڽ��� ���� ȸ�� ����� ���鶧 �ڽ��� �θ� ������Ʈ�� �����Ѵٸ�?
	// �ڽ��� ���� ȸ�� ��İ� �θ��� ���� ȸ�� ����� ���Ѵ�.
	Matrix matRot = m_matLocalRot;
	if (m_pParentTransform != nullptr)
	{
		matRot = matRot* m_pParentTransform->GetLocalRotMatrix();
	}

	// ���� �� ���
	for (int i = 0; i < AXIS_END; ++i)
	{
		// XMVector3TransformNormal
		// MSDN : ������ ��ķ� 3D ���� ������ �����մϴ�.
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