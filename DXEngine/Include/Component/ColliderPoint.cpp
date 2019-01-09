
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
	// 충돌 판단
	switch (_pDest->GetColliderType())
	{
		case CT_POINT:
		{
			// 점과 점 충돌
			return m_vPos == ((CColliderPoint*)_pDest)->m_vPos;
		}
		case CT_RECT:
		{
			// 사각형과 점 충돌
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
	// 오브젝트의 위치 + 상대적 위치 
	Vector3	vPos = m_pTransform->GetWorldPos();
	m_vPos		 = vPos + m_vRelativePos;

	// 이 정보 자체가 사각형의 영역을 판단할 데이터로 사용된다.
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

	// 공간변환 계산
	Matrix matPos, matScale;
	matPos.Translation(m_vPos - Vector3(colliderScale.x/2.f, colliderScale.y/2.f, 0.f));
	matScale.Scaling(colliderScale);

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
	tCBuffer.matWorld		  = matScale * matPos;
	tCBuffer.matView		  = matView;
	tCBuffer.matProj		  = pMainCamera->GetProjMatrix();
	tCBuffer.matWV			  = tCBuffer.matWorld * tCBuffer.matView;
	tCBuffer.matWVP			  = tCBuffer.matWV * tCBuffer.matProj;
	tCBuffer.vPivot			  = Vector3::Zero;
	tCBuffer.vLength		  = m_pMesh->GetLength();
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
	CCollider::Render(_fTime);
#endif // _DEBUG
}

CColliderPoint* CColliderPoint::Clone()
{
	return new CColliderPoint(*this);
}