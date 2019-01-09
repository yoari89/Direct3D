#pragma once
#include "Component/Component.h"

ENGINE_BEGIN
class ENGINE_DLL CSound : public CComponent
{
private:
	friend class CGameObject;

public:
	CSound();
	CSound(const CSound& _sound);
	virtual ~CSound();

/* 컴포넌트 클래스 가상함수 재정의 */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CSound* Clone()					override;

private:
	bool	m_isBGM;
	bool	m_isLoop;
	bool	m_isPlay;
	string	m_strFileName;

public:
	void SetSound(const string& _strFileName, bool _isLoop,
				  const string& _strPathKey = SOUND_PATH);
	void EnableBGM();
	void Play();
	void Stop();
	void Pause();
	bool IsBGMPlaying();
};
ENGINE_END