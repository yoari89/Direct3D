#pragma once

ENGINE_BEGIN
class CSync
{
/* ����ȭ Lock �۾� */
// > DeadLock(�����) ���� ����
public:
	CSync(CRITICAL_SECTION* _pCriSection)
	{
		m_pCriSection = _pCriSection;
		
		// Lock ����
		EnterCriticalSection(_pCriSection);
	}
	~CSync()
	{
		// Lock ����
		LeaveCriticalSection(m_pCriSection);
	}

private:
	CRITICAL_SECTION* m_pCriSection;
};
ENGINE_END