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

/* ���۷��� ī��Ʈ */
public:
	int AddRef();
	int Release();

/* �±� */
public:
	void   SetTag(const string& _strTag);
	string GetTag()   const;

/* ����  */
public:
	void   Die();
	bool   GetActive() const;
	void   SetEnable(bool _isEnable);
	bool   GetEnable() const;
};
ENGINE_END