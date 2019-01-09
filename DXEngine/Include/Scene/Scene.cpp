#include "PCHEngine.h"
#include "../Device.h"
#include "Scene.h"
#include "Layer.h"
#include "Utility/Input/Input.h"
#include "Object/GameObject.h"
#include "SceneComponent.h"
#include "Component/Camera.h"
#include "Component/Transform.h"
#include "Utility/Collision/CollisionManager.h"
#include "Utility/Parser/XML/tinyxml2.h"

ENGINE_USING

CScene::CScene()
{
}

CScene::~CScene()
{
	// 프로토타입 삭제
	CGameObject::DestroyPrototype(this);

	// 씬 소멸시 씬 컴포넌트, 레이어 메모리 해제
	Safe_Release_VecList(m_layerList);
	Safe_Release_VecList(m_sceneComponentList);
	Safe_Release_Map<unordered_map<string, CGameObject*>>(m_mapCamera);

	SAFE_RELEASE(m_pMainCameraObj);
	SAFE_RELEASE(m_pMainCameraTr);
	SAFE_RELEASE(m_pMainCamera);

	SAFE_RELEASE(m_pUICameraObj);
	SAFE_RELEASE(m_pUICameraTr);
	SAFE_RELEASE(m_pUICamera);
}

CGameObject* CScene::GetMainCameraObj() const
{
	m_pMainCameraObj->AddRef();
	return m_pMainCameraObj;
}

CCamera* CScene::GetMainCamera() const
{
	m_pMainCamera->AddRef();
	return m_pMainCamera;
}

CTransform* CScene::GetMainCameraTransform() const
{
	m_pMainCameraTr->AddRef();
	return m_pMainCameraTr;
}

CGameObject* CScene::GetUICameraObj() const
{
	m_pUICameraObj->AddRef();
	return m_pUICameraObj;
}

CCamera* CScene::GetUICamera() const
{
	m_pUICamera->AddRef();
	return m_pUICamera;
}

CTransform* CScene::GetUICameraTransform() const
{
	m_pUICameraTr->AddRef();
	return m_pUICameraTr;
}

void CScene::AddLayer(const string& _strTag, int _iZorder)
{
	// 레이어 생성
	CLayer* pLayer = new CLayer();
	pLayer->SetTag(_strTag);
	pLayer->m_pScene = this;

	// 초기화
	if (pLayer->Init() == false)
	{
		SAFE_RELEASE(pLayer);
		return;
	}

	// 추가
	m_layerList.push_back(pLayer);

	// 정렬
	pLayer->SetZOrder(_iZorder);
}

void CScene::SortLayer()
{
	// order sequence, using operator<
	// 비교연산자 재정의
	m_layerList.sort(CScene::SortLayerZOrder);
}

void CScene::ChangeLayerZOrder(const string& _strTag, int _iZorder)
{
	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_layerList.end();
	for (iter = m_layerList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == _strTag)
		{
			(*iter)->SetZOrder(_iZorder);
			return;
		}
	}
}

void CScene::EnableLayer(const string& _strTag, bool _isEnable)
{
	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_layerList.end();
	for (iter = m_layerList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == _strTag)
		{
			(*iter)->SetEnable(_isEnable);
			SortLayer();
			return;
		}
	}
}

CLayer* CScene::FindLayer(const string& _strTag)
{
	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_layerList.end();
	for (iter = m_layerList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == _strTag)
		{
			(*iter)->AddRef();
			return *iter;
		}
	}
	return nullptr;
}

bool CScene::SortLayerZOrder(const CLayer* _pSrc, const CLayer* _pDest)
{
	return _pSrc->GetZOrder() < _pDest->GetZOrder();
}

CGameObject* CScene::FindObject(const string& _strTag)
{
	list<CLayer*>::iterator	iter;
	list<CLayer*>::iterator	iterEnd = m_layerList.end();
	for (iter = m_layerList.begin(); iter != iterEnd; ++iter)
	{
		CGameObject* pObj = (*iter)->FindObject(_strTag);
		if (pObj)
		{
			return pObj;
		}
	}
	return nullptr;
}

