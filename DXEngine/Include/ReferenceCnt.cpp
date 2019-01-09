#include "PCHEngine.h"
#include "ReferenceCnt.h"

ENGINE_USING

CReferenceCnt::CReferenceCnt() :
	m_iRefCount(1),
	m_isActive(true),
	m_isEnable(true),
	m_strTag("")
{
}

CReferenceCnt::~CReferenceCnt()
{
}

int CReferenceCnt::AddRef()
{
	++m_iRefCount;
	return m_iRefCount;
}

int CReferenceCnt::Release()
{
	--m_iRefCount;
	if (m_iRefCount == 0)
	{
		// RefCount가 0이되면 객체 삭제
		delete this;
		return 0;
	}
	return m_iRefCount;
}

void CReferenceCnt::SetTag(const string& _strTag)
{
	m_strTag = _strTag;
}

string CReferenceCnt::GetTag() const
{
	return m_strTag;
}

void CReferenceCnt::Die()
{
	m_isActive = false;
}

bool CReferenceCnt::GetActive() const
{
	return m_isActive;
}

void CReferenceCnt::SetEnable(bool _isEnable)
{
	m_isEnable = _isEnable;
}

bool CReferenceCnt::GetEnable() const
{
	return m_isEnable;
}