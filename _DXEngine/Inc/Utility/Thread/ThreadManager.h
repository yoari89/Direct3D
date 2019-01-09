#pragma once

ENGINE_BEGIN
class CThread;
class ENGINE_DLL CThreadManager
{
DECLARE_SINGLE(CThreadManager);

private:
	unordered_map<string, CThread*> m_mapThread;
	unordered_map<string, CRITICAL_SECTION> m_mapCriticalSection; // ����ȭ
	string m_strLoadSafeKey;

/* ������ ��� */
public:
	bool Init();
	bool DeleteThread(const string& _strName);
	CThread* FindThread(const string& _strName);

/* ����ȭ */
public:
	bool CreateCriticalSection(const string& _strName);
	CRITICAL_SECTION* FindCriticalSection(const string& _strName, bool bLoadSafe = false);
	CRITICAL_SECTION* GetLoadSafe();

public:
	template <typename T>
	T* Create(const string& strName)
	{
		T*	pThread = (T*)FindThread(strName);
		if (pThread != nullptr)
		{
			return pThread;
		}

		// ����
		pThread = new T();
		if(pThread->Init() == false)
		{
			SAFE_DELETE(pThread);
			return nullptr;
		}

		// �߰�
		m_mapThread.insert(make_pair(strName, pThread));
		return pThread;
	}
};

ENGINE_END