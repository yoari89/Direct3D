#pragma once

ENGINE_BEGIN
class CTimer;
class ENGINE_DLL CTimerManager
{
DECLARE_SINGLE(CTimerManager)

private:
	unordered_map<string, CTimer*> m_mapTimer;

public:
	bool Init();
	bool CreateTimer(const string& _strName);
	CTimer* FindTimer(const string& _strKey);
};
ENGINE_END