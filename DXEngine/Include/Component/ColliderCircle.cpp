#include "PCHEngine.h"
#include "ColliderCircle.h"
#include "ColliderPoint.h"
#include "ColliderRect.h"
#include "ColliderOBB.h"
#include "Component/Camera.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"
#include "Resources/Mesh.h"
#include "Resources/ResourcesManager.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/RenderManager.h"

ENGINE_USING

CColliderCircle::CColliderCircle()
{
	m_eCollType = CT_CIRCLE;
#ifdef _DEBUG
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh("ColliderCircle");
	m_pDepthDisable = GET_SINGLE(CRenderManager)->FindRenderState(DEPTH_DISABLE);
#endif
}

CColliderCircle::CColliderCircle(const CColliderCircle& _com)
{
	m_tRelativeInfo = _com.m_tRelativeInfo;
}

CColliderCircle::~CColliderCircle()
{
}

void CColliderCircle::SetInfo(const Vector3& _vCenter, float _fRadius)
{
	m_tRelativeInfo.vCenter = _vCenter;
	m_tRelativeInfo.fRadius = _fRadius;
}

CircleInfo CColliderCircle::GetInfo() const
{
	return m_tInfo;
}

bool CColliderCircle::Collision(CCollider* _pDest, float _fTime)
{
	// �浹 �Ǵ�
	switch (_pDest->GetColliderType())
	{
		case CT_RECT:
		{
			// ���� �簢�� �浹
			CColliderRect* pColliderRect = (CColliderRect*)_pDest;
			return CollisionRectToCircle(pColliderRect->GetInfo(), m_tInfo);
		}
		case CT_POINT:
		{
			// ���� �� �浹
			CColliderPoint* pColliderPoint = (CColliderPoint*)_pDest;
			//_cprintf("%f %f\n", pColliderPoint->GetInfo().x, pColliderPoint->GetInfo().y);
			return CollisionCircleToPoint(m_tInfo, pColliderPoint->GetInfo());
		}
		case CT_CIRCLE:
		{
			CColliderCircle* pColliderCircle = (CColliderCircle*)_pDest;
			return CollisionCircleToCircle(m_tInfo, pColliderCircle->GetInfo());
		}
		case CT_OBB:
		{
			CColliderOBB* pColliderOBB = (CColliderOBB*)_pDest;
			return CollisionOBB2DRectToCircle(pColliderOBB->GetInfo(), m_tInfo);
		}
	}

	return false;
}

bool CColliderCircle::Init()
{
	return true;
}

int CColliderCircle::Input(float fTime)
{
	return 0;
}

int CColliderCircle::Update(float fTime)
{
	return 0;
}

int CColliderCircle::LateUpdate(float fTime)
{
	m_tRelativeInfo.vCenter = Vector3(m_pTransform->GetWorldPos().x, 
									  m_pTransform->GetWorldPos().y + m_tRelativeInfo.fRadius, 1.0f);
	/*_cprintf("%f %f %f\n", m_tRelativeInfo.vCenter.x, m_tRelativeInfo.vCenter.y, m_tRelativeInfo.vCenter.z);*/
	m_tInfo = m_tRelativeInfo;
	m_vColliderSizeMin.x = m_tInfo.vCenter.x - m_tInfo.fRadius;
	m_vColliderSizeMin.y = m_tInfo.vCenter.y - m_tInfo.fRadius;
	m_vColliderSizeMax.x = m_tInfo.vCenter.x + m_tInfo.fRadius;
	m_vColliderSizeMax.y = m_tInfo.vCenter.y + m_tInfo.fRadius;
	return 0;
}

void CColliderCircle::Collision(float fTime)
{
}

void CColliderCircle::Render(float fTime)
{
#ifdef _DEBUG
	Matrix matPos, matScale;
	matPos.Translation(m_tInfo.vCenter);
	matScale.Scaling(Vector3(m_tRelativeInfo.fRadius, m_tRelativeInfo.fRadius, 1.f));

	// ī�޶� ������Ʈ
	// Matrix ���� �� �⺻ �׵� ���(���� ���)
	Matrix   matView;
	CCamera* pMainCamera = m_pScene->GetMainCamera();

	// �浹 �׷��� UI�� �ƴ� ��� ī����� �� ����� �����´�.
	// > UI �׷��� ī�޶� ��ġ�� ������ ���� �ʴ´�.(������ ũ�� �״�� ����) 
	if (m_strCollisionGroup != "UI")
	{
		matView = pMainCamera->GetViewMatrix();
	}
	// �������	: ũ�� * ����(ȸ��) * �̵� * ���� *�θ�
	// > ���ؽ� ���̴� ��꿡�� Pivot * Length �� �����Ͽ� ��ġ ������ ���ش�.
	TransformCBuffer tCBuffer = {};
	tCBuffer.matWorld = matScale * matPos;
	tCBuffer.matView = matView;
	tCBuffer.matProj = pMainCamera->GetProjMatrix();
	tCBuffer.matWV = tCBuffer.matWorld * tCBuffer.matView;
	tCBuffer.matWVP = tCBuffer.matWV * tCBuffer.matProj;
	tCBuffer.vPivot = m_vPivot;
	tCBuffer.vLength = m_pMesh->GetLength();
	// ��ġ ���
	tCBuffer.matWorld.Transpose();
	tCBuffer.matView.Transpose();
	tCBuffer.matProj.Transpose();
	tCBuffer.matWV.Transpose();
	tCBuffer.matWVP.Transpose();

	// ī�޶� ��ü �޸� ����
	SAFE_RELEASE(pMainCamera);

	// Transform ��� ���� ������Ʈ
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Transform", &tCBuffer);

	// �浹ü ������ ����
	CCollider::Render(fTime);
#endif // _DEBUG
}

CColliderCircle* CColliderCircle::Clone()
{
	return new CColliderCircle(*this);
}