#include "PCHEngine.h"
#include "SlideControl.h"
#include "Component/Renderer.h"
#include "Component/Material.h"
#include "Component/Transform.h"
#include "Component/ColliderRect.h"
#include "Utility/Input/Input.h"
#include "Object/GameObject.h"
#include "Scene/Scene.h"
#include "Resources/Texture.h"
#include "Resources/ResourcesManager.h"

ENGINE_USING

CSlideControl::CSlideControl() :
	m_isPush(false),
	m_startMosePosX(0.f),
	m_beforeDistance(0.f),
	m_ratioEndPosX(0.f),
	m_ratioTr(nullptr),
	m_controllerTr(nullptr),
	m_controllerState(eControllerState::CONTROLLER_NORMAL)
{
	SetTag("SlideContorl");
}

CSlideControl::CSlideControl(const CSlideControl& _sildeControl) :
	CUI(_sildeControl)
{
}

CSlideControl::~CSlideControl()
{
	SAFE_RELEASE(m_ratioTr);
	SAFE_RELEASE(m_controllerTr);
}

bool CSlideControl::Init()
{
	return true;
}

int CSlideControl::Input(float _fTime)
{
	switch (m_controllerState)
	{
		case eControllerState::CONTROLLER_OVER:
		{
			if (m_isPush == false && KEYPUSH("LButton") == true)
			{
				m_isPush = true;
				m_startMosePosX   = GET_SINGLE(CInput)->GetMouseClient().x;
				m_controllerState = eControllerState::CONTROLLER_CLICK;

			}
			break;
		}
		case eControllerState::CONTROLLER_CLICK:
		{
			// Drag
			// > 2픽셀 차이가 나면 Drag 상태로 변경
			float mousePosX = GET_SINGLE(CInput)->GetMouseClient().x;
			float distanceX = abs((mousePosX - m_startMosePosX));
			if (distanceX >= 2.f)
			{
				m_controllerState = eControllerState::CONTROLLER_DRAG;
			}
			break;
		}
	}// end of switch

	if (m_isPush == true && KEYUP("LButton") == true)
	{
		m_isPush = false;
		m_beforeDistance = 0.f;
		m_controllerState = eControllerState::CONTROLLER_NORMAL;
	}

	return 0;
}

int CSlideControl::Update(float _fTime)
{
	if (m_controllerState == eControllerState::CONTROLLER_DRAG)
	{
		// Relative Position 계산
		// > 부모 위치 - 자식 위치
		Vector3 vControllerRelativePos = m_controllerTr->GetWorldPos() - m_ratioTr->GetWorldPos();
		if (vControllerRelativePos.x >= 0.f && vControllerRelativePos.x <= m_ratioEndPosX)
		{
			float mousePosX = GET_SINGLE(CInput)->GetMouseClient().x;
			float distance  = mousePosX - m_startMosePosX;
			if (m_beforeDistance != distance)
			{
				// 거리에 변화가 있을 경우에만 처리
				vControllerRelativePos.x += (distance - m_beforeDistance);
				m_beforeDistance = distance;
			}

			// 좌표 보정
			if (vControllerRelativePos.x < 0.f)
			{
				vControllerRelativePos.x = 0.f;
			}
			else if (vControllerRelativePos.x > m_ratioEndPosX)
			{
				vControllerRelativePos.x = m_ratioEndPosX;
			}
			m_controllerTr->SetWorldRelativePos(vControllerRelativePos);
			_cprintf("FromZeroToOnePercent : %f\n", FromZeroToOnePercent());
		}
	}
	return 0;
}

int CSlideControl::LateUpdate(float _fTime)
{
	return 0;
}

void CSlideControl::Collision(float _fTime)
{
}

void CSlideControl::Render(float _fTime)
{
}

void CSlideControl::AfterClone()
{
}

CSlideControl* CSlideControl::Clone()
{
	return nullptr;
}

void CSlideControl::CreateSlideContol(const string _pTexRatioName,	 Vector3 _vRatioPos,   Vector3 _vRatioSize,
									  const string _pTexControlName, Vector3 _vControlPos, Vector3 _vControlSize, 
									  const string _pTexBorderName,	 Vector3 _vBorderPos,  Vector3 _vBorderSize)
{
	// Border 생성
	if (_pTexBorderName != "")
	{
		CreateBorder(_pTexBorderName, _vBorderPos, _vBorderSize);
	}
	// Ratio 생성
	CreateRatio(_pTexRatioName, _vRatioPos, _vRatioSize);

	// SlideController 생성
	CreateController(_pTexControlName, _vControlPos, _vControlSize);
}

void CSlideControl::CreateBorder(const string _pTexBorderName, Vector3 _vBorderPos, Vector3 _vBorderSize)
{
	// 자식객체로 생성
	CGameObject* pObjBorder = CGameObject::CreateObject("SlideControl_Border", m_pLayer);
	m_pObject->AddChild(pObjBorder);
	CTransform* pBorderTr   = pObjBorder->GetTransform();
	pBorderTr->SetWorldScale(_vBorderSize);
	pBorderTr->SetWorldRelativePos(_vBorderPos);

	// Component(Renderer)
	CRenderer* pBorderRender = pObjBorder->AddComponent<CRenderer>("BorderRenderer");
	pBorderRender->SetMesh("TexRect");
	pBorderRender->SetRenderState(ALPHA_BLEND);

	// Component(Material)
	CTexture* pTexBorder = GET_SINGLE(CResourcesManager)->FindTexture(_pTexBorderName);
	CMaterial* pMaterial = pObjBorder->FindComponentFromType<CMaterial>(CT_MATERIAL);
	pMaterial->SetDiffuseTex(0, pTexBorder);

	// 메모리 해제
	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pTexBorder)
	SAFE_RELEASE(pBorderRender);
	SAFE_RELEASE(pBorderTr);
	SAFE_RELEASE(pObjBorder);
}

