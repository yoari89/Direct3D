#include "PCHEngine.h"
#include "ThreadManager.h"
#include "Thread.h"

ENGINE_USING
DEFINITION_SINGLE(CThreadManager);

CThreadManager::CThreadManager()
{
}

CThreadManager::~CThreadManager()
{
	// 동기화
	unordered_map<string, CRITICAL_SECTION>::iterator iter;
	unordered_map<string, CRITICAL_SECTION>::iterator iterEnd = m_mapCriticalSection.end();
	for (iter = m_mapCriticalSection.begin(); iter != iterEnd; ++iter)
	{
		DeleteCriticalSection(&iter->second);
	}
	m_mapCriticalSection.clear();

	// 쓰레드
	Safe_Delete_Map(m_mapThread);
}

bool CThreadManager::Init()
{
	return true;
}

bool CThreadManager::DeleteThread(const string& _strName)
{
	unordered_map<string, CThread*>::iterator iter = m_mapThread.find(_strName);
	if (iter == m_mapThread.end())
	{
		return false;
	}
	SAFE_DELETE(iter->second);
	m_mapThread.erase(iter);

	return true;
}

CThread* CThreadManager::FindThread(const string& _strName)
{
	unordered_map<string, CThread*>::iterator iter = m_mapThread.find(_strName);
	if (iter == m_mapThread.end())
	{
		return nullptr;
	}

	return iter->second;
}

bool CThreadManager::CreateCriticalSection(const string& _strName)
{
	if (FindCriticalSection(_strName) != nullptr)
	{
		return false;
	}

	// 생성
	CRITICAL_SECTION tCriScction;
	InitializeCriticalSection(&tCriScction);
	
	// 추가
	m_mapCriticalSection.insert(make_pair(_strName, tCriScction));

	return true;
}

CRITICAL_SECTION * CThreadManager::GetLoadSafe()
{
	return FindCriticalSection(m_strLoadSafeKey);
}

CRITICAL_SECTION* CThreadManager::FindCriticalSection(const string& _strName, bool bLoadSafe)
{
	unordered_map<string, CRITICAL_SECTION>::iterator iter = m_mapCriticalSection.find(_strName);
	if (iter == m_mapCriticalSection.end())
	{
		return nullptr;
	}

	if (bLoadSafe)
		m_strLoadSafeKey = _strName;

	return &iter->second;
}