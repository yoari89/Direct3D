#pragma once
#include "../ReferenceCnt.h"

ENGINE_BEGIN
class CScene;
class CGameObject;
class ENGINE_DLL CLayer : public CReferenceCnt
{
private:
	CLayer();
	~CLayer();

private:
	friend CScene;

private:
	int					m_iZorder;
	CScene*				m_pScene;	
	list<CGameObject*>	m_objList;

public:
	void Start();
	bool Init();
	int  Input(float _fTime);
	int  Update(float _fTime);
	int  LateUpdate(float _fTime);
	void Collision(float _fTime);
	void Render(float _fTime);

// [[ZOrder]]
public:
	void SetZOrder(int _iZorder);
	int  GetZOrder() const;

	class CScene* GetScene()	const;

// [[오브젝트]]
public:
	void AddObject(CGameObject* _pObj);
	void AddObject(const string _strLayerTag, CGameObject* _pObj);
	void EraseObject(const string& _strTag);
	void SetObjLastZOrder(const string& _strTag);
	CGameObject* FindObject(const string& _strTag);
};
ENGINE_END