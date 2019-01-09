#pragma once
#include "RenderState.h"

ENGINE_BEGIN
class ENGINE_DLL CBlendState : public CRenderState
{
private:
	friend class CRenderManager;

private:
	CBlendState();
	~CBlendState();

/* D3D11_RENDER_TARGET_BLEND_DESC 구조체 데이터 */
private:
	vector<D3D11_RENDER_TARGET_BLEND_DESC> m_vecTargetDesc;
	float	m_fBlendFactor[4];
	UINT	m_iSampleMask;
	float	m_fOldBlendFactor[4];
	UINT	m_iOldSampleMask;
	
/* RenderState 생성 */
public:
	void AddTargetDesc(BOOL _bEnable, 
					   D3D11_BLEND _srcBlend		= D3D11_BLEND_SRC_ALPHA,
					   D3D11_BLEND _destBlend		= D3D11_BLEND_INV_SRC_ALPHA,
					   D3D11_BLEND_OP _blendOp		= D3D11_BLEND_OP_ADD,
					   D3D11_BLEND _srcAlphaBlend	= D3D11_BLEND_ONE,
					   D3D11_BLEND _destAlphaBlend	= D3D11_BLEND_ZERO,
					   D3D11_BLEND_OP _blendAlphaOp = D3D11_BLEND_OP_ADD,
					   UINT8 _iWriteMask			= D3D11_COLOR_WRITE_ENABLE_ALL);
	bool CreateState(BOOL _bAlphaCoverage, BOOL _bIndependent);
	void SetSampleMask(UINT _iMask);
	void SetBlendFactor(float _fFactor[4]);

/* 순수 가상 함수 재정의 */
public:
	void SetState()   override;
	void ResetState() override;
};
ENGINE_END