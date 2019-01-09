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
	// 충돌 판단
	switch (_pDest->GetColliderType())
	{
		case CT_RECT:
		{
			// 점과 사각형 충돌
			CColliderRect* pColliderRect = (CColliderRect*)_pDest;
			return CollisionRectToCircle(pColliderRect->GetInfo(), m_tInfo);
		}
		case CT_POINT:
		{
			// 원과 점 충돌
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

	// 카메라 오브젝트
	// Matrix 선언 시 기본 항등 행렬(단위 행렬)
	Matrix   matView;
	CCamera* pMainCamera = m_pScene->GetMainCamera();

	// 충돌 그룹이 UI가 아닌 경우 카멜라의 뷰 행렬을 가져온다.
	// > UI 그룹은 카메라 위치에 영향을 받지 않는다.(윈도우 크기 그대로 적용) 
	if (m_strCollisionGroup != "UI")
	{
		matView = pMainCamera->GetViewMatrix();
	}
	// 월드행렬	: 크기 * 자전(회전) * 이동 * 공전 *부모
	// > 버텍스 셰이더 계산에서 Pivot * Length 을 적용하여 위치 적용을 해준다.
	TransformCBuffer tCBuffer = {};
	tCBuffer.matWorld = matScale * matPos;
	tCBuffer.matView = matView;
	tCBuffer.matProj = pMainCamera->GetProjMatrix();
	tCBuffer.matWV = tCBuffer.matWorld * tCBuffer.matView;
	tCBuffer.matWVP = tCBuffer.matWV * tCBuffer.matProj;
	tCBuffer.vPivot = m_vPivot;
	tCBuffer.vLength = m_pMesh->GetLength();
	// 전치 행렬
	tCBuffer.matWorld.Transpose();
	tCBuffer.matView.Transpose();
	tCBuffer.matProj.Transpose();
	tCBuffer.matWV.Transpose();
	tCBuffer.matWVP.Transpose();

	// 카메라 객체 메모리 해제
	SAFE_RELEASE(pMainCamera);

	// Transform 상수 버퍼 업데이트
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Transform", &tCBuffer);

	// 충돌체 렌더링 시작
	CCollider::Render(fTime);
#endif // _DEBUG
}

CColliderCircle* CColliderCircle::Clone()
{
	return new CColliderCircle(*this);
}