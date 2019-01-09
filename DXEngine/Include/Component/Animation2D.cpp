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
	// �⺻ �ִϸ��̼� �̸� ����
	m_strDefaultClip = _anim.m_strDefaultClip;

	// �ִϸ��̼� Ŭ�� ����
	m_mapClip.clear();
	unordered_map<string, PAnimationClip2D>::const_iterator	iter;
	unordered_map<string, PAnimationClip2D>::const_iterator	iterEnd = _anim.m_mapClip.end();
	for (iter = _anim.m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		PAnimationClip2D pClip = new AnimationClip2D;
		*pClip = *(iter->second);  // ������ ������..(�� ����) 
		pClip->pTexture->AddRef(); // ���� Texture* ������ ����Ű��, 
								   // ������ ������ �� '��۸� ������'�� �����ϱ� ���� RefCount�� ������Ų��.

		// �߰�
		m_mapClip.insert(make_pair(iter->first, pClip));

		// ���� Ŭ�� ����
		if (_anim.m_pCurClip->strName == iter->first)
		{
			m_pCurClip = pClip;
		}
	}
}

CAnimation2D::~CAnimation2D()
{
	// PAnimationClip2D �޸� ����
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

	// ���� �ð�
	float frameTime = 0.f;
	if(m_pCurClip->vecFrameTime.size() == 0)
	{
		// ��) PlayLimitTime(1��)���� 15���� �ؽ�ó�� �����ؾ��ϴ� ���
		//     1/60 = 0.00666�ʰ� ���´�. �� 0.00666�ʸ��� ���徿 �ؽ�ó�� ����Ǿ���Ѵ�.
		frameTime = m_pCurClip->fPlayFixedTime / m_pCurClip->vecFrame.size();
	}
	// �����Ӻ� �ӵ�
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

		// �ִϸ��̼� ����
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
					// �ִϸ��̼� �ݺ�
					break;
				}
				case AO_DISABLE:
				{
					m_pObject->SetEnable(false);
					break;
				}
				case AO_ONCE_DESTROY:
				{
					// �ִϸ��̼� ���� �� �ִϸ��̼��� ����ϴ� ������Ʈ ����
					m_pObject->Die();
					break;
				}
				case AO_ONCE_RETURN:
				{
					// �⺻���� ������ �ִϸ��̼� ó��
					m_pCurClip = FindClip(m_strDefaultClip);
					break;
				}
			}
			
		}
	}

	// Shader �ܰ迡�� �ؽ�ó�� ó���ϱ� ���� Clip2DFrame����ü(UV) ���� ä���ְ�
	// "Animation2D" Register Slot Number(b8) ��� ���۸� �����Ѵ�.
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
			// > Uv (V2 LeftTop), (V2 RightBottom) ���� ������ �� �ִ� ����ü
			// > �̹����� ���� �������� Uv LeftTop, RightBottom ����
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

		// ��� ���� ����
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
	// �ִϸ��̼� ����
	m_isAniPlaying = true;
	m_frameIndex = 0;

	// ���� �ִϸ��̼ǰ� ����� �ִϸ��̼��� ���� ���
	if (m_pCurClip->strName == _strName)
	{
		return;
	}
	
	// ã��
	// > PAnimationClip2D ������, �÷��� Ÿ�� �ʱ�ȭ
	m_pCurClip = FindClip(_strName);
	m_pCurClip->isAniPlaying = true;
	m_pCurClip->iFrame	  = 0;
	m_pCurClip->fPlayTime = 0.f;

	// �ִϸ��̼� �ؽ�ó ����
	CMaterial* pMaterial = FindComponentFromType<CMaterial>(CT_MATERIAL);
	if (pMaterial != nullptr)
	{
		pMaterial->SetDiffuseTex(m_pCurClip->iTexRegister, m_pCurClip->pTexture);
		SAFE_RELEASE(pMaterial);
	}
}

void CAnimation2D::SetDefaultClip(const string& _strName)
{
	// �⺻ �ִϸ��̼� Ŭ�� ����
	m_strDefaultClip = _strName;
}

