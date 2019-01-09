#include "PCHEngine.h"
#include "Animation2D.h"
#include "Component/Renderer.h"
#include "Component/Material.h"
#include "Object/GameObject.h"
#include "Utility/Path/PathManager.h"
#include "Resources/Texture.h"
#include "Resources/ResourcesManager.h"

ENGINE_USING

CAnimation2D::CAnimation2D() :
	m_frameIndex(0),
	m_isAniPlaying(false),
	m_pRenderer(nullptr),
	m_pCurClip(nullptr)
{
	m_eComType = CT_ANIMATION2D;
}

CAnimation2D::CAnimation2D(const CAnimation2D& _anim) :
	CComponent(_anim)
{
	// 기본 애니메이션 이름 복사
	m_strDefaultClip = _anim.m_strDefaultClip;

	// 애니메이션 클립 복사
	m_mapClip.clear();
	unordered_map<string, PAnimationClip2D>::const_iterator	iter;
	unordered_map<string, PAnimationClip2D>::const_iterator	iterEnd = _anim.m_mapClip.end();
	for (iter = _anim.m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		PAnimationClip2D pClip = new AnimationClip2D;
		*pClip = *(iter->second);  // 포인터 역참조..(값 복사) 
		pClip->pTexture->AddRef(); // 같은 Texture* 공간을 가르키며, 
								   // 원본이 삭제될 때 '댕글링 포인터'를 방지하기 위해 RefCount를 증가시킨다.

		// 추가
		m_mapClip.insert(make_pair(iter->first, pClip));

		// 현재 클립 설정
		if (_anim.m_pCurClip->strName == iter->first)
		{
			m_pCurClip = pClip;
		}
	}
}

CAnimation2D::~CAnimation2D()
{
	// PAnimationClip2D 메모리 해제
	unordered_map<string, PAnimationClip2D>::iterator iter;
	unordered_map<string, PAnimationClip2D>::iterator iterEnd = m_mapClip.end();
	for (iter = m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second->pTexture);
		SAFE_DELETE(iter->second);	
	}
	m_mapClip.clear();
}

void CAnimation2D::Start()
{
	CMaterial* pMaterial = FindComponentFromType<CMaterial>(CT_MATERIAL);
	if (pMaterial != nullptr)
	{
		pMaterial->SetDiffuseTex(m_pCurClip->iTexRegister, m_pCurClip->pTexture);
		SAFE_RELEASE(pMaterial);
	}
}

bool CAnimation2D::Init()
{
	return true;
}

int CAnimation2D::Input(float _fTime)
{
	return 0;
}

int CAnimation2D::Update(float _fTime)
{
	return 0;
}

