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
	// Blend Data �� ����
	// > State�� ����� ���� �ʿ��� ����ü D3D11_RENDER_TARGET_BLEND_DESC ��
	D3D11_RENDER_TARGET_BLEND_DESC tDesc = {};
	tDesc.BlendEnable			= _bEnable;
	tDesc.SrcBlend				= _srcBlend;
	tDesc.DestBlend				= _destBlend;
	tDesc.BlendOp				= _blendOp;
	tDesc.SrcBlendAlpha			= _srcAlphaBlend;
	tDesc.DestBlendAlpha		= _destAlphaBlend;
	tDesc.BlendOpAlpha			= _blendAlphaOp;
	tDesc.RenderTargetWriteMask = _iWriteMask;

	// �߰�
	m_vecTargetDesc.push_back(tDesc);
}

bool CBlendState::CreateState(BOOL _bAlphaCoverage, BOOL _bIndependent)
{
	D3D11_BLEND_DESC tDesc = {};
	tDesc.AlphaToCoverageEnable  = _bAlphaCoverage;
	tDesc.IndependentBlendEnable = _bIndependent;
	// D3D11_RENDER_TARGET_BLEND_DESC 8���� 1������ ���
	for (size_t i = 0; i < m_vecTargetDesc.size(); ++i)
	{
		memcpy(&tDesc.RenderTarget[i], &m_vecTargetDesc[i], sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
	}
	m_vecTargetDesc.clear();

	// ���� ����
	// > �ٿ� ĳ�����̿�
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
	// ���� ���� ����
	// > Get�Լ��� �̿��ؼ� state�� ������ �Ǹ� ���۷��� ī��Ʈ�� �����Ѵ�.
	CONTEXT->OMGetBlendState((ID3D11BlendState**)&m_pOldState, m_fOldBlendFactor, &m_iOldSampleMask);

	// ���� ���� ����
	CONTEXT->OMSetBlendState((ID3D11BlendState*)m_pState, m_fBlendFactor, m_iSampleMask);
}

void CBlendState::ResetState()
{
	// ���� ������ �����⶧���� �ʿ���� ���� �������� �ʵ��� �Ѵ�.
	// > ��� �� ���� ���·� �����Ѵ�.
	CONTEXT->OMSetBlendState((ID3D11BlendState*)m_pOldState, m_fOldBlendFactor, m_iOldSampleMask);
	SAFE_RELEASE(m_pOldState);
}