CGameObject* CScene::CreateCamera(const string& _strTag, const Vector3& _vPos, CAMERA_TYPE _eType, 
								  float _fWidth, float _fHeight, float _fVieweAngle, float _fNear, float _fFar)
{
	CGameObject* pCameraObj = FindCamera(_strTag);
	if (pCameraObj != nullptr)
	{
		return pCameraObj;
	}

	// 카메라 오브젝트 생성
	pCameraObj = CGameObject::CreateObject(_strTag);
	CTransform*	pTr = pCameraObj->GetTransform();

	//pTr->SetWorldPos(_vPos);
	SAFE_RELEASE(pTr);

	// 카메라 컴포넌트 등록 및 설정
	CCamera* pCamera = pCameraObj->AddComponent<CCamera>("Camera");
	pCamera->SetCameraInfo(_eType, _fWidth, _fHeight, _fVieweAngle, _fNear, _fFar);
	SAFE_RELEASE(pCamera);

	// 카메라 등록
	pCameraObj->AddRef();
	m_mapCamera.insert(make_pair(_strTag, pCameraObj));
	return pCameraObj;
}

void CScene::ChangeCamera(const string& _strTag)
{
	CGameObject* pCameraObj = FindCamera(_strTag);
	if (pCameraObj == nullptr)
	{
		return;
	}

	// 기존에 참조하던 메모리 해제
	SAFE_RELEASE(m_pMainCameraObj);
	SAFE_RELEASE(m_pMainCamera);
	SAFE_RELEASE(m_pMainCameraTr);

	// 변경
	m_pMainCameraObj = pCameraObj;
	m_pMainCameraTr  = pCameraObj->GetTransform();
	m_pMainCamera	 = pCameraObj->FindComponentFromType<CCamera>(CT_CAMERA);
}

CGameObject* CScene::FindCamera(const string& _strTag)
{
	unordered_map<string, CGameObject*>::iterator iter = m_mapCamera.find(_strTag);
	if (iter == m_mapCamera.end())
	{
		return nullptr;
	}
	iter->second->AddRef();
	return iter->second;
}

void CScene::EnableSceneComponent(const string& _strTag, bool _isEnable)
{
	list<CSceneComponent*>::iterator iter;
	list<CSceneComponent*>::iterator iter1End = m_sceneComponentList.end();
	for (iter = m_sceneComponentList.begin(); iter != iter1End; ++iter)
	{
		if ((*iter)->GetTag() == _strTag)
		{
			(*iter)->SetEnable(_isEnable);
			return;
		}
	}
}

void CScene::Start()
{
	list<CLayer*>::iterator	iter;
	list<CLayer*>::iterator	iterEnd = m_layerList.end();
	for (iter = m_layerList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Start();
	}

	list<CSceneComponent*>::iterator	iter1;
	list<CSceneComponent*>::iterator	iter1End = m_sceneComponentList.end();
	for (iter1 = m_sceneComponentList.begin(); iter1 != iter1End; ++iter1)
	{
		(*iter1)->Start();
	}
}

bool CScene::Init()
{
	// Scene에서 사용되는 Layer 목록들을 필요에 의해서 추가한다.
	// Unity3D 엔진에서 사용되는 레이어 개념을 직접 구현한다고 보면된다.
	AddLayer("BackGround",		INT_MIN + 1);
	AddLayer("SkillBack",		0);
	AddLayer("Monster",			1);
	AddLayer("Player",			2);
	AddLayer("SkillFront",		3);
	AddLayer("UIBack",			INT_MAX - 4);
	AddLayer("UI",				INT_MAX - 3);
	AddLayer("QuickSlotItem",	INT_MAX - 2);
	AddLayer("QuickSlotNum",	INT_MAX - 1);
	AddLayer("ScreenEffect",	INT_MAX);

	// 메인 카메라 등록
	m_pMainCameraObj = CreateCamera("MainCamera", Vector3(0.0f, 0.0f, -5.0f), CT_PERSPECTIVE, 
									(float)_RESOLUTION.iWidth, (float)_RESOLUTION.iHeight, 60.0f, 0.03f, 1000.0f);
	m_pMainCameraTr  = m_pMainCameraObj->GetTransform();
	m_pMainCamera	 = m_pMainCameraObj->FindComponentFromType<CCamera>(CT_CAMERA);

	// UI 카메라
	m_pUICameraObj = CreateCamera("UICamera", Vector3(0.f, 0.f, 0.f), CT_ORTHO, 
								  (float)_RESOLUTION.iWidth, (float)_RESOLUTION.iHeight, 60.f, 0.f, 1000.f);
	m_pUICameraTr = m_pUICameraObj->GetTransform();
	m_pUICamera = m_pUICameraObj->FindComponentFromType<CCamera>(CT_CAMERA);

	return true;
}

