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
	// �ٶ󺸴� ����
	Vector3 vView = _vPos - m_vecWorldPos;
	vView.Normalize();					   // 0.? 0.? 0
	Vector3 vAxis = Vector3::Axis[_eAxis]; // 0 1 0

										   // ����
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
	// �θ� Transform�� ���ٸ� ����� ũ��� �ڽ��� ���� ũ�Ⱑ �ȴ�.
	if (m_pParentTransform == nullptr)
	{
		m_vWorldRelativeScale = m_vecWorldScale;
	}
	// �θ��� ũ�� ���� ���� �ʴ´ٸ� ����� ũ��� �ڽ��� ���� ũ�Ⱑ �ȴ�.
	else if ((m_iParentFlag & TPF_SCALE) == 0)
	{
		m_vWorldRelativeScale = m_vecWorldScale / m_pParentTransform->m_vecWorldScale;
	}
	// �θ��� ũ�⿡ ������ �޴´�.
	// > ���ÿ��� ���� ũ�� ����ó�� ���������, ���� ����� �θ� ũ�⿡ ����Ͽ� ���Ǿ��Ѵ�.
	else
	{
		m_vWorldRelativeScale = m_vecWorldScale / m_pParentTransform->m_vecWorldScale;
	}

	// m_vecWorldScale ���� �̿��Ͽ� ũ�� ��� ���
	m_matWorldScale.Scaling(m_vecWorldScale);

	// �ڽ� Transform�� �ִٸ�?
	// > �ڽ� Transform�� ũ�� ���� �θ� ũ�⿡ ���� ũ�� ���� ����Ѵ�.
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
	// �θ� Transform�� ���ٸ� ����� ��ġ�� �ڽ��� ���� ��ġ�� �ȴ�.
	if (m_pParentTransform == nullptr)
	{
		m_vWorldRelativePos = m_vecWorldPos;
	}
	// �θ��� ��ġ�� ���� ���� �ʴ´ٸ� ����� ��ġ�� �ڽ��� ���� ��ġ�� �ȴ�.
	else if ((m_iParentFlag & TPF_POS) == 0)
	{
		m_vWorldRelativePos = m_vecWorldPos;
	}

	// m_vWorldRelativePos ���� �̿��Ͽ� ��ġ ��� ���
	m_matWorldPos.Translation(m_vWorldRelativePos);

	// �θ� ������Ʈ�� �ִٸ�?
	// �θ𿡰Լ� ���ǿ� ���� ��� ���� ������ m_vWorldRelativePos�� ��� �����Ͽ� ��ġ ���͸� ���Ѵ�.
	// > (��, �θ𿡰Լ� ������ �޴� ���¿� ���� �θ� ����� �����, �θ� ��İ��� �̿��Ͽ� 
	//        �ڽ��� ���� ��ġ ���� ����Ѵ�.)
	if (m_pParentTransform)
	{
		// �θ� ȸ����, �ڽ��� �θ��� ȸ�� ���¿� ������ �޴´ٸ� �翬�� ��ġ�� ����ȴ�. (����)
		Matrix	matParent;
		if (m_iParentFlag & TPF_POS && m_iParentFlag & TPF_ROT)
		{
			// ũ�� ����
			matParent = m_pParentTransform->GetParentMatrixFromNoScale();
		}
		else if (m_iParentFlag & TPF_POS)
		{
			// ��ġ
			matParent = m_pParentTransform->GetParentPosMatrix();
		}
		else if (m_iParentFlag & TPF_ROT)
		{
			// ũ��
			matParent = m_pParentTransform->GetParentRotMatrix();
		}
		else
		{
			// �׵� ���
			matParent.Identity();
		}

		// ���� ��ġ ��
		Vector3 vPos = m_vWorldRelativePos.TransformCoord(matParent);
		m_vecWorldMove = vPos - m_vecWorldPos;
		m_vecWorldPos = vPos;
	}

	// �ڽ� Transform�� �ִٸ�?
	// > �ڽ� Transform�� ��ġ ���� �θ��� ���� ���ο� ���� ��ġ ���� ����Ѵ�.
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