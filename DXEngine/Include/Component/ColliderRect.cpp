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
	// 상대적 크기 설정
	// > 오브젝트에서 CColliderRect에 대한 사이즈를 넘겨받는다.
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
	// Collision() 호출 시점
	// > CollisionManager
	//   - CollisionMouseWindow()
	//   - CollisionMouseWorld()
	//   - CollisionObject()
	// > 게임 로직 중 Collision() 단계에서 호출된다.
	// > 충돌을 판단하기 위해 LateUpdate()를 최소 한번 호출해야한다.
	//   - LateUpdate() 후 충돌 영역 계산이 된다.
	// > 게임 로직 : LateUpdate() -> Collision() -> Render();

	// 충돌 판단
	switch (_pDest->GetColliderType())
	{
		case CT_RECT:
		{
			// 점과 사각형 충돌
			CColliderRect* pColliderRect = (CColliderRect*)_pDest;
			return CollisionRectToRect(m_tInfo, pColliderRect->m_tInfo);
		}
		case CT_POINT:
		{
			// 점과 점 충돌
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
	// 시작 위치(vPos) : 오브젝트 월드 위치 - (중심점 * 오브젝트 크기) : Left,Bottom
	// 최소 크기(vMin) : 시작 위치에서부터 m_tRelativeInfo.vMin 크기
	// 최대 크기(vMin) : 시작 위치에서부터 m_tRelativeInfo.vMax 크기
	Vector3	vPos	= m_pTransform->GetWorldPos() - (m_pTransform->GetPivot() * m_pTransform->GetWorldScale());
	m_tInfo.vMin	= vPos + m_tRelativeInfo.vMin + (m_vPivot * m_tRelativeInfo.vLength);
	m_tInfo.vMax	= vPos + m_tRelativeInfo.vMax + (m_vPivot * m_tRelativeInfo.vLength);
	m_tInfo.vLength = m_tRelativeInfo.vLength;

	// SectionMin
	// > 충돌체 자체의 영역, tInfo 값과 일치한다. 
	// > Collider 클래스에서 Section(Min, Max) 값으로 검사한다. 
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
	// Matrix 선언 시 기본 항등 행렬(단위 행렬)
	// 공간변환 계산
	Matrix matPos, matScale;
	matPos.Translation(m_tInfo.vMin);
	matScale.Scaling(m_tInfo.vLength);

	// 카메라 오브젝트
	// 충돌 그룹이 UI가 아닌 경우 카멜라의 뷰 행렬을 가져온다.
	// > UI 그룹은 카메라 위치에 영향을 받지 않는다.(윈도우 크기 그대로 적용) 
	Matrix matView;
	CCamera* pMainCamera = m_pScene->GetMainCamera();
	if (m_strCollisionGroup != "UI")
	{
		matView = pMainCamera->GetViewMatrix();
	}

	// 월드행렬	: 크기 * 자전(회전) * 이동 * 공전 *부모
	// > 버텍스 셰이더 계산에서 Pivot * Length 을 적용하여 위치 적용을 해준다.
	TransformCBuffer tCBuffer	= {};
	tCBuffer.matWorld			= matScale * matPos;
	tCBuffer.matView			= matView;
	tCBuffer.matProj			= pMainCamera->GetProjMatrix();
	tCBuffer.matWV				= tCBuffer.matWorld * tCBuffer.matView;
	tCBuffer.matWVP				= tCBuffer.matWV * tCBuffer.matProj;
	tCBuffer.vPivot				= m_vPivot;
	tCBuffer.vLength			= m_pMesh->GetLength();
	
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

CColliderRect* CColliderRect::Clone()
{
	return new CColliderRect(*this);
}