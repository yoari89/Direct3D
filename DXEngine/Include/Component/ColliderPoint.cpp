
#include "PCHEngine.h"
#include "ColliderPoint.h"
#include "ColliderRect.h"
#include "ColliderCircle.h"
#include "ColliderOBB.h"
#include "ColliderPixel.h"
#include "Component/Camera.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"
#include "Resources/Mesh.h"
#include "Resources/ResourcesManager.h"
#include "Rendering/RenderManager.h"
#include "Rendering/ShaderManager.h"

ENGINE_USING

CColliderPoint::CColliderPoint()
{
	m_eCollType		= CT_POINT;
#ifdef _DEBUG
	m_pMesh			= GET_SINGLE(CResourcesManager)->FindMesh("ColliderRect");
	m_pDepthDisable = GET_SINGLE(CRenderManager)->FindRenderState(DEPTH_DISABLE);
#endif // _DEBUG
}

CColliderPoint::CColliderPoint(const CColliderPoint& _com)
{
	*this = _com;
}

CColliderPoint::~CColliderPoint()
{
}

void CColliderPoint::SetInfo(const Vector3& _vRelativePos)
{
	m_vRelativePos = _vRelativePos;
}

Vector3 CColliderPoint::GetInfo() const
{
	return m_vPos;
}

bool CColliderPoint::Collision(CCollider* _pDest, float _fTime)
{
	// �浹 �Ǵ�
	switch (_pDest->GetColliderType())
	{
		case CT_POINT:
		{
			// ���� �� �浹
			return m_vPos == ((CColliderPoint*)_pDest)->m_vPos;
		}
		case CT_RECT:
		{
			// �簢���� �� �浹
			return CollisionRectToPoint(((CColliderRect*)_pDest)->GetInfo(), m_vPos);
		}
		case CT_CIRCLE:
		{
			/*CColliderCircle* pColliderCircle = (CColliderCircle*)_pDest;
			return CollisionCircleToPoint(pColliderCircle->GetInfo(), m_vPos);*/
		}
		case CT_OBB:
		{
			CColliderOBB* pColliderOBB = (CColliderOBB*)_pDest;
			return CollisionOBB2DRectToPoint(pColliderOBB->GetInfo(), m_vPos);
		}
		case CT_PIXEL:
		{
			return false;
		}
	}
	
	return false;
}

bool CColliderPoint::Init()
{
	return true;
}

int CColliderPoint::Input(float _fTime)
{
	return 0;
}

int CColliderPoint::Update(float _fTime)
{
	return 0;
}

int CColliderPoint::LateUpdate(float _fTime)
{
	// ������Ʈ�� ��ġ + ����� ��ġ 
	Vector3	vPos = m_pTransform->GetWorldPos();
	m_vPos		 = vPos + m_vRelativePos;

	// �� ���� ��ü�� �簢���� ������ �Ǵ��� �����ͷ� ���ȴ�.
	m_vColliderSizeMin = m_vPos;
	m_vColliderSizeMax = m_vPos;

	return 0;
}

void CColliderPoint::Collision(float _fTime)
{
}

void CColliderPoint::Render(float _fTime)
{
#ifdef _DEBUG
	Vector3 colliderScale = Vector3(3.f, 3.f, 1.f);

	// ������ȯ ���
	Matrix matPos, matScale;
	matPos.Translation(m_vPos - Vector3(colliderScale.x/2.f, colliderScale.y/2.f, 0.f));
	matScale.Scaling(colliderScale);

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
	tCBuffer.matWorld		  = matScale * matPos;
	tCBuffer.matView		  = matView;
	tCBuffer.matProj		  = pMainCamera->GetProjMatrix();
	tCBuffer.matWV			  = tCBuffer.matWorld * tCBuffer.matView;
	tCBuffer.matWVP			  = tCBuffer.matWV * tCBuffer.matProj;
	tCBuffer.vPivot			  = Vector3::Zero;
	tCBuffer.vLength		  = m_pMesh->GetLength();
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
	CCollider::Render(_fTime);
#endif // _DEBUG
}

CColliderPoint* CColliderPoint::Clone()
{
	return new CColliderPoint(*this);
}