int CAnimation2D::LateUpdate(float _fTime)
{
	m_pCurClip->fPlayTime += _fTime;

	// 고정 시간
	float frameTime = 0.f;
	if(m_pCurClip->vecFrameTime.size() == 0)
	{
		// 예) PlayLimitTime(1초)동안 15장의 텍스처를 변경해야하는 경우
		//     1/60 = 0.00666초가 나온다. 즉 0.00666초마다 한장씩 텍스처가 변경되어야한다.
		frameTime = m_pCurClip->fPlayFixedTime / m_pCurClip->vecFrame.size();
	}
	// 프레임별 속도
	else
	{
		frameTime = m_pCurClip->vecFrameTime[m_frameIndex];
	}
	
	while (m_pCurClip->fPlayTime >= frameTime)
	{
		m_pCurClip->fPlayTime -= frameTime;
		++m_pCurClip->iFrame;
		if (m_pCurClip->vecFrameTime.size() > 0)
		{
			++m_frameIndex;
		}

		// 애니메이션 종료
		if (m_pCurClip->iFrame >= (int)m_pCurClip->vecFrame.size())
		{
			m_frameIndex		= 0;
			m_pCurClip->iFrame = 0;
			m_isAniPlaying		= false;
			m_pCurClip->isAniPlaying = false;
			ANIMATION_OPTION aniOption = m_pCurClip->eOption;
			switch (aniOption)
			{ 
				case AO_LOOP:
				{
					// 애니메이션 반복
					break;
				}
				case AO_DISABLE:
				{
					m_pObject->SetEnable(false);
					break;
				}
				case AO_ONCE_DESTROY:
				{
					// 애니메이션 종료 후 애니메이션을 재생하는 오브젝트 삭제
					m_pObject->Die();
					break;
				}
				case AO_ONCE_RETURN:
				{
					// 기본으로 설정된 애니메이션 처리
					m_pCurClip = FindClip(m_strDefaultClip);
					break;
				}
			}
			
		}
	}

	// Shader 단계에서 텍스처를 처리하기 위해 Clip2DFrame구조체(UV) 값을 채워넣고
	// "Animation2D" Register Slot Number(b8) 상수 버퍼를 갱신한다.
	CRenderer* pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);
	if (pRenderer)
	{
		Animation2DCBuffer tCBuffer = {};
		tCBuffer.iFrame = m_pCurClip->iFrame;
		pRenderer->UpdateAnimationType(m_pCurClip->eType);

		// Atlas
		if (m_pCurClip->eType == A2D_ATLAS)
		{
			// vecFrame 
			// > Uv (V2 LeftTop), (V2 RightBottom) 값을 설정할 수 있는 구조체
			// > 이미지의 현재 프레임의 Uv LeftTop, RightBottom 설정
			Clip2DFrame& tFrame = m_pCurClip->vecFrame[m_pCurClip->iFrame];
			tCBuffer.vUVLT.x = tFrame.vLT.x / m_pCurClip->fTextureWidth;
			tCBuffer.vUVLT.y = tFrame.vLT.y / m_pCurClip->fTextureHeight;
			tCBuffer.vUVRB.x = tFrame.vRB.x / m_pCurClip->fTextureWidth;
			tCBuffer.vUVRB.y = tFrame.vRB.y / m_pCurClip->fTextureHeight;
		}
		// Frame
		else
		{
			tCBuffer.vUVLT = Vector2(0.f, 0.f);
			tCBuffer.vUVRB = Vector2(1.f, 1.f);
		}

		// 상수 버퍼 갱신
		pRenderer->UpdateRendererCBuffer("Animation2D", &tCBuffer, sizeof(Animation2DCBuffer));
		SAFE_RELEASE(pRenderer);
	}

	return 0;
}

void CAnimation2D::Collision(float _fTime)
{
}

void CAnimation2D::Render(float _fTime)
{
}

void CAnimation2D::AfterClone()
{
}

