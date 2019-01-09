#pragma once

ENGINE_BEGIN
class ENGINE_DLL CReferenceCnt
{
protected:
	CReferenceCnt();
	virtual ~CReferenceCnt() = 0;

protected:
	int		m_iRefCount;
	bool	m_isActive;
	bool	m_isEnable;
	string	m_strTag;

/* 레퍼런스 카운트 */
public:
	int AddRef();
	int Release();

/* 태그 */
public:
	void   SetTag(const string& _strTag);
	string GetTag()   const;

/* 상태  */
public:
	void   Die();
	bool   GetActive() const;
	void   SetEnable(bool _isEnable);
	bool   GetEnable() const;
};
ENGINE_END