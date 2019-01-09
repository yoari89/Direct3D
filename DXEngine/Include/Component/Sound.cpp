#include "PCHEngine.h"
#include "Sound.h"
#include "Utility/Sound/SoundManager.h"

ENGINE_USING

CSound::CSound() :
	m_isBGM(false),
	m_isPlay(false)
{
	SetTag("Sound");
	m_eComType = CT_SOUND;
}

CSound::CSound(const CSound& _sound) : 
	CComponent(_sound)
{
	*this = _sound;
}

CSound::~CSound()
{
}

bool CSound::Init()
{
	return true;
}

int CSound::Input(float _fTime)
{
	return 0;
}

int CSound::Update(float _fTime)
{
	return 0;
}

int CSound::LateUpdate(float _fTime)
{
	return 0;
}

void CSound::Collision(float _fTime)
{
}

void CSound::Render(float _fTime)
{
}

void CSound::AfterClone()
{
}

CSound* CSound::Clone()
{
	return new CSound(*this);
}

void CSound::SetSound(const string& _strFileName, bool _isLoop, const string& _strPathKey)
{
	m_strFileName = _strFileName;
	m_isLoop = _isLoop;
	GET_SINGLE(CSoundManager)->LoadSound(m_pScene, m_isLoop, m_strFileName.c_str(), _strPathKey);
}

void CSound::EnableBGM()
{
	m_isBGM = true;
}

void CSound::Play()
{
	GET_SINGLE(CSoundManager)->Play(m_strFileName, m_isBGM);
}

void CSound::Stop()
{
	GET_SINGLE(CSoundManager)->Stop(m_strFileName, m_isBGM);
}

void CSound::Pause()
{
}

bool CSound::IsBGMPlaying()
{
	return GET_SINGLE(CSoundManager)->IsBGMPlaying();
}
