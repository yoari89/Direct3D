#include "PCHEngine.h"
#include "DepthStencilState.h"
#include "Utility/Types.h"
#include "../Device.h"

ENGINE_USING

CDepthStencilState::CDepthStencilState()
{
	m_eState = RS_DEPTHSTENCIL;
	m_iStencilRef = 0xffffffff;
}

CDepthStencilState::~CDepthStencilState()
{
}

bool CDepthStencilState::CreateState(BOOL bDepthEnable,
									 D3D11_DEPTH_WRITE_MASK eMask,			
									 D3D11_COMPARISON_FUNC eDepthFunc,		
									 BOOL bStencilEnable,	
									 UINT8 iStencilReadMask,
									 UINT8 iStencilWriteMask,				
									 D3D11_DEPTH_STENCILOP_DESC tFrontFace,	
									 D3D11_DEPTH_STENCILOP_DESC tBackFace)	
{
	D3D11_DEPTH_STENCIL_DESC tDesc	= {};
	tDesc.DepthEnable				= bDepthEnable;
	tDesc.DepthWriteMask			= eMask;
	tDesc.DepthFunc					= eDepthFunc;
	tDesc.StencilEnable				= bStencilEnable;
	tDesc.StencilReadMask			= iStencilReadMask;
	tDesc.StencilWriteMask			= iStencilWriteMask;
	tDesc.FrontFace					= tFrontFace;
	tDesc.BackFace					= tBackFace;

	// 블렌딩 생성
	if (FAILED(DEVICE->CreateDepthStencilState(&tDesc, (ID3D11DepthStencilState**)&m_pState)))
	{
		return false;
	}

	return true;
}

void CDepthStencilState::SetState()
{
	// 이전 블렌딩 상태
	CONTEXT->OMGetDepthStencilState((ID3D11DepthStencilState**)&m_pOldState, &m_iOldStencilRef);

	// 현재 블렌딩 상태
	CONTEXT->OMSetDepthStencilState((ID3D11DepthStencilState*)m_pState, m_iStencilRef);
}

void CDepthStencilState::ResetState()
{
	CONTEXT->OMSetDepthStencilState((ID3D11DepthStencilState*)m_pOldState, m_iOldStencilRef);
	SAFE_RELEASE(m_pOldState);
}