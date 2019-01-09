#include "PCHEngine.h"
#include "FadeScreen.h"
#include "Component/Transform.h"
#include "Component/Renderer.h"
#include "Component/Material.h"
#include "Device.h"
#include "Object/GameObject.h"
#include "Resources/Texture.h"
#include "Scene/SceneManager.h"
#include "Resources/ResourcesManager.h"

ENGINE_USING

CFadeScreen::CFadeScreen() :
	m_eFadeInState(FIS_READY),
	m_eFadeOutState(FOS_READY),
	m_colorState(0.f),
	m_fp(nullptr)
{
	// 상수 버퍼 초기화
	m_fadeCBuffer.vColorMin = Vector4::Zero;
	m_fadeCBuffer.vColorMax = Vector4::Zero;
	m_fadeCBuffer.ratio = 0.f;
}

CFadeScreen::CFadeScreen(const CFadeScreen& _screenFade)
{
}

CFadeScreen::~CFadeScreen()
{
}

bool CFadeScreen::Init()
{
	// Texture
	CTexture* pFadeBGTex = GET_SINGLE(CResourcesManager)->FindTexture("FadeTexture");
	float texWidth  = (float)pFadeBGTex->GetWidth();
	float texHeight = (float)pFadeBGTex->GetHeight();

	// Scale
	m_pTransform->SetWorldScale(Vector3(texWidth, texHeight, 1.f));

	// Renderer
	CRenderer* pFadeBGRender = m_pObject->AddComponent<CRenderer>("FadeBGRenderer");
	pFadeBGRender->SetMesh("TexRect");
	pFadeBGRender->SetRenderState(ALPHA_BLEND);
	pFadeBGRender->SetShader(FADE_SHADER);
	pFadeBGRender->CreateRendererCBuffer("FadeShader", sizeof(FadeCBuffer));
	SAFE_RELEASE(pFadeBGRender);

	int registerSlot = 0;
	CMaterial* pMaterial = m_pObject->FindComponentFromType<CMaterial>(CT_MATERIAL);
	pMaterial->SetDiffuseTex(registerSlot, pFadeBGTex);
	SAFE_RELEASE(pFadeBGTex);
	SAFE_RELEASE(pMaterial);

	return true;
}

int CFadeScreen::Input(float _fTime)
{
	return 0;
}

int CFadeScreen::Update(float _fTime)
{
	int a = 0;

	// fp 함수 포인터 호출
	if (m_fp != nullptr)
	{
		(this->*m_fp)(_fTime);
	}
	return 0;
}

int CFadeScreen::LateUpdate(float _fTime)
{
	return 0;
}

void CFadeScreen::Collision(float _fTime)
{
}

void CFadeScreen::Render(float _fTime)
{
}

void CFadeScreen::AfterClone()
{
}

CFadeScreen* CFadeScreen::Clone()
{
	return nullptr;
}

void CFadeScreen::VariableClear()
{
	m_fp					= nullptr;
	m_eFadeInState			= FIS_READY;
	m_eFadeOutState			= FOS_READY;
	m_colorState			= 0.f;
	m_fadeCBuffer.ratio		= 0.f;
	m_fadeCBuffer.vColorMin = Vector4::Zero;
	m_fadeCBuffer.vColorMax = Vector4::Zero;
}

void CFadeScreen::SetFadeType(eFadeType _eType)
{
	// 초기화
	VariableClear();

	// 타입별 min, max 설정
	if (_eType == FADETYPE_IN)
	{
		m_fadeCBuffer.vColorMin = Vector4(0.f, 0.f, 0.f, 0.f);
		m_fadeCBuffer.vColorMax = Vector4(0.f, 0.f, 0.f, 1.f);
	}
	else if (_eType == FADETYPE_OUT)
	{
		m_fadeCBuffer.vColorMin = Vector4(0.f, 0.f, 0.f, 1.f);
		m_fadeCBuffer.vColorMax = Vector4(0.f, 0.f, 0.f, 0.f);
	}
}

eFadeInState CFadeScreen::GetFadeInState() const
{
	return m_eFadeInState;
}

eFadeOutState CFadeScreen::GetFadeOutState() const
{
	return m_eFadeOutState;
}

void CFadeScreen::SetFuncPointer(void(CFadeScreen::*_fp)(float))
{
	m_fp = _fp;
}

void CFadeScreen::FadeIn(float _fTime)
{
	m_eFadeInState = FIS_START;
	m_colorState = m_fadeCBuffer.ratio;
	m_fadeCBuffer.ratio	+= _fTime * 2.f;
	CRenderer* pFadeBGRender = m_pObject->FindComponentFromType<CRenderer>(CT_RENDERER);
	pFadeBGRender->UpdateRendererCBuffer("FadeCBuffer", &m_fadeCBuffer, sizeof(FadeCBuffer));
	SAFE_RELEASE(pFadeBGRender);
	if (m_colorState >= 1.f)
	{
		m_fp = nullptr;
		m_eFadeInState = FIS_END;
	}
}

void CFadeScreen::FadeOut(float _fTime)
{
	m_eFadeOutState = FOS_START;
	m_colorState = m_fadeCBuffer.ratio;
	m_fadeCBuffer.ratio += _fTime * 2.f;
	CRenderer* pFadeBGRender = m_pObject->FindComponentFromType<CRenderer>(CT_RENDERER);
	pFadeBGRender->UpdateRendererCBuffer("FadeCBuffer", &m_fadeCBuffer, sizeof(FadeCBuffer));
	SAFE_RELEASE(pFadeBGRender);
	if (m_colorState >= 1.f)
	{
		m_fp = nullptr;
		m_eFadeOutState = FOS_END;
	}
}