void CAnimation2D::AddFixedClip(const string& _aniName, const string& _textureKey, const wchar_t* _pFileName, 
								float _fixedTime, const vector<Clip2DFrame>& _vecFrame,
								ANIMATION2D_TYPE _eType, ANIMATION_OPTION _eOption, const string& _strPathKey)
{
	// ã��
	PAnimationClip2D pClip = FindClip(_aniName);
	if (pClip != nullptr)
	{
		return;
	}

	// AnimationClip2D
	// > 2D �ִϸ��̼��� ����� ���� �ʿ��� ����ü
	pClip = new AnimationClip2D;
	pClip->strName		  = _aniName;		// �̸�
	pClip->eType		  = _eType;			// Ÿ��(Atals, Frame)
	pClip->eOption		  = _eOption;		// �ɼ�
	pClip->fPlayFixedTime = _fixedTime;		// �ִϸ��̼� ��� �ð�
	pClip->vecFrame		  = _vecFrame;		// ������ ����
	pClip->iFrame		  = 0;				// ���� �ִϸ��̼� ������(��)
	pClip->fPlayTime	  = 0.f;			// ���� �÷��� �ð�

	// �ִϸ��̼� �ؽ�ó
	GET_SINGLE(CResourcesManager)->CreateTexture(_textureKey, _pFileName, _strPathKey);
	pClip->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(_textureKey);
	pClip->iTexRegister = 0;
	pClip->fTextureWidth = (float)pClip->pTexture->GetWidth();
	pClip->fTextureHeight = (float)pClip->pTexture->GetHeight();

	// �⺻ �ִϸ��̼� ����
	if (m_strDefaultClip == "")
	{
		m_strDefaultClip = _aniName;
	}

	// ���� �ִϸ��̼��� ���ٸ�? ������ �ִϸ��̼����� ����
	if (m_pCurClip == nullptr)
	{
		m_pCurClip = pClip;
	}

	// �߰�
	m_mapClip.insert(make_pair(_aniName, pClip));
}

void CAnimation2D::AddFrameTimeClip(const string& _aniName, const string& _textureKey, const wchar_t* _pFileName,
									vector<float> _vecFrameTime, const vector<Clip2DFrame>& _vecFrame, 
									ANIMATION2D_TYPE _eType, ANIMATION_OPTION _eOption, const string& _strPathKey)
{
	// ã��
	PAnimationClip2D pClip = FindClip(_aniName);
	if (pClip != nullptr)
	{
		return;
	}

	// AnimationClip2D
	// > 2D �ִϸ��̼��� ����� ���� �ʿ��� ����ü
	pClip = new AnimationClip2D;
	pClip->strName		= _aniName;			// �̸�
	pClip->eType		= _eType;			// Ÿ��(Atals, Frame)
	pClip->eOption		= _eOption;			// �ɼ�
	pClip->vecFrameTime = _vecFrameTime;	// �����Ӻ� ����ð�
	pClip->vecFrame		= _vecFrame;		// ������ ����
	pClip->iFrame		= 0;				// ���� �ִϸ��̼� ������(��)
	pClip->fPlayTime	= 0.f;				// ���� �÷��� �ð�

	// �ִϸ��̼� �ؽ�ó
	GET_SINGLE(CResourcesManager)->CreateTexture(_textureKey, _pFileName, _strPathKey);
	pClip->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(_textureKey);
	pClip->iTexRegister = 0;
	pClip->fTextureWidth = (float)pClip->pTexture->GetWidth();
	pClip->fTextureHeight = (float)pClip->pTexture->GetHeight();

	// �⺻ �ִϸ��̼� ����
	if (m_strDefaultClip == "")
	{
		m_strDefaultClip = _aniName;
	}

	// ���� �ִϸ��̼��� ���ٸ�? ������ �ִϸ��̼����� ����
	if (m_pCurClip == nullptr)
	{
		m_pCurClip = pClip;
	}

	// �߰�
	m_mapClip.insert(make_pair(_aniName, pClip));
}





















PAnimationClip2D CAnimation2D::FindClip(const string& _strName)
{
	// �ִϸ��̼� ã��
	unordered_map<string, PAnimationClip2D>::iterator iter = m_mapClip.find(_strName);
	if (iter == m_mapClip.end())
	{
		return nullptr;
	}
	
	return iter->second;
}