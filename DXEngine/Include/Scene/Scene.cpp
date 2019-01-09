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
	// ������Ÿ�� ����
	CGameObject::DestroyPrototype(this);

	// �� �Ҹ�� �� ������Ʈ, ���̾� �޸� ����
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
	// ���̾� ����
	CLayer* pLayer = new CLayer();
	pLayer->SetTag(_strTag);
	pLayer->m_pScene = this;

	// �ʱ�ȭ
	if (pLayer->Init() == false)
	{
		SAFE_RELEASE(pLayer);
		return;
	}

	// �߰�
	m_layerList.push_back(pLayer);

	// ����
	pLayer->SetZOrder(_iZorder);
}

void CScene::SortLayer()
{
	// order sequence, using operator<
	// �񱳿����� ������
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

	// ī�޶� ������Ʈ ����
	pCameraObj = CGameObject::CreateObject(_strTag);
	CTransform*	pTr = pCameraObj->GetTransform();

	//pTr->SetWorldPos(_vPos);
	SAFE_RELEASE(pTr);

	// ī�޶� ������Ʈ ��� �� ����
	CCamera* pCamera = pCameraObj->AddComponent<CCamera>("Camera");
	pCamera->SetCameraInfo(_eType, _fWidth, _fHeight, _fVieweAngle, _fNear, _fFar);
	SAFE_RELEASE(pCamera);

	// ī�޶� ���
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

	// ������ �����ϴ� �޸� ����
	SAFE_RELEASE(m_pMainCameraObj);
	SAFE_RELEASE(m_pMainCamera);
	SAFE_RELEASE(m_pMainCameraTr);

	// ����
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
	// Scene���� ���Ǵ� Layer ��ϵ��� �ʿ信 ���ؼ� �߰��Ѵ�.
	// Unity3D �������� ���Ǵ� ���̾� ������ ���� �����Ѵٰ� ����ȴ�.
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

	// ���� ī�޶� ���
	m_pMainCameraObj = CreateCamera("MainCamera", Vector3(0.0f, 0.0f, -5.0f), CT_PERSPECTIVE, 
									(float)_RESOLUTION.iWidth, (float)_RESOLUTION.iHeight, 60.0f, 0.03f, 1000.0f);
	m_pMainCameraTr  = m_pMainCameraObj->GetTransform();
	m_pMainCamera	 = m_pMainCameraObj->FindComponentFromType<CCamera>(CT_CAMERA);

	// UI ī�޶�
	m_pUICameraObj = CreateCamera("UICamera", Vector3(0.f, 0.f, 0.f), CT_ORTHO, 
								  (float)_RESOLUTION.iWidth, (float)_RESOLUTION.iHeight, 60.f, 0.f, 1000.f);
	m_pUICameraTr = m_pUICameraObj->GetTransform();
	m_pUICamera = m_pUICameraObj->FindComponentFromType<CCamera>(CT_CAMERA);

	return true;
}

int CScene::Input(float _fTime)
{
	// �� ������Ʈ
	list<CSceneComponent*>::iterator compIter;
	list<CSceneComponent*>::iterator compIterEnd = m_sceneComponentList.end();
	for (compIter = m_sceneComponentList.begin(); compIter != compIterEnd;)
	{
		// �� ������Ʈ�� �ı��� ����
		if ((*compIter)->GetActive() == false)
		{
			SAFE_RELEASE((*compIter));
			compIter = m_sceneComponentList.erase(compIter);
			continue;
		}

		// �� ������Ʈ�� ��Ȱ��ȭ ����
		else if ((*compIter)->GetEnable() == false)
		{
			++compIter;
			continue;
		}

		// �� ������Ʈ Input() ȣ�� 
		(*compIter)->Input(_fTime);
		++compIter;
	}

	// ���̾�
	list<CLayer*>::iterator	layerIter;
	list<CLayer*>::iterator	layerIterEnd = m_layerList.end();
	for (layerIter = m_layerList.begin(); layerIter != layerIterEnd;)
	{
		// ���̾ �ı��� ����
		if ((*layerIter)->GetActive() == false)
		{
			SAFE_RELEASE((*layerIter));
			layerIter = m_layerList.erase(layerIter);
			continue;
		}

		// ���̾ ��Ȱ��ȭ ����
		else if ((*layerIter)->GetEnable() == false)
		{
			++layerIter;
			continue;
		}

		// ���̾� Input() ȣ�� 
		(*layerIter)->Input(_fTime);
		++layerIter;
	}

	// ���� ī�޶� Input() ȣ��
	m_pMainCameraObj->Input(_fTime);

	return 0;
}

