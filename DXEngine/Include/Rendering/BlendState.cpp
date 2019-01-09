#include "PCHEngine.h"
#include "Utility/Types.h"
#include "BlendState.h"
#include "../Device.h"

ENGINE_USING

CBlendState::CBlendState()
{
	m_eState = RS_BLEND;
	m_iSampleMask = 0xffffffff;
}

CBlendState::~CBlendState()
{
}

void CBlendState::AddTargetDesc(BOOL _bEnable, D3D11_BLEND _srcBlend, D3D11_BLEND _destBlend, D3D11_BLEND_OP _blendOp, 
								D3D11_BLEND _srcAlphaBlend, D3D11_BLEND _destAlphaBlend, D3D11_BLEND_OP _blendAlphaOp, UINT8 _iWriteMask)
{
	// Blend Data 값 정의
	// > State를 만들기 위해 필요한 구조체 D3D11_RENDER_TARGET_BLEND_DESC 값
	D3D11_RENDER_TARGET_BLEND_DESC tDesc = {};
	tDesc.BlendEnable			= _bEnable;
	tDesc.SrcBlend				= _srcBlend;
	tDesc.DestBlend				= _destBlend;
	tDesc.BlendOp				= _blendOp;
	tDesc.SrcBlendAlpha			= _srcAlphaBlend;
	tDesc.DestBlendAlpha		= _destAlphaBlend;
	tDesc.BlendOpAlpha			= _blendAlphaOp;
	tDesc.RenderTargetWriteMask = _iWriteMask;

	// 추가
	m_vecTargetDesc.push_back(tDesc);
}

bool CBlendState::CreateState(BOOL _bAlphaCoverage, BOOL _bIndependent)
{
	D3D11_BLEND_DESC tDesc = {};
	tDesc.AlphaToCoverageEnable  = _bAlphaCoverage;
	tDesc.IndependentBlendEnable = _bIndependent;
	// D3D11_RENDER_TARGET_BLEND_DESC 8개중 1개만을 사용
	for (size_t i = 0; i < m_vecTargetDesc.size(); ++i)
	{
		memcpy(&tDesc.RenderTarget[i], &m_vecTargetDesc[i], sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
	}
	m_vecTargetDesc.clear();

	// 블렌딩 생성
	// > 다운 캐스팅이용
	if (FAILED(DEVICE->CreateBlendState(&tDesc, (ID3D11BlendState**)&m_pState)))
	{
		return false;
	}

	return true;
}

void CBlendState::SetSampleMask(UINT _iMask)
{
	m_iSampleMask = _iMask;
}

void CBlendState::SetBlendFactor(float _fFactor[4])
{
	memcpy(m_fBlendFactor, _fFactor, sizeof(float) * 4);
}

void CBlendState::SetState()
{
	// 이전 블렌딩 상태
	// > Get함수를 이용해서 state를 얻어오게 되면 레퍼런스 카운트가 증가한다.
	CONTEXT->OMGetBlendState((ID3D11BlendState**)&m_pOldState, m_fOldBlendFactor, &m_iOldSampleMask);

	// 현재 블렌딩 상태
	CONTEXT->OMSetBlendState((ID3D11BlendState*)m_pState, m_fBlendFactor, m_iSampleMask);
}

void CBlendState::ResetState()
{
	// 블렌딩 연산은 느리기때문에 필요없는 경우는 연산하지 않도록 한다.
	// > 사용 후 기존 상태로 변경한다.
	CONTEXT->OMSetBlendState((ID3D11BlendState*)m_pOldState, m_fOldBlendFactor, m_iOldSampleMask);
	SAFE_RELEASE(m_pOldState);
}