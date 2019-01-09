#pragma once
#include "../ReferenceCnt.h"

ENGINE_BEGIN
class ENGINE_DLL CSampler : public CReferenceCnt
{		
private :
	friend class CResourcesManager;

private:
	CSampler();
	~CSampler();

/* 基敲傅 包府 按眉 */
private:
	ID3D11SamplerState*	m_pSampler;

public:
	bool CreateSampler(const string& _strName,
					   D3D11_FILTER _eFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
					   D3D11_TEXTURE_ADDRESS_MODE _eU = D3D11_TEXTURE_ADDRESS_WRAP,
					   D3D11_TEXTURE_ADDRESS_MODE _eV = D3D11_TEXTURE_ADDRESS_WRAP,
					   D3D11_TEXTURE_ADDRESS_MODE _eW = D3D11_TEXTURE_ADDRESS_WRAP);
	void SetShader(int _iRegister);
};
ENGINE_END