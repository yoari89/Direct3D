#include "PCHEngine.h"
#include "Transform.h"
#include "Object/GameObject.h"

ENGINE_USING

CTransform::CTransform() :
	m_isUpdate(true),
	m_isStatic(false),
	m_iParentFlag(0),
	m_pParentTransform(nullptr)
{
	// ������Ʈ Ÿ��
	m_eComType = CT_TRANSFORM;
}

CTransform::CTransform(const CTransform& _tr) : 
	CComponent(_tr)
{
	*this = _tr;
	m_isUpdate = true;
	m_pParentTransform = nullptr;
}

CTransform::~CTransform()
{
	Safe_Release_VecList(m_childTransformList);
}

CTransform* CTransform::Clone()
{
	// ������Ʈ�� '����' �Ͽ� ���
	return new CTransform(*this);
}

void* CTransform::operator new(size_t _iSize)
{
	return _aligned_malloc(_iSize, 16);
}

void CTransform::operator delete(void* _p)
{
	_aligned_free(_p);
}

bool CTransform::Init()
{
	for (int i = 0; i < 3; ++i)
	{
		m_vecLocalAxis[i] = Vector3::Axis[i];
		m_vecWorldAxis[i] = Vector3::Axis[i];
	}
	return true;
}

int CTransform::Input(float _fTime)
{
	return 0;
}

int CTransform::Update(float _fTime)
{
	// ���� ��ü (��� ��� ������ ���� �ʿ䰡 ����.)
	if (m_isStatic == true)
	{
		return 0;
	}

	// �� ������ ��� ��� ����
	else if (m_isUpdate == false)
	{
		return 0;
	}

	// 'ũ��->����->�̵�->����->�θ�' ���� ����� ����� ���� ���� �� �θ� ����� ����� ���� ���
	if (m_pParentTransform)
	{
		if (m_iParentFlag & TPF_ROT && m_iParentFlag & TPF_POS)
		{
			m_matParent = m_pParentTransform->GetParentMatrixFromNoScale();
		}
		else if (m_iParentFlag & TPF_ROT)
		{
			m_matParent = m_pParentTransform->GetParentRotMatrix();
		}
		else if (m_iParentFlag & TPF_POS)
		{
			m_matParent = m_pParentTransform->GetParentPosMatrix();
		}
		else
		{
			m_matParent.Identity();
		}
	}

	// ���� ��� ���
	m_matLocal = m_matLocalScale * m_matLocalRot * m_matLocalPos;

	// ���� ��� ���
	m_matWorld = m_matWorldScale * m_matWorldRot * m_matWorldPos * m_matParent;

	m_isUpdate = false;

	return 0;
}

int CTransform::LateUpdate(float _fTime)
{
	if (m_isStatic == true)
	{
		return 0;
	}
	else if (m_isUpdate == false)
	{
		return 0;
	}

	if (m_pParentTransform)
	{
		if (m_iParentFlag & TPF_ROT && m_iParentFlag & TPF_POS)
		{
			m_matParent = m_pParentTransform->GetParentMatrixFromNoScale();
		}
		else if (m_iParentFlag & TPF_ROT)
		{
			m_matParent = m_pParentTransform->GetParentRotMatrix();
		}
		else if (m_iParentFlag & TPF_POS)
		{
			m_matParent = m_pParentTransform->GetParentPosMatrix();
		}
		else
		{
			m_matParent.Identity();
		}
	}

	m_matLocal = m_matLocalScale * m_matLocalRot * m_matLocalPos;
	m_matWorld = m_matWorldScale * m_matWorldRot * m_matWorldPos * m_matParent;
	m_isUpdate = false;
	return 0;
}

void CTransform::Collision(float _fTime)
{
}

void CTransform::Render(float _fTime)
{
	m_vecWorldMove = Vector3::Zero;
}

void CTransform::Static(bool _isStatic)
{
	m_isStatic = _isStatic;
}

// ========== �θ�, �ڽ� Transform ==========

void CTransform::SetParentFlag(int iFlag)
{
	m_iParentFlag = iFlag;
}

void CTransform::AddParentFlag(TRANSFORM_PARENT_FLAG eFlag)
{
	m_iParentFlag |= eFlag;
}

void CTransform::DeleteParentFlag(TRANSFORM_PARENT_FLAG eFlag)
{
	if (m_iParentFlag & eFlag)
		m_iParentFlag ^= eFlag;
}

void CTransform::DeleteParentFlag()
{
	m_iParentFlag = 0;
}