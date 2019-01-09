#pragma once

ENGINE_BEGIN
class ENGINE_DLL CThread
{
private:
	friend class CThreadManager;

protected:
	CThread();

public:
	virtual ~CThread() = 0;

private:
	// 쓰레드 핸들
	HANDLE m_hThread;
	HANDLE m_hStart;

public:
	bool Init();
	void Wait();
	void Awake();

	// 실제 쓰레드가 처리하는 부분이며,
	// > 생성되는 쓰레드는 Run 함수를 반드시 재정의 해야한다.
	virtual void Run() = 0;

public:
	void SetThreadNullInit();
	HANDLE GetThread() const;

public:
	static unsigned int __stdcall ThreadFunc(void* _pArg);
};
ENGINE_END