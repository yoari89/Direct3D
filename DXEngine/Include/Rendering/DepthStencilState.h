#pragma once
#include "RenderState.h"

ENGINE_BEGIN
class ENGINE_DLL CDepthStencilState : public CRenderState
{
private:
	friend class CRenderManager;

private:
	CDepthStencilState();
	~CDepthStencilState();

private:
	UINT m_iStencilRef;
	UINT m_iOldStencilRef;

public:
	bool CreateState(BOOL bDepthEnable,
					 D3D11_DEPTH_WRITE_MASK eMask			= D3D11_DEPTH_WRITE_MASK_ALL,
					 D3D11_COMPARISON_FUNC eDepthFunc		= D3D11_COMPARISON_LESS,
					 BOOL bStencilEnable					= FALSE,
					 UINT8 iStencilReadMask					= 0, 
					 UINT8 iStencilWriteMask				= 0,
					 D3D11_DEPTH_STENCILOP_DESC tFrontFace	= {},
					 D3D11_DEPTH_STENCILOP_DESC tBackFace	= {});

/* 순수 가상 함수 재정의 */
public:
	void SetState()   override;
	void ResetState() override;
};
ENGINE_END