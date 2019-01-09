#include "PCHEngine.h"
#include "TimerManager.h"
#include "Timer.h"

ENGINE_USING
DEFINITION_SINGLE(CTimerManager)

CTimerManager::CTimerManager()
{
}

CTimerManager::~CTimerManager()
{
	unordered_map<string, CTimer*>::iterator iter;
	unordered_map<string, CTimer*>::iterator iterEnd = m_mapTimer.end();
	for (iter = m_mapTimer.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}
	m_mapTimer.clear();
}

bool CTimerManager::Init()
{
	CreateTimer("MainTimer");
	return true;
}

bool CTimerManager::CreateTimer(const string& _strName)
{
	CTimer*	pTimer = FindTimer(_strName);
	if (pTimer)
	{
		return false;
	}
	
	// 클래스 생성
	pTimer = new CTimer;
	pTimer->m_strName = _strName;
	pTimer->Init();
	m_mapTimer.insert(make_pair(_strName, pTimer));
	
	return true;
}

CTimer* CTimerManager::FindTimer(const string& _strKey)
{
	unordered_map<string, CTimer*>::iterator iter = m_mapTimer.find(_strKey);
	if (iter == m_mapTimer.end())
	{
		return nullptr;
	}
	return iter->second;
}