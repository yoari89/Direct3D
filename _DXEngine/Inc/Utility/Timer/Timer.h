#pragma once

ENGINE_BEGIN
class CTimerManager;
class ENGINE_DLL CTimer
{
public:
	CTimer();
	~CTimer();

private:
	friend CTimerManager;

private:
	LARGE_INTEGER	m_tSecond;
	LARGE_INTEGER	m_tTick;
	float			m_fTime;
	float			m_fFPS;
	float			m_fFPSTime;
	int				m_iTick;
	float			m_fTimeScale;
	string			m_strName;

public:
	float GetTime() const;
	float GetFPS()	const;

public:
	void SetTimeScale(float _fTimeScale);

public:
	bool Init();
	void Update();
};
ENGINE_END