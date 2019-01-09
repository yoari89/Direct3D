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

// ��� ���� ������ �Լ�
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

// �ִϸ��̼�
private:
	bool m_isAniPlaying;
	int  m_frameIndex;

public:
	bool IsAniPlaying();
	void ChangeClip(const string& _strName);
	void SetDefaultClip(const string& _strName);
	
	// Ŭ�� �߰� (���� ���ϰ��)
	// > ���� �ӵ�
	void AddFixedClip(const string& _aniName, const string& _textureKey, const wchar_t* _pFileName, 
					  float _fixedTime, const vector<Clip2DFrame>& _vecFrame,
					  ANIMATION2D_TYPE _eType, ANIMATION_OPTION _eOption, const string& _strPathKey = TEXTURE_PATH);

	// > �����Ӻ� �ӵ�
	void AddFrameTimeClip(const string& _aniName, const string& _textureKey, const wchar_t* _pFileName,
						  vector<float> _vecFrameTime, const vector<Clip2DFrame>& _vecFrame,
						  ANIMATION2D_TYPE _eType, ANIMATION_OPTION _eOption, const string& _strPathKey = TEXTURE_PATH);

	// > Ŭ�� �߰� (���� ���ϰ��)
	void AddClip(const string& _strName, ANIMATION2D_TYPE _eType, ANIMATION_OPTION _eOption,
				 float _fPlayLimitTime, const vector<Clip2DFrame>& _vecFrame,
				 const string& _strTexKey, const vector<const TCHAR*>& _vecFileName,
				 const string& _strPathKey = TEXTURE_PATH);

public:
	PAnimationClip2D FindClip(const string& _strName);
};
ENGINE_END