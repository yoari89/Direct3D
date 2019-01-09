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
	// ������ �ڵ�
	HANDLE m_hThread;
	HANDLE m_hStart;

public:
	bool Init();
	void Wait();
	void Awake();

	// ���� �����尡 ó���ϴ� �κ��̸�,
	// > �����Ǵ� ������� Run �Լ��� �ݵ�� ������ �ؾ��Ѵ�.
	virtual void Run() = 0;

public:
	void SetThreadNullInit();
	HANDLE GetThread() const;

public:
	static unsigned int __stdcall ThreadFunc(void* _pArg);
};
ENGINE_END