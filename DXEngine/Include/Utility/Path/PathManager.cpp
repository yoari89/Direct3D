#include "PCHEngine.h"
#include "PathManager.h"

ENGINE_USING
DEFINITION_SINGLE(CPathManager)

CPathManager::CPathManager()
{
}

CPathManager::~CPathManager()
{
}

bool CPathManager::Init()
{
	TCHAR strPath[MAX_PATH] = {};
	GetModuleFileName(nullptr, strPath, MAX_PATH);
	for (int i = lstrlen(strPath) - 1; i >= 0; --i)
	{
		if (strPath[i] == '\\' || strPath[i] == '/')
		{
			memset(strPath + (i + 1), 0, sizeof(TCHAR) * (MAX_PATH - (i + 1)));
			break;
		}
	}
	m_mapPath.insert(make_pair(ROOT_PATH, strPath));

	// 경로 추가
	AddPath(SHADER_PATH,	TEXT("Shader\\"));
	AddPath(TEXTURE_PATH,	TEXT("Texture\\"));
	AddPath(SOUND_PATH,		TEXT("Sounds\\"));

	return true;
}

bool CPathManager::AddPath(const string& _strKey, const wstring& _strPath, const string& _strBaseKey)
{
	if (FindPath(_strKey))
	{
		return false;
	}
	
	const TCHAR* pPath = FindPath(_strBaseKey);
	wstring	strFullPath;
	if (pPath != nullptr)
	{
		strFullPath = pPath;
	}
	strFullPath += _strPath;
	m_mapPath.insert(make_pair(_strKey, strFullPath));
	
	return true;
}

const wchar_t* CPathManager::FindPath(const string& _strKey)
{
	unordered_map<string, wstring>::iterator iter = m_mapPath.find(_strKey);
	if (iter == m_mapPath.end())
	{
		return nullptr;
	}

	return iter->second.c_str();
}

const char* CPathManager::FindPathFromMultibyte(const string& _strKey)
{
	const TCHAR* pPath = FindPath(_strKey);
	if (pPath == nullptr)
	{
		return nullptr;
	}
	memset(m_strPath, 0, sizeof(char) * MAX_PATH);

	// 유니코드 문자열을 멀티바이트로 변환한다.
#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0, pPath, -1, m_strPath, lstrlen(pPath) * 2, 0, 0);
#else
	memcpy(m_strPath, pPath, sizeof(TCHAR) * lstrlen(pPath));
#endif	// UNICODE

	return m_strPath;
}