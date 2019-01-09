#pragma once

ENGINE_BEGIN
class CSync
{
/* 동기화 Lock 작업 */
// > DeadLock(데드락) 현상 방지
public:
	CSync(CRITICAL_SECTION* _pCriSection)
	{
		m_pCriSection = _pCriSection;
		
		// Lock 설정
		EnterCriticalSection(_pCriSection);
	}
	~CSync()
	{
		// Lock 해제
		LeaveCriticalSection(m_pCriSection);
	}

private:
	CRITICAL_SECTION* m_pCriSection;
};
ENGINE_END