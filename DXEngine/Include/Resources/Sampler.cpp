#include "PCHEngine.h"
#include "Sampler.h"
#include "../Device.h"

ENGINE_USING

CSampler::CSampler() :
	m_pSampler(nullptr)
{
}

CSampler::~CSampler()
{
	SAFE_RELEASE(m_pSampler);
}

bool CSampler::CreateSampler(const string& _strName, D3D11_FILTER _eFilter,
							 D3D11_TEXTURE_ADDRESS_MODE _eU, 
							 D3D11_TEXTURE_ADDRESS_MODE _eV, 
							 D3D11_TEXTURE_ADDRESS_MODE _eW)
{
	// »ùÇÃ·¯ ÅÂ±×¸í
	SetTag(_strName);

	D3D11_SAMPLER_DESC	tDesc = {};
	tDesc.Filter		 = _eFilter;
	tDesc.AddressU		 = _eU;
	tDesc.AddressV		 = _eV;
	tDesc.AddressW		 = _eW;
	tDesc.MinLOD		 = -FLT_MAX;
	tDesc.MaxLOD		 = FLT_MAX;
	tDesc.MipLODBias	 = 0.f;
	tDesc.MaxAnisotropy  = 1;
	tDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	float f = 1.f;
	memcpy(tDesc.BorderColor, &f, sizeof(float) * 4);

	// »ý¼º
	if (FAILED(DEVICE->CreateSamplerState(&tDesc, &m_pSampler)))
	{
		SAFE_RELEASE(m_pSampler);
		return false;
	}

	return true;
}
 
void CSampler::SetShader(int _iRegister)
{
	// ÇÈ¼¿ ¼ÎÀÌ´õ SlotNum(iRegister)¿¡ »ùÇÃ·¯¸¦ ¼³Á¤
	CONTEXT->PSSetSamplers(_iRegister, 1, &m_pSampler);
}