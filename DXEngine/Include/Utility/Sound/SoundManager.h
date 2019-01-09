#pragma once
#include "fmod.hpp"

using namespace FMOD;

#ifdef _WIN64
#pragma comment(lib, "fmod64_vc")
#else
#pragma comment(lib, "fmod_vc")
#endif

ENGINE_BEGIN
class CScene;
class ENGINE_DLL CSoundManager
{
private:
	typedef struct _tagSoundInfo
	{
		bool	isLoop;
		int     iChanel;
		Sound*	pSound;
		CScene*	pScene;
	}SOUNDINFO, *PSOUNDINFO;

private:
	System*	 m_pSystem;
	Channel* m_pChannel[10];
	bool m_isChanel[10];
	unordered_map<string, PSOUNDINFO> m_mapSound;

public:
	bool Init();
	void Update();
	bool LoadSound(CScene* _pScene, bool _isLoop, const char* _pFileName,
				   const string& _strPathKey = SOUND_PATH);
	void Play(const string&  _strKey, bool _isBGM = false);
	void Stop(const string&  _strKey, bool _isBGM = false);
	void SoundAllStop();
	void Pause(const string& _strKey, bool _isBGM = false);
	void Pause();
	void Resume();
	void SetMasterVolume(float _fVolume);
	void SetVolume(float _fVolume, bool _isbBGM = false);
	void DeleteSound();
	void DeleteSound(class CScene* _pScene);
	void DeleteSound(const char* _pFileName);
	bool IsBGMPlaying();

private:
	PSOUNDINFO FindSound(const string& _strKey);

private:
	DECLARE_SINGLE(CSoundManager)
};
ENGINE_END