int CScene::Update(float _fTime)
{
	// �� ������Ʈ
	list<CSceneComponent*>::iterator compIter;
	list<CSceneComponent*>::iterator compIterEnd = m_sceneComponentList.end();
	for (compIter = m_sceneComponentList.begin(); compIter != compIterEnd;)
	{
		// �� ������Ʈ�� �ı��� ����
		if ((*compIter)->GetActive() == false)
		{
			SAFE_RELEASE((*compIter));
			compIter = m_sceneComponentList.erase(compIter);
			continue;
		}

		// �� ������Ʈ�� ��Ȱ��ȭ ����
		else if ((*compIter)->GetEnable() == false)
		{
			++compIter;
			continue;
		}

		// �� ������Ʈ Update() ȣ�� 
		(*compIter)->Update(_fTime);
		++compIter;
	}

	// ���̾�
	list<CLayer*>::iterator	layerIter;
	list<CLayer*>::iterator	layerIterEnd = m_layerList.end();
	for (layerIter = m_layerList.begin(); layerIter != layerIterEnd;)
	{
		// ���̾ �ı��� ����
		if ((*layerIter)->GetActive() == false)
		{
			SAFE_RELEASE((*layerIter));
			layerIter = m_layerList.erase(layerIter);
			continue;
		}

		// ���̾ ��Ȱ��ȭ ����
		else if ((*layerIter)->GetEnable() == false)
		{
			++layerIter;
			continue;
		}

		// ���̾� Update() ȣ�� 
		(*layerIter)->Update(_fTime);
		++layerIter;
	}
	
	// ���� ī�޶� Update() ȣ��
	m_pMainCameraObj->Update(_fTime);

	return 0;
}

int CScene::LateUpdate(float _fTime)
{
	// �� ������Ʈ
	list<CSceneComponent*>::iterator compIter;
	list<CSceneComponent*>::iterator compIterEnd = m_sceneComponentList.end();
	for (compIter = m_sceneComponentList.begin(); compIter != compIterEnd;)
	{
		// �� ������Ʈ�� �ı��� ����
		if ((*compIter)->GetActive() == false)
		{
			SAFE_RELEASE((*compIter));
			compIter = m_sceneComponentList.erase(compIter);
			continue;
		}

		// �� ������Ʈ�� ��Ȱ��ȭ ����
		else if ((*compIter)->GetEnable() == false)
		{
			++compIter;
			continue;
		}

		// �� ������Ʈ LateUpdate() ȣ�� 
		(*compIter)->LateUpdate(_fTime);
		++compIter;
	}

	// ���̾�
	list<CLayer*>::iterator	layerIter;
	list<CLayer*>::iterator	layerIterEnd = m_layerList.end();
	for (layerIter = m_layerList.begin(); layerIter != layerIterEnd;)
	{
		// ���̾ �ı��� ����
		if ((*layerIter)->GetActive() == false)
		{
			SAFE_RELEASE((*layerIter));
			layerIter = m_layerList.erase(layerIter);
			continue;
		}

		// ���̾ ��Ȱ��ȭ ����
		else if ((*layerIter)->GetEnable() == false)
		{
			++layerIter;
			continue;
		}

		// ���̾� LateUpdate() ȣ�� 
		(*layerIter)->LateUpdate(_fTime);
		++layerIter;
	}

	// ���� ī�޶� LateUpdate() ȣ��
	m_pMainCameraObj->LateUpdate(_fTime);

	return 0;
}