void CSlideControl::CreateRatio(const string _pTexRatioName, Vector3 _vRatioPos, Vector3 _vRatioSize)
{
	// 자식객체로 생성
	CGameObject* pObjRatio = CGameObject::CreateObject("SlideControl_Ratio");
	m_pObject->AddChild(pObjRatio);
	m_ratioTr = pObjRatio->GetTransform();
	m_ratioTr->SetWorldScale(_vRatioSize);
	m_ratioTr->SetWorldRelativePos(_vRatioPos);
	m_ratioTr->SetWorldPivot(Vector3(0.f, 0.5f, 0.f));

	// Component(Renderer)
	CRenderer* pBorderRender = pObjRatio->AddComponent<CRenderer>("RatioRenderer");
	pBorderRender->SetMesh("TexRect");
	pBorderRender->SetRenderState(ALPHA_BLEND);

	// Component(Material)
	CTexture* pTexRatio  = GET_SINGLE(CResourcesManager)->FindTexture(_pTexRatioName);
	CMaterial* pMaterial = pObjRatio->FindComponentFromType<CMaterial>(CT_MATERIAL);
	pMaterial->SetDiffuseTex(0, pTexRatio);

	// 길이 값 설정
	m_ratioEndPosX = m_ratioTr->GetWorldScale().x;

	// 메모리 해제
	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pTexRatio)
	SAFE_RELEASE(pBorderRender);
	SAFE_RELEASE(pObjRatio);
}

void CSlideControl::CreateController(const string _pTexControlName, Vector3 _vControlPos, Vector3 _vControlSize)
{
	// Controller는 Ratio의 자식으로 연결
	CGameObject* pObjController = CGameObject::CreateObject("SlideControl_Controller");
	CGameObject* pObjRatio = m_pObject->FindObject("SlideControl_Ratio");
	if (pObjRatio == nullptr)
	{
		return;
	}
	pObjRatio->AddChild(pObjController);
	m_controllerTr = pObjController->GetTransform();
	m_controllerTr->SetWorldScale(_vControlSize);
	m_controllerTr->SetWorldRelativePos(_vControlPos);
	m_controllerTr->SetWorldPivot(0.5f, 0.5f, 0.f);

	// Component(Renderer)
	CRenderer* pControllerRender = pObjController->AddComponent<CRenderer>("ControllerRenderer");
	pControllerRender->SetMesh("TexRect");
	pControllerRender->SetRenderState(ALPHA_BLEND);

	// Component(Material)
	CTexture* pTexController = GET_SINGLE(CResourcesManager)->FindTexture(_pTexControlName);
	CMaterial* pMaterial = pObjController->FindComponentFromType<CMaterial>(COMPONENT_TYPE::CT_MATERIAL);
	pMaterial->SetDiffuseTex(0, pTexController);

	// Component(Collider Rect)
	CColliderRect* pColRect = pObjController->AddComponent<CColliderRect>("ControllerCollider");
	pColRect->SetCollisionGroup("UI");
	float width  = (float)pTexController->GetWidth();
	float height = (float)pTexController->GetHeight();
	pColRect->SetInfo(Vector3::Zero, Vector3(width, height, 0.f));
	// > Collider CallBack Type - Enter -> MouseOn 함수 호출
	//                          - Leave -> MouseLeave 함수 호출
	pColRect->SetCollisionCallback(COLLISION_CALLBACK_TYPE::CCT_ENTER, this, &CSlideControl::ControllerOn);
	pColRect->SetCollisionCallback(COLLISION_CALLBACK_TYPE::CCT_LEAVE, this, &CSlideControl::ControllerLeave);

	// 메모리 해제
	SAFE_RELEASE(pColRect);
	SAFE_RELEASE(pMaterial);
	SAFE_RELEASE(pTexController)
	SAFE_RELEASE(pControllerRender);
	SAFE_RELEASE(pObjController);
	SAFE_RELEASE(pObjRatio);
}

float CSlideControl::FromZeroToOnePercent()
{
	// 0 ~ 1 비율
	// > Controller Position / RatioEndPos
	Vector3 vControllerRelativePos = m_controllerTr->GetWorldPos() - m_ratioTr->GetWorldPos();
	return (vControllerRelativePos.x / m_ratioEndPosX);
}

void CSlideControl::ControllerOn(CCollider* _pSrc, CCollider* _pDest, float _fTime)
{
	if (m_controllerState == eControllerState::CONTROLLER_NORMAL)
	{
		m_controllerState = eControllerState::CONTROLLER_OVER;
	}
}

void CSlideControl::ControllerLeave(CCollider* _pSrc, CCollider* _pDest, float _fTime)
{
	if (m_isPush == false)
	{
		m_controllerState = eControllerState::CONTROLLER_NORMAL;
	}
}