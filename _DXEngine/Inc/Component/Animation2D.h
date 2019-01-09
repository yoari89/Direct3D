#pragma once
#include "Utility/Types.h"
#include "Component.h"

ENGINE_BEGIN
class CRenderer;
class ENGINE_DLL CAnimation2D : public CComponent
{
private:
	friend class CGameObject;

public:
	CAnimation2D();
	CAnimation2D(const CAnimation2D& _anim);
	~CAnimation2D();

private:
	string									m_strDefaultClip;
	CRenderer*								m_pRenderer;
	unordered_map<string, PAnimationClip2D> m_mapClip;
	PAnimationClip2D						m_pCurClip;

// 상속 받은 재정의 함수
public:
	void Start()					override;
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CAnimation2D* Clone()			override;

// 애니메이션
private:
	bool m_isAniPlaying;
	int  m_frameIndex;

public:
	bool IsAniPlaying();
	void ChangeClip(const string& _strName);
	void SetDefaultClip(const string& _strName);
	
	// 클립 추가 (단일 파일경로)
	// > 고정 속도
	void AddFixedClip(const string& _aniName, const string& _textureKey, const wchar_t* _pFileName, 
					  float _fixedTime, const vector<Clip2DFrame>& _vecFrame,
					  ANIMATION2D_TYPE _eType, ANIMATION_OPTION _eOption, const string& _strPathKey = TEXTURE_PATH);

	// > 프레임별 속도
	void AddFrameTimeClip(const string& _aniName, const string& _textureKey, const wchar_t* _pFileName,
						  vector<float> _vecFrameTime, const vector<Clip2DFrame>& _vecFrame,
						  ANIMATION2D_TYPE _eType, ANIMATION_OPTION _eOption, const string& _strPathKey = TEXTURE_PATH);

	// > 클립 추가 (다중 파일경로)
	void AddClip(const string& _strName, ANIMATION2D_TYPE _eType, ANIMATION_OPTION _eOption,
				 float _fPlayLimitTime, const vector<Clip2DFrame>& _vecFrame,
				 const string& _strTexKey, const vector<const TCHAR*>& _vecFileName,
				 const string& _strPathKey = TEXTURE_PATH);

public:
	PAnimationClip2D FindClip(const string& _strName);
};
ENGINE_END