#pragma once
#include "Utility/Types.h"
#include "../ReferenceCnt.h"

ENGINE_BEGIN
class CLayer;
class CSceneComponent;
class CSceneManager;
class CGameObject;
class CCamera;
class CTransform;
class ENGINE_DLL CScene : public CReferenceCnt
{
private:
	CScene();
	~CScene();

private:
	friend CSceneManager;

private:
	list<CLayer*>		   m_layerList;				
	list<CSceneComponent*> m_sceneComponentList;	

	// > ī�޶�
	unordered_map<string, CGameObject*> m_mapCamera;
	CGameObject* m_pMainCameraObj;
	CTransform*	 m_pMainCameraTr;
	CCamera*	 m_pMainCamera;

	// > UI ī�޶�
	CGameObject*	m_pUICameraObj;
	CTransform*		m_pUICameraTr;
	CCamera*		m_pUICamera;

public:
	CGameObject*	GetMainCameraObj()			const;
	CCamera*		GetMainCamera()				const;
	CTransform*		GetMainCameraTransform()	const;

	CGameObject*	GetUICameraObj()			const;
	CCamera*		GetUICamera()				const;
	CTransform*		GetUICameraTransform()		const;

// [[���̾�]]
public:
	void	AddLayer(const string& _strTag, int _iZorder);
	void	SortLayer();
	void	ChangeLayerZOrder(const string& _strTag, int _iZorder);
	void	EnableLayer(const string& _strTag, bool _isEnable);
	CLayer* FindLayer(const string& _strTag);
	static bool SortLayerZOrder(const CLayer* _pSrc, const CLayer* _pDest);

// [[������Ʈ]]
public:
	CGameObject* FindObject(const string& _strTag);
	CGameObject* CreateCamera(const string& _strTag, const Vector3& _vPos, CAMERA_TYPE _eType,
							  float _fWidth, float _fHeight, float _fVieweAngle, float _fNear, float _fFar);
	void ChangeCamera(const string& _strTag);

private:
	CGameObject* FindCamera(const string& _strTag);

// [[�� ������Ʈ]]
public:
	void EnableSceneComponent(const string& _strTag, bool _isEnable);

	// � ������Ʈ�� �߰����� �𸣱� ������ ���ø��� ���
	template<typename T>
	bool AddSceneComponent(const string& _strTag)
	{
		T* pCom = new T();
		pCom->SetTag(_strTag);
		pCom->m_pScene = this;

		// �ʱ�ȭ
		if (pCom->Init() == false)
		{
			SAFE_RELEASE(pCom);
			return false;
		}

		// �߰�
		m_sceneComponentList.push_back(pCom);
		return true;
	}

// [[Game Logic]]
public:
	void Start();
	bool Init();					// ��
	int  Input(float _fTime);		// ��
	int  Update(float _fTime);		// ��
	int  LateUpdate(float _fTime);	// ��
	void Collision(float _fTime);	// ��
	void Render(float _fTime);		// ��
};
ENGINE_END