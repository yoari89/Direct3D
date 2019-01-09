#pragma once

ENGINE_BEGIN
class ENGINE_DLL CPathManager
{
// ΩÃ±€≈Ê ∏≈≈©∑Œ
DECLARE_SINGLE(CPathManager)

private:
	unordered_map<string, wstring> m_mapPath;
	char m_strPath[MAX_PATH] = {};

public:
	bool Init();
	bool AddPath(const string& _strKey, const wstring& _strPath,
				 const string& _strBaseKey = ROOT_PATH);
	const wchar_t* FindPath(const string& _strKey);
	const char* FindPathFromMultibyte(const string& _strKey);
};
ENGINE_END