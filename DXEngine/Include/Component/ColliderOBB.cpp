#include "PCHEngine.h"

#include "ColliderOBB.h"
#include "ColliderPoint.h"
#include "ColliderRect.h"
#include "ColliderCircle.h"
#include "ColliderPixel.h"
#include "Component/Camera.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"
#include "Resources/Mesh.h"
#include "Resources/ResourcesManager.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/RenderManager.h"

ENGINE_USING

CColliderOBB::CColliderOBB()
{
	m_eCollType = CT_OBB;
#ifdef _DEBUG
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh("ColliderRect");
	m_pDepthDisable = GET_SINGLE(CRenderManager)->FindRenderState(DEPTH_DISABLE);
#endif // _DEBUG
}

CColliderOBB::CColliderOBB(const CColliderOBB& _obb)
{
	*this = _obb;
}

CColliderOBB::~CColliderOBB()
{
}

void CColliderOBB::SetInfo(const Vector3& _vCetner, Vector3 _vAxis[2], float _fLength[2])
{
	// OBB Center ���� ������Ʈ�� �������� �����Ѵ�.
	m_tRelativeInfo.vCenter = _vCetner;
	for (int i = 0; i < 2; ++i)
	{
		// Axis   : 2D������ Axis(X,Y)���� �˻��Ѵ�.
		// Length : ����
		m_tRelativeInfo.vAxis[i]	= _vAxis[i];
		m_tRelativeInfo.fLength[i]	= _fLength[i];
		m_tInfo.fLength[i]			= _fLength[i];
	}
}

OBB2DInfo CColliderOBB::GetInfo() const
{
	return m_tInfo;
}

bool CColliderOBB::Collision(CCollider* _pDest, float _fTime)
{
	// �浹 �Ǵ�
	switch (_pDest->GetColliderType())
	{
		case CT_RECT:
		{
			CColliderRect* pColliderRect = (CColliderRect*)_pDest;
			return CollisionOBB2DRectToRect(m_tInfo, pColliderRect->GetInfo());
		}
		case CT_CIRCLE:
		{
			CColliderCircle* pColliderCircle = (CColliderCircle*)_pDest;
			return CollisionOBB2DRectToCircle(m_tInfo, pColliderCircle->GetInfo());
		}
		case CT_POINT:
		{
			CColliderPoint* pColliderPoint = (CColliderPoint*)_pDest;
			return CollisionOBB2DRectToPoint(m_tInfo, pColliderPoint->GetInfo());
		}
		case CT_OBB:
		{
			CColliderOBB* pColliderOBB = (CColliderOBB*)_pDest;
			return CollisionOBB2DRectToOBB2DRect(m_tInfo, pColliderOBB->GetInfo());
		}
		case CT_PIXEL:
		{
			return false;
		}
	}

	return false;
}

bool CColliderOBB::Init()
{
	return true;
}

int CColliderOBB::Input(float _fTime)
{
	return 0;
}

int CColliderOBB::Update(float _fTime)
{
	return 0;
}

int CColliderOBB::LateUpdate(float _fTime)
{
	// OBB �浹ü ������ ����� ���� ��� ���
	// > ������Ʈ�� ��ġ�� �������� ���
	// > 41, 42, 43 �̵� ��� ��� ���� ������Ʈ�� ���� ��ġ ������ ����
	Matrix matWorld = m_pTransform->GetParentRotMatrix();
	memcpy(&matWorld._41, &m_pTransform->GetWorldPos(), sizeof(Vector3));
	//matWorld *= m_pTransform->GetParentMatrix();

	// ���� x ��� (w=1) ����
	m_tInfo.vCenter = m_tRelativeInfo.vCenter.TransformCoord(matWorld);

	Vector3	vAxis[2];
	for (int i = 0; i < 2; ++i)
	{
		// TransformNormal(Matrix) = ������ ��Ʈ������ ���� 3���� �Ϲ� ����(���� ����)�� ��ȯ�մϴ�.
		m_tInfo.vAxis[i] = m_tRelativeInfo.vAxis[i].TransformNormal(matWorld); 
		m_tInfo.vAxis[i].Normalize();

		// ����ȭ�� ���Ϳ� ���̸� ���Ѵ�.
		vAxis[i] = m_tInfo.vAxis[i] * m_tInfo.fLength[i];
	}

	float minX = (m_tInfo.vCenter - vAxis[0] - vAxis[1]).x;
	float minY = (m_tInfo.vCenter - vAxis[0] - vAxis[1]).y;
	float maxX = (m_tInfo.vCenter + vAxis[0] + vAxis[1]).x;
	float maxY = (m_tInfo.vCenter + vAxis[0] + vAxis[1]).y;

	// ���� �浹ü ����
	m_vColliderSizeMin.x = min(minX, maxX);
	m_vColliderSizeMin.y = min(minY, maxY);
	m_vColliderSizeMax.x = max(minX, maxX);
	m_vColliderSizeMax.y = max(minY, maxY);

	return 0;
}

void CColliderOBB::Collision(float _fTime)
{
}

void CColliderOBB::Render(float _fTime)
{
#ifdef _DEBUG
	Matrix	matPos, matScale, matRot;

	// ũ�⸦ ���Ѵ�.
	Vector3	vLength = Vector3(m_tInfo.fLength[0] * 2.f, m_tInfo.fLength[1] * 2.f, 1.f);

	// ȸ�� ������ ���Ѵ�. ȸ���� 2D �̱� ������ Z�� ȸ���� �Ѵ�.
	float	fAngle = m_tInfo.vAxis[0].Angle(Vector3(1.f, 0.f, 0.f));

	if (m_tInfo.vAxis[0].y < 0.f)
		fAngle = 360.f - fAngle;

	matRot.RotationZ(fAngle);
	matPos.Translation(m_tInfo.vCenter);
	matScale.Scaling(vLength);

	TransformCBuffer	tCBuffer = {};

	CCamera*	pMainCamera = m_pScene->GetMainCamera();

	Matrix	matView;

	if (m_strCollisionGroup != "UI")
		matView = pMainCamera->GetViewMatrix();

	tCBuffer.matWorld = matScale * matRot * matPos;
	tCBuffer.matView = matView;
	tCBuffer.matProj = pMainCamera->GetProjMatrix();
	tCBuffer.matWV = tCBuffer.matWorld * tCBuffer.matView;
	tCBuffer.matWVP = tCBuffer.matWV * tCBuffer.matProj;
	tCBuffer.vPivot = Vector3(0.5f, 0.5f, 0.f);
	tCBuffer.vLength = m_pMesh->GetLength();

	tCBuffer.matWorld.Transpose();
	tCBuffer.matView.Transpose();
	tCBuffer.matProj.Transpose();
	tCBuffer.matWV.Transpose();
	tCBuffer.matWVP.Transpose();

	GET_SINGLE(CShaderManager)->UpdateCBuffer("Transform", &tCBuffer);

	SAFE_RELEASE(pMainCamera);

	CCollider::Render(_fTime);
#endif // _DEBUG
}

CColliderOBB* CColliderOBB::Clone()
{
	return new CColliderOBB(*this);
}