#include "PCHEngine.h"
#include "Thread.h"

ENGINE_USING

CThread::CThread() :
	m_hThread(NULL),
	m_hStart(NULL)
{
}

CThread::~CThread()
{
	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
	}
}

bool CThread::Init()
{
	// �̺�Ʈ ���� (Ŀ�� ����ȭ ����)
	m_hStart = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	
	// ������ �ڵ� ���
	// > _beginthreadex_proc_type �Լ� ȣ�� �Ծ�(__stdcall)
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, CThread::ThreadFunc, this, 0, nullptr);
	return true;
}

void CThread::Wait()
{
	// ������ ��ü�� ��ȣ�� �ްų� ���� �ð��� ��� �� ������ ����Ѵ�.
	// > ��ȣ(�ñ׳�)�� ���������� INFINITE(������) ���
	WaitForSingleObject(m_hStart, INFINITE);
}

void CThread::Awake()
{
	// ������ �̺�Ʈ ��ü�� ��ȣ ����(�ñ׳� ����)�� �����Ѵ�.
	SetEvent(m_hStart);
}

void CThread::SetThreadNullInit()
{
	m_hThread = NULL;
}

HANDLE CThread::GetThread() const
{
	return m_hThread;
}

unsigned int CThread::ThreadFunc(void* _pArg)
{
	// _pArg �����带 ������ ��ü
	// > Up Casting
	CThread* pThread = (CThread*)_pArg;
	
	// ��ȣ�� ��ٸ���.
	pThread->Wait();

	// ��ȣ�� ���Դٸ� ������ ����
	pThread->Run();

	return 0;
}