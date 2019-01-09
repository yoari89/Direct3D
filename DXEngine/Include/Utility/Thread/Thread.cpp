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
	// 이벤트 생성 (커널 동기화 설정)
	m_hStart = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	
	// 쓰레드 핸들 등록
	// > _beginthreadex_proc_type 함수 호출 규약(__stdcall)
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, CThread::ThreadFunc, this, 0, nullptr);
	return true;
}

void CThread::Wait()
{
	// 지정된 개체가 신호를 받거나 제한 시간이 경과 할 때까지 대기한다.
	// > 신호(시그널)을 받을때까지 INFINITE(무한정) 대기
	WaitForSingleObject(m_hStart, INFINITE);
}

void CThread::Awake()
{
	// 지정된 이벤트 객체를 신호 상태(시그널 상태)로 설정한다.
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
	// _pArg 쓰레드를 생성한 객체
	// > Up Casting
	CThread* pThread = (CThread*)_pArg;
	
	// 신호를 기다린다.
	pThread->Wait();

	// 신호가 들어왔다면 쓰레드 실행
	pThread->Run();

	return 0;
}