void CScene::Collision(float _fTime)
{
	// �� ������Ʈ
	list<CSceneComponent*>::iterator compIter;
	list<CSceneComponent*>::iterator compIterEnd = m_sceneComponentList.end();
	for (compIter = m_sceneComponentList.begin(); compIter != compIterEnd;)
	{
		// �� ������Ʈ�� �ı��� ����
		if ((*compIter)->GetActive() == false)
		{
			SAFE_RELEASE((*compIter));
			compIter = m_sceneComponentList.erase(compIter);
			continue;
		}

		// �� ������Ʈ�� ��Ȱ��ȭ ����
		else if ((*compIter)->GetEnable() == false)
		{
			++compIter;
			continue;
		}

		// �� ������Ʈ Collision() ȣ�� 
		(*compIter)->Collision(_fTime);
		++compIter;
	}

	// ���̾�
	list<CLayer*>::iterator	layerIter;
	list<CLayer*>::iterator	layerIterEnd = m_layerList.end();
	for (layerIter = m_layerList.begin(); layerIter != layerIterEnd;)
	{
		// ���̾ �ı��� ����
		if ((*layerIter)->GetActive() == false)
		{
			SAFE_RELEASE((*layerIter));
			layerIter = m_layerList.erase(layerIter);
			continue;
		}

		// ���̾ ��Ȱ��ȭ ����
		else if ((*layerIter)->GetEnable() == false)
		{
			++layerIter;
			continue;
		}

		// ���̾� Collision() ȣ�� 
		(*layerIter)->Collision(_fTime);
		++layerIter;
	}

	// ���콺(World) ���� ����
	// > ���콺�� Window(UI�� �浹�ϱ� ���� �浹ü), World(ī�޶� ����޴� �浹ü)�� �ִ�.
	// > ���콺(World)�� �� ���� ī�޶��� ������ �޴´�.
	GET_SINGLE(CInput)->AddMouseWorldCollision();

	// Collision Manager Collision() ȣ��
	GET_SINGLE(CCollisionManager)->Collision(_fTime);
}

void CScene::Render(float _fTime)
{
	// �� ������Ʈ
	list<CSceneComponent*>::iterator compIter;
	list<CSceneComponent*>::iterator compIterEnd = m_sceneComponentList.end();
	for (compIter = m_sceneComponentList.begin(); compIter != compIterEnd;)
	{
		// �� ������Ʈ�� �ı��� ����
		if ((*compIter)->GetActive() == false)
		{
			SAFE_RELEASE((*compIter));
			compIter = m_sceneComponentList.erase(compIter);
			continue;
		}

		// �� ������Ʈ�� ��Ȱ��ȭ ����
		else if ((*compIter)->GetEnable() == false)
		{
			++compIter;
			continue;
		}

		// �� ������Ʈ Render() ȣ�� 
		(*compIter)->Render(_fTime);
		++compIter;
	}

	// ���̾�
	list<CLayer*>::iterator	layerIter;
	list<CLayer*>::iterator	layerIterEnd = m_layerList.end();
	for (layerIter = m_layerList.begin(); layerIter != layerIterEnd;)
	{
		// ���̾ �ı��� ����
		if ((*layerIter)->GetActive() == false)
		{
			SAFE_RELEASE((*layerIter));
			layerIter = m_layerList.erase(layerIter);
			continue;
		}

		// ���̾ ��Ȱ��ȭ ����
		else if ((*layerIter)->GetEnable() == false)
		{
			++layerIter;
			continue;
		}

		// ���̾� Render() ȣ�� 
		(*layerIter)->Render(_fTime);
		++layerIter;
	}
}