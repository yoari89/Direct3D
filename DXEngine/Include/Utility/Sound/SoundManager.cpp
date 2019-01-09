#include "PCHEngine.h"
#include "SoundManager.h"
#include "Utility/Path/PathManager.h"
#include "Scene/Scene.h"

ENGINE_USING
DEFINITION_SINGLE(CSoundManager)

CSoundManager::CSoundManager()
{
	memset(m_pChannel, 0, sizeof(Channel*) * 10);
}

CSoundManager::~CSoundManager()
{
	unordered_map<string, PSOUNDINFO>::iterator	iter;
	unordered_map<string, PSOUNDINFO>::iterator	iterEnd = m_mapSound.end();
	for (iter = m_mapSound.begin(); iter != iterEnd; ++iter)
	{
		iter->second->pSound->release();
		SAFE_DELETE(iter->second);
	}
	m_pSystem->close();
	m_pSystem->release();
}

bool CSoundManager::Init()
{
	System_Create(&m_pSystem);
	m_pSystem->init(10, FMOD_INIT_NORMAL, nullptr);
	return true;
}

void CSoundManager::Update()
{
	for (size_t i = 1; i < 10; ++i)
	{
		if (m_isChanel[i] != false)
		{
			bool isPlay = false;
			m_pChannel[i]->isPlaying(&isPlay);
			if (!isPlay)
			{
				m_pChannel[i] = nullptr;
				m_isChanel[i-1] = false;
			}
		}
	}
}

bool CSoundManager::LoadSound(CScene* _pScene, bool _isLoop,
							  const char* _pFileName, const string& _strPathKey)
{
	PSOUNDINFO pSound = FindSound(_pFileName);
	if (pSound)
	{
		return false;
	}

	const char*	pPath = GET_SINGLE(CPathManager)->FindPathFromMultibyte(_strPathKey);
	string strFullPath;
	if (pPath != nullptr)
	{
		strFullPath = pPath;
	}
	strFullPath += _pFileName;

	// 사운드 생성
	pSound = new SOUNDINFO;
	if (_pScene != nullptr)
	{
		pSound->pScene = _pScene;
	}
	pSound->isLoop = _isLoop;

	FMOD_MODE eMode = FMOD_LOOP_NORMAL;
	if (_isLoop == false)
	{
		eMode = FMOD_DEFAULT;
	}
	m_pSystem->createSound(strFullPath.c_str(), eMode, NULL, &pSound->pSound);

	// 추가
	m_mapSound.insert(make_pair(_pFileName, pSound));

	return true;
}

void CSoundManager::Play(const string& _strKey, bool _isBGM)
{
	PSOUNDINFO pSound = FindSound(_strKey);
	if (pSound == nullptr)
	{
		return;
	}

	// 재생
	if (_isBGM == true)
	{
		pSound->iChanel = 0;
		m_pSystem->playSound(pSound->pSound, nullptr, false, &m_pChannel[SOUNDTYPE_BGM]);
	}
	else
	{
		int iChanel = -1;
		for (size_t i = 1; i < 10; ++i)
		{
			if (m_isChanel[i] == false)
			{
				iChanel = i + 1;
				m_isChanel[i] = true;
				break;
			}
		}
		if (iChanel == -1)
		{
			iChanel = 1;
		}

		m_pSystem->playSound(pSound->pSound, NULL, false, &m_pChannel[iChanel]);
		pSound->iChanel = iChanel;
	}
}

void CSoundManager::Stop(const string& _strKey, bool _isBGM)
{
	PSOUNDINFO pSound = FindSound(_strKey);
	if (pSound == nullptr)
	{
		return;
	}

	m_pChannel[pSound->iChanel]->stop();
	m_isChanel[pSound->iChanel - 1] = false;
	m_pChannel[pSound->iChanel] = nullptr;


	/*if (_isBGM)
	{
		m_pChannel[SOUNDTYPE_BGM]->stop();
	}
	else
	{
		m_pChannel[SOUNDTYPE_EFFECT]->stop();
	}*/
}

void CSoundManager::SoundAllStop()
{
	for (int i = 0; i < SOUNDTYPE_MAX; ++i)
	{
		m_pChannel[i]->stop();
	}
}

void CSoundManager::Pause(const string& _strKey, bool _isBGM)
{
	PSOUNDINFO pSound = FindSound(_strKey);
	if (pSound == nullptr)
	{
		return;
	}

	if (_isBGM)
	{
		m_pChannel[SOUNDTYPE_BGM]->setPaused(false);
	}
	else
	{
		m_pSystem->playSound(pSound->pSound, NULL, false, &m_pChannel[SOUNDTYPE_EFFECT]);
	}
}

void CSoundManager::Pause()
{
	ChannelGroup* pGroup;
	m_pSystem->getMasterChannelGroup(&pGroup);
	pGroup->setPaused(true);
}

void CSoundManager::Resume()
{
	ChannelGroup* pGroup;
	m_pSystem->getMasterChannelGroup(&pGroup);
	pGroup->setPaused(false);
}

void CSoundManager::SetMasterVolume(float _fVolume)
{
	ChannelGroup* pGroup;
	m_pSystem->getMasterChannelGroup(&pGroup);
	pGroup->setVolume(_fVolume);
}

void CSoundManager::SetVolume(float _fVolume, bool _isbBGM)
{
	if (_isbBGM)
	{
		m_pChannel[SOUNDTYPE_BGM]->setVolume(_fVolume);
	}
	else
	{
		m_pChannel[SOUNDTYPE_EFFECT]->setVolume(_fVolume);
	}
}

void CSoundManager::DeleteSound()
{
	unordered_map<string, PSOUNDINFO>::iterator	iter;
	unordered_map<string, PSOUNDINFO>::iterator	iterEnd = m_mapSound.end();
	for (iter = m_mapSound.begin(); iter != iterEnd; ++iter)
	{
		iter->second->pSound->release();
		SAFE_DELETE(iter->second);
	}
	m_mapSound.clear();
}

void CSoundManager::DeleteSound(CScene* _pScene)
{
	unordered_map<string, PSOUNDINFO>::iterator	iter;
	unordered_map<string, PSOUNDINFO>::iterator	iterEnd = m_mapSound.end();
	for (iter = m_mapSound.begin(); iter != iterEnd;)
	{
		if (iter->second->pScene == _pScene)
		{
			iter->second->pSound->release();
			SAFE_DELETE(iter->second);
			iter = m_mapSound.erase(iter);
			iterEnd = m_mapSound.end();
		}
		else
		{
			++iter;
		}
	}
	for (int i = 0; i < 2; ++i)
	{
		m_pChannel[i]->stop();
	}
}

void CSoundManager::DeleteSound(const char* _pFileName)
{
	unordered_map<string, PSOUNDINFO>::iterator	iter = m_mapSound.find(_pFileName);
	if (iter == m_mapSound.end())
	{
		return;
	}
	iter->second->pSound->release();
	SAFE_DELETE(iter->second);
	m_mapSound.erase(iter);
}

bool CSoundManager::IsBGMPlaying()
{
	bool _isPlaying = false;
	m_pChannel[SOUNDTYPE_BGM]->isPlaying(&_isPlaying);
	return _isPlaying;
}

CSoundManager::PSOUNDINFO CSoundManager::FindSound(const string& _strKey)
{
	unordered_map<string, PSOUNDINFO>::iterator	iter = m_mapSound.find(_strKey);
	if (iter == m_mapSound.end())
	{
		return nullptr;
	}

	return iter->second;
}