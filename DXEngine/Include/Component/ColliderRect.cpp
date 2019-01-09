#include "PCHEngine.h"
#include "ColliderRect.h"
#include "ColliderPoint.h"
#include "ColliderCircle.h"
#include "ColliderOBB.h"
#include "ColliderPixel.h"
#include "Component/Camera.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"
#include "Resources/Mesh.h"
#include "Resources/ResourcesManager.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/RenderManager.h"

ENGINE_USING

CColliderRect::CColliderRect()
{
	m_eCollType		= CT_RECT;
#ifdef _DEBUG
	m_pMesh			= GET_SINGLE(CResourcesManager)->FindMesh("ColliderRect");
	m_pDepthDisable = GET_SINGLE(CRenderManager)->FindRenderState(DEPTH_DISABLE);
#endif // _DEBUG
}

CColliderRect::CColliderRect(const CColliderRect& _com) :
	CCollider(_com)
{
	m_tRelativeInfo = _com.m_tRelativeInfo;
}

CColliderRect::~CColliderRect()
{
}

void CColliderRect::SetInfo(const Vector3& _vMin, const Vector3& _vMax)
{
	// ����� ũ�� ����
	// > ������Ʈ���� CColliderRect�� ���� ����� �Ѱܹ޴´�.
	m_tRelativeInfo.vMin		= _vMin;
	m_tRelativeInfo.vMax		= _vMax;
	m_tRelativeInfo.vLength		= _vMax - _vMin;
	m_tRelativeInfo.vMin.z		= 0.f;
	m_tRelativeInfo.vMax.z		= 0.f;
	m_tRelativeInfo.vLength.z	= 1.f;
}

BoxInfo CColliderRect::GetInfo() const
{
	return m_tInfo;
}

bool CColliderRect::Collision(CCollider* _pDest, float _fTime)
{
	// Collision() ȣ�� ����
	// > CollisionManager
	//   - CollisionMouseWindow()
	//   - CollisionMouseWorld()
	//   - CollisionObject()
	// > ���� ���� �� Collision() �ܰ迡�� ȣ��ȴ�.
	// > �浹�� �Ǵ��ϱ� ���� LateUpdate()�� �ּ� �ѹ� ȣ���ؾ��Ѵ�.
	//   - LateUpdate() �� �浹 ���� ����� �ȴ�.
	// > ���� ���� : LateUpdate() -> Collision() -> Render();

	// �浹 �Ǵ�
	switch (_pDest->GetColliderType())
	{
		case CT_RECT:
		{
			// ���� �簢�� �浹
			CColliderRect* pColliderRect = (CColliderRect*)_pDest;
			return CollisionRectToRect(m_tInfo, pColliderRect->m_tInfo);
		}
		case CT_POINT:
		{
			// ���� �� �浹
			CColliderPoint* pColliderPoint = (CColliderPoint*)_pDest;
			return CollisionRectToPoint(m_tInfo, pColliderPoint->GetInfo());
		}
		case CT_CIRCLE:
		{
			CColliderCircle* pColliderCircle = (CColliderCircle*)_pDest;
			return CollisionRectToCircle(m_tInfo, pColliderCircle->GetInfo());
		}
		case CT_OBB:
		{
			CColliderOBB* pColliderOBB = (CColliderOBB*)_pDest;
			//return CollisionOBB2DRectToRect(pColliderOBB->GetInfo(), m_tInfo);
		}
		case CT_PIXEL:
		{
			CColliderPixel* pColliderPixel = (CColliderPixel*)_pDest;
			return CollisionRectToPixel(m_tInfo, pColliderPixel->GetInfo());
		}
	}
	
	return false;
}

bool CColliderRect::Init()
{
	return true;
}

int CColliderRect::Input(float fTime)
{
	return 0;
}

int CColliderRect::Update(float fTime)
{
	return 0;
}

int CColliderRect::LateUpdate(float fTime)
{
	// ���� ��ġ(vPos) : ������Ʈ ���� ��ġ - (�߽��� * ������Ʈ ũ��) : Left,Bottom
	// �ּ� ũ��(vMin) : ���� ��ġ�������� m_tRelativeInfo.vMin ũ��
	// �ִ� ũ��(vMin) : ���� ��ġ�������� m_tRelativeInfo.vMax ũ��
	Vector3	vPos	= m_pTransform->GetWorldPos() - (m_pTransform->GetPivot() * m_pTransform->GetWorldScale());
	m_tInfo.vMin	= vPos + m_tRelativeInfo.vMin + (m_vPivot * m_tRelativeInfo.vLength);
	m_tInfo.vMax	= vPos + m_tRelativeInfo.vMax + (m_vPivot * m_tRelativeInfo.vLength);
	m_tInfo.vLength = m_tRelativeInfo.vLength;

	// SectionMin
	// > �浹ü ��ü�� ����, tInfo ���� ��ġ�Ѵ�. 
	// > Collider Ŭ�������� Section(Min, Max) ������ �˻��Ѵ�. 
	m_vColliderSizeMin = m_tInfo.vMin;
	m_vColliderSizeMax = m_tInfo.vMax;

	return 0;
}

void CColliderRect::Collision(float fTime)
{
	
}

void CColliderRect::Render(float fTime)
{
#ifdef _DEBUG
	// Matrix ���� �� �⺻ �׵� ���(���� ���)
	// ������ȯ ���
	Matrix matPos, matScale;
	matPos.Translation(m_tInfo.vMin);
	matScale.Scaling(m_tInfo.vLength);

	// ī�޶� ������Ʈ
	// �浹 �׷��� UI�� �ƴ� ��� ī����� �� ����� �����´�.
	// > UI �׷��� ī�޶� ��ġ�� ������ ���� �ʴ´�.(������ ũ�� �״�� ����) 
	Matrix matView;
	CCamera* pMainCamera = m_pScene->GetMainCamera();
	if (m_strCollisionGroup != "UI")
	{
		matView = pMainCamera->GetViewMatrix();
	}

	// �������	: ũ�� * ����(ȸ��) * �̵� * ���� *�θ�
	// > ���ؽ� ���̴� ��꿡�� Pivot * Length �� �����Ͽ� ��ġ ������ ���ش�.
	TransformCBuffer tCBuffer	= {};
	tCBuffer.matWorld			= matScale * matPos;
	tCBuffer.matView			= matView;
	tCBuffer.matProj			= pMainCamera->GetProjMatrix();
	tCBuffer.matWV				= tCBuffer.matWorld * tCBuffer.matView;
	tCBuffer.matWVP				= tCBuffer.matWV * tCBuffer.matProj;
	tCBuffer.vPivot				= m_vPivot;
	tCBuffer.vLength			= m_pMesh->GetLength();
	
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

CColliderRect* CColliderRect::Clone()
{
	return new CColliderRect(*this);
}