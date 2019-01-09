#include "PCHEngine.h"
#include "Timer.h"
#include "Core.h"

ENGINE_USING

CTimer::CTimer()
{
}

CTimer::~CTimer()
{
}

float CTimer::GetTime() const
{
	return m_fTime * m_fTimeScale;
}

float CTimer::GetFPS() const
{
	return m_fFPS;
}

void CTimer::SetTimeScale(float fTimeScale)
{
	m_fTimeScale = fTimeScale;
}

bool CTimer::Init()
{
	QueryPerformanceFrequency(&m_tSecond);
	QueryPerformanceCounter(&m_tTick);
	
	m_fTime		 = 0.f;
	m_fFPS		 = 0.f;
	m_fFPSTime	 = 0.f;
	m_iTick		 = 0;
	m_fTimeScale = 1.f;

	return true;
}

void CTimer::Update()
{
	LARGE_INTEGER tTick;
	QueryPerformanceCounter(&tTick);
	m_fTime = (tTick.QuadPart - m_tTick.QuadPart) / (float)m_tSecond.QuadPart;
	m_tTick = tTick;
	
	m_fFPSTime += m_fTime;
	++m_iTick;
	if (m_iTick == 60)
	{
		m_fFPS = m_iTick / m_fFPSTime;
		char Buffer[128] = {};
		sprintf_s(Buffer , "FPS : %f", m_fFPS);
		SetWindowTextA(WINDOWHANDLE, Buffer);
		m_fFPSTime = 0.f;
		m_iTick = 0;
	}
}