int CScene::Input(float _fTime)
{
	// 씬 컴포넌트
	list<CSceneComponent*>::iterator compIter;
	list<CSceneComponent*>::iterator compIterEnd = m_sceneComponentList.end();
	for (compIter = m_sceneComponentList.begin(); compIter != compIterEnd;)
	{
		// 씬 컴포넌트가 파괴된 상태
		if ((*compIter)->GetActive() == false)
		{
			SAFE_RELEASE((*compIter));
			compIter = m_sceneComponentList.erase(compIter);
			continue;
		}

		// 씬 컴포넌트가 비활성화 상태
		else if ((*compIter)->GetEnable() == false)
		{
			++compIter;
			continue;
		}

		// 씬 컴포넌트 Input() 호출 
		(*compIter)->Input(_fTime);
		++compIter;
	}

	// 레이어
	list<CLayer*>::iterator	layerIter;
	list<CLayer*>::iterator	layerIterEnd = m_layerList.end();
	for (layerIter = m_layerList.begin(); layerIter != layerIterEnd;)
	{
		// 레이어가 파괴된 상태
		if ((*layerIter)->GetActive() == false)
		{
			SAFE_RELEASE((*layerIter));
			layerIter = m_layerList.erase(layerIter);
			continue;
		}

		// 레이어가 비활성화 상태
		else if ((*layerIter)->GetEnable() == false)
		{
			++layerIter;
			continue;
		}

		// 레이어 Input() 호출 
		(*layerIter)->Input(_fTime);
		++layerIter;
	}

	// 메인 카메라 Input() 호출
	m_pMainCameraObj->Input(_fTime);

	return 0;
}

int CScene::Update(float _fTime)
{
	// 씬 컴포넌트
	list<CSceneComponent*>::iterator compIter;
	list<CSceneComponent*>::iterator compIterEnd = m_sceneComponentList.end();
	for (compIter = m_sceneComponentList.begin(); compIter != compIterEnd;)
	{
		// 씬 컴포넌트가 파괴된 상태
		if ((*compIter)->GetActive() == false)
		{
			SAFE_RELEASE((*compIter));
			compIter = m_sceneComponentList.erase(compIter);
			continue;
		}

		// 씬 컴포넌트가 비활성화 상태
		else if ((*compIter)->GetEnable() == false)
		{
			++compIter;
			continue;
		}

		// 씬 컴포넌트 Update() 호출 
		(*compIter)->Update(_fTime);
		++compIter;
	}

	// 레이어
	list<CLayer*>::iterator	layerIter;
	list<CLayer*>::iterator	layerIterEnd = m_layerList.end();
	for (layerIter = m_layerList.begin(); layerIter != layerIterEnd;)
	{
		// 레이어가 파괴된 상태
		if ((*layerIter)->GetActive() == false)
		{
			SAFE_RELEASE((*layerIter));
			layerIter = m_layerList.erase(layerIter);
			continue;
		}

		// 레이어가 비활성화 상태
		else if ((*layerIter)->GetEnable() == false)
		{
			++layerIter;
			continue;
		}

		// 레이어 Update() 호출 
		(*layerIter)->Update(_fTime);
		++layerIter;
	}
	
	// 메인 카메라 Update() 호출
	m_pMainCameraObj->Update(_fTime);

	return 0;
}

int CScene::LateUpdate(float _fTime)
{
	// 씬 컴포넌트
	list<CSceneComponent*>::iterator compIter;
	list<CSceneComponent*>::iterator compIterEnd = m_sceneComponentList.end();
	for (compIter = m_sceneComponentList.begin(); compIter != compIterEnd;)
	{
		// 씬 컴포넌트가 파괴된 상태
		if ((*compIter)->GetActive() == false)
		{
			SAFE_RELEASE((*compIter));
			compIter = m_sceneComponentList.erase(compIter);
			continue;
		}

		// 씬 컴포넌트가 비활성화 상태
		else if ((*compIter)->GetEnable() == false)
		{
			++compIter;
			continue;
		}

		// 씬 컴포넌트 LateUpdate() 호출 
		(*compIter)->LateUpdate(_fTime);
		++compIter;
	}

	// 레이어
	list<CLayer*>::iterator	layerIter;
	list<CLayer*>::iterator	layerIterEnd = m_layerList.end();
	for (layerIter = m_layerList.begin(); layerIter != layerIterEnd;)
	{
		// 레이어가 파괴된 상태
		if ((*layerIter)->GetActive() == false)
		{
			SAFE_RELEASE((*layerIter));
			layerIter = m_layerList.erase(layerIter);
			continue;
		}

		// 레이어가 비활성화 상태
		else if ((*layerIter)->GetEnable() == false)
		{
			++layerIter;
			continue;
		}

		// 레이어 LateUpdate() 호출 
		(*layerIter)->LateUpdate(_fTime);
		++layerIter;
	}

	// 메인 카메라 LateUpdate() 호출
	m_pMainCameraObj->LateUpdate(_fTime);

	return 0;
}