CAnimation2D* CAnimation2D::Clone()
{
	return new CAnimation2D(*this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CAnimation2D::IsAniPlaying()
{
	return m_isAniPlaying;
}

void CAnimation2D::ChangeClip(const string& _strName)
{
	// 애니메이션 시작
	m_isAniPlaying = true;
	m_frameIndex = 0;

	// 현재 애니메이션과 변경될 애니메이션이 같은 경우
	if (m_pCurClip->strName == _strName)
	{
		return;
	}
	
	// 찾기
	// > PAnimationClip2D 프레임, 플레이 타임 초기화
	m_pCurClip = FindClip(_strName);
	m_pCurClip->isAniPlaying = true;
	m_pCurClip->iFrame	  = 0;
	m_pCurClip->fPlayTime = 0.f;

	// 애니메이션 텍스처 변경
	CMaterial* pMaterial = FindComponentFromType<CMaterial>(CT_MATERIAL);
	if (pMaterial != nullptr)
	{
		pMaterial->SetDiffuseTex(m_pCurClip->iTexRegister, m_pCurClip->pTexture);
		SAFE_RELEASE(pMaterial);
	}
}

void CAnimation2D::SetDefaultClip(const string& _strName)
{
	// 기본 애니메이션 클립 설정
	m_strDefaultClip = _strName;
}

void CAnimation2D::AddFixedClip(const string& _aniName, const string& _textureKey, const wchar_t* _pFileName, 
								float _fixedTime, const vector<Clip2DFrame>& _vecFrame,
								ANIMATION2D_TYPE _eType, ANIMATION_OPTION _eOption, const string& _strPathKey)
{
	// 찾기
	PAnimationClip2D pClip = FindClip(_aniName);
	if (pClip != nullptr)
	{
		return;
	}

	// AnimationClip2D
	// > 2D 애니메이션을 만들기 위해 필요한 구조체
	pClip = new AnimationClip2D;
	pClip->strName		  = _aniName;		// 이름
	pClip->eType		  = _eType;			// 타입(Atals, Frame)
	pClip->eOption		  = _eOption;		// 옵션
	pClip->fPlayFixedTime = _fixedTime;		// 애니메이션 재생 시간
	pClip->vecFrame		  = _vecFrame;		// 프레임 갯수
	pClip->iFrame		  = 0;				// 현재 애니메이션 프레임(장)
	pClip->fPlayTime	  = 0.f;			// 현재 플레이 시간

	// 애니메이션 텍스처
	GET_SINGLE(CResourcesManager)->CreateTexture(_textureKey, _pFileName, _strPathKey);
	pClip->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(_textureKey);
	pClip->iTexRegister = 0;
	pClip->fTextureWidth = (float)pClip->pTexture->GetWidth();
	pClip->fTextureHeight = (float)pClip->pTexture->GetHeight();

	// 기본 애니메이션 설정
	if (m_strDefaultClip == "")
	{
		m_strDefaultClip = _aniName;
	}

	// 현재 애니메이션이 없다면? 생성된 애니메이션으로 설정
	if (m_pCurClip == nullptr)
	{
		m_pCurClip = pClip;
	}

	// 추가
	m_mapClip.insert(make_pair(_aniName, pClip));
}

void CAnimation2D::AddFrameTimeClip(const string& _aniName, const string& _textureKey, const wchar_t* _pFileName,
									vector<float> _vecFrameTime, const vector<Clip2DFrame>& _vecFrame, 
									ANIMATION2D_TYPE _eType, ANIMATION_OPTION _eOption, const string& _strPathKey)
{
	// 찾기
	PAnimationClip2D pClip = FindClip(_aniName);
	if (pClip != nullptr)
	{
		return;
	}

	// AnimationClip2D
	// > 2D 애니메이션을 만들기 위해 필요한 구조체
	pClip = new AnimationClip2D;
	pClip->strName		= _aniName;			// 이름
	pClip->eType		= _eType;			// 타입(Atals, Frame)
	pClip->eOption		= _eOption;			// 옵션
	pClip->vecFrameTime = _vecFrameTime;	// 프레임별 재생시간
	pClip->vecFrame		= _vecFrame;		// 프레임 갯수
	pClip->iFrame		= 0;				// 현재 애니메이션 프레임(장)
	pClip->fPlayTime	= 0.f;				// 현재 플레이 시간

	// 애니메이션 텍스처
	GET_SINGLE(CResourcesManager)->CreateTexture(_textureKey, _pFileName, _strPathKey);
	pClip->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(_textureKey);
	pClip->iTexRegister = 0;
	pClip->fTextureWidth = (float)pClip->pTexture->GetWidth();
	pClip->fTextureHeight = (float)pClip->pTexture->GetHeight();

	// 기본 애니메이션 설정
	if (m_strDefaultClip == "")
	{
		m_strDefaultClip = _aniName;
	}

	// 현재 애니메이션이 없다면? 생성된 애니메이션으로 설정
	if (m_pCurClip == nullptr)
	{
		m_pCurClip = pClip;
	}

	// 추가
	m_mapClip.insert(make_pair(_aniName, pClip));
}





















PAnimationClip2D CAnimation2D::FindClip(const string& _strName)
{
	// 애니메이션 찾기
	unordered_map<string, PAnimationClip2D>::iterator iter = m_mapClip.find(_strName);
	if (iter == m_mapClip.end())
	{
		return nullptr;
	}
	
	return iter->second;
}