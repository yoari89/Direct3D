#pragma once

ENGINE_BEGIN
class CGameObject;
class ENGINE_DLL CObjectManager
{
private:
	list<CGameObject*>	m_DontDestroyObj;

public:
	void AddDontDestroyObj(class CGameObject* pObj);
	void ChangeScene(class CScene* pScene);
	bool CheckDontDestroyObj(const string& strTag);
	class CGameObject* FindDontDestroyObj(const string& strTag);

public:
	bool Init();

	DECLARE_SINGLE(CObjectManager)
};
ENGINE_END