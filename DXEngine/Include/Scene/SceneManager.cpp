#include "PCHEngine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Utility/Input/Input.h"
#include "Object/GameObject.h"
#include "Object/ObjectManager.h"

ENGINE_USING
DEFINITION_SINGLE(CSceneManager)

CSceneManager::CSceneManager() :
	m_isChange(false) ,
	m_pScene(nullptr),
	m_pNextScene(nullptr)
{
}

CSceneManager::~CSceneManager()
{
	SAFE_RELEASE(m_pScene);
	SAFE_RELEASE(m_pNextScene);
}

bool CSceneManager::Init()
{
	// ¾À »ý¼º
	m_pScene = new CScene();
	if (m_pScene->Init() == false)
	{
		return false;
	}

	// ¸¶¿ì½º ¾À ¼³Á¤
	GET_SINGLE(CInput)->ChangeMouseScene(m_pScene);

	return true;
}

int CSceneManager::Input(float _fTime)
{
	m_pScene->Input(_fTime);
	return ChangeScene();
}

int CSceneManager::Update(float _fTime)
{
	m_pScene->Update(_fTime);
	return ChangeScene();
}

int CSceneManager::LateUpdate(float _fTime)
{
	m_pScene->LateUpdate(_fTime);
	return ChangeScene();
}

int CSceneManager::Collision(float _fTime)
{
	m_pScene->Collision(_fTime);
	return ChangeScene();
}

void CSceneManager::Render(float _fTime)
{
	m_pScene->Render(_fTime);
}

int CSceneManager::ChangeScene()
{
	if (m_pNextScene && m_isChange == true)
	{
		m_isChange = false;
		SAFE_RELEASE(m_pScene);
		m_pScene = m_pNextScene;
		m_pNextScene = nullptr;

		GET_SINGLE(CInput)->ChangeMouseScene(m_pScene);
		GET_SINGLE(CObjectManager)->ChangeScene(m_pScene);

		m_pScene->Start();

		return 1;
	}

	return 0;
}

CScene* CSceneManager::GetScene() const
{
	m_pScene->AddRef();
	return m_pScene;
}

CScene * CSceneManager::GetNextScene() const
{
	m_pNextScene->AddRef();
	return m_pNextScene;
}

void CSceneManager::CreateNextScene(bool _isChange)
{
	SAFE_RELEASE(m_pNextScene);
	m_pNextScene = new CScene;

	m_pNextScene->Init();
	m_isChange = _isChange;
}

void CSceneManager::ChangeEnable()
{
	m_isChange = true;
}

void CSceneManager::AddLayer(const string& _strTag, int _iZOrder, bool _isCurrent)
{
	if (_isCurrent == true)
	{
		m_pScene->AddLayer(_strTag, _iZOrder);
	}
	else
	{
		m_pNextScene->AddLayer(_strTag, _iZOrder);
	}
}

void CSceneManager::ChangeLayerZOrder(const string& _strTag, int _iZOrder, bool _isCurrent)
{
	if (_isCurrent == true)
	{
		m_pScene->ChangeLayerZOrder(_strTag, _iZOrder);
	}
	else
	{
		m_pNextScene->ChangeLayerZOrder(_strTag, _iZOrder);
	}
}

CLayer* CSceneManager::FindLayer(const string& _strTag, bool _isCurrent)
{
	if (_isCurrent == true)
	{
		return m_pScene->FindLayer(_strTag);
	}
	else
	{
		return m_pNextScene->FindLayer(_strTag);
	}
	return nullptr;
}

CGameObject* CSceneManager::FindObject(const string& _strTag)
{
	CGameObject* pObj = m_pScene->FindObject(_strTag);

	if (pObj)
		return pObj;

	if (!m_pNextScene)
		return nullptr;

	return m_pNextScene->FindObject(_strTag);
}