void CScene::Collision(float _fTime)
{
	// 씬 컴포넌트
	list<CSceneComponent*>::iterator compIter;
	list<CSceneComponent*>::iterator compIterEnd = m_sceneComponentList.end();
	for (compIter = m_sceneComponentList.begin(); compIter != compIterEnd;)
	{
		// 씬 컴포넌트가 파괴된 상태
		if ((*compIter)->GetActive() == false)
		{
			SAFE_RELEASE((*compIter));
			compIter = m_sceneComponentList.erase(compIter);
			continue;
		}

		// 씬 컴포넌트가 비활성화 상태
		else if ((*compIter)->GetEnable() == false)
		{
			++compIter;
			continue;
		}

		// 씬 컴포넌트 Collision() 호출 
		(*compIter)->Collision(_fTime);
		++compIter;
	}

	// 레이어
	list<CLayer*>::iterator	layerIter;
	list<CLayer*>::iterator	layerIterEnd = m_layerList.end();
	for (layerIter = m_layerList.begin(); layerIter != layerIterEnd;)
	{
		// 레이어가 파괴된 상태
		if ((*layerIter)->GetActive() == false)
		{
			SAFE_RELEASE((*layerIter));
			layerIter = m_layerList.erase(layerIter);
			continue;
		}

		// 레이어가 비활성화 상태
		else if ((*layerIter)->GetEnable() == false)
		{
			++layerIter;
			continue;
		}

		// 레이어 Collision() 호출 
		(*layerIter)->Collision(_fTime);
		++layerIter;
	}

	// 마우스(World) 정보 설정
	// > 마우스는 Window(UI와 충돌하기 위한 충돌체), World(카메라에 영향받는 충돌체)가 있다.
	// > 마우스(World)는 씬 메인 카메라의 영향을 받는다.
	GET_SINGLE(CInput)->AddMouseWorldCollision();

	// Collision Manager Collision() 호출
	GET_SINGLE(CCollisionManager)->Collision(_fTime);
}

void CScene::Render(float _fTime)
{
	// 씬 컴포넌트
	list<CSceneComponent*>::iterator compIter;
	list<CSceneComponent*>::iterator compIterEnd = m_sceneComponentList.end();
	for (compIter = m_sceneComponentList.begin(); compIter != compIterEnd;)
	{
		// 씬 컴포넌트가 파괴된 상태
		if ((*compIter)->GetActive() == false)
		{
			SAFE_RELEASE((*compIter));
			compIter = m_sceneComponentList.erase(compIter);
			continue;
		}

		// 씬 컴포넌트가 비활성화 상태
		else if ((*compIter)->GetEnable() == false)
		{
			++compIter;
			continue;
		}

		// 씬 컴포넌트 Render() 호출 
		(*compIter)->Render(_fTime);
		++compIter;
	}

	// 레이어
	list<CLayer*>::iterator	layerIter;
	list<CLayer*>::iterator	layerIterEnd = m_layerList.end();
	for (layerIter = m_layerList.begin(); layerIter != layerIterEnd;)
	{
		// 레이어가 파괴된 상태
		if ((*layerIter)->GetActive() == false)
		{
			SAFE_RELEASE((*layerIter));
			layerIter = m_layerList.erase(layerIter);
			continue;
		}

		// 레이어가 비활성화 상태
		else if ((*layerIter)->GetEnable() == false)
		{
			++layerIter;
			continue;
		}

		// 레이어 Render() 호출 
		(*layerIter)->Render(_fTime);
		++layerIter;
	}
}