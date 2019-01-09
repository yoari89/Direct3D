#include "PCHEngine.h"
#include "Material.h"
#include "Resources/Texture.h"
#include "Resources/Sampler.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/RenderManager.h"
#include "Component/Animation2D.h"
#include "Object/GameObject.h"

ENGINE_USING

CMaterial::CMaterial()
{
	m_eComType = CT_MATERIAL;
}

CMaterial::CMaterial(const CMaterial& _material) : 
	CComponent(_material)
{
	m_vecMaterial.clear();
	for (size_t i = 0; i < _material.m_vecMaterial.size(); ++i)
	{
		vector<PSubsetMaterial>	vec;
		m_vecMaterial.push_back(vec);
		for (size_t j = 0; j < _material.m_vecMaterial[i].size(); ++j)
		{
			PSubsetMaterial	pMtrl = new SubsetMaterial;

			pMtrl->tMaterial = _material.m_vecMaterial[i][j]->tMaterial;
			pMtrl->pDiffuseTex = _material.m_vecMaterial[i][j]->pDiffuseTex;
			pMtrl->iDiffuseTexRegister = _material.m_vecMaterial[i][j]->iDiffuseTexRegister;
			pMtrl->pSampler = _material.m_vecMaterial[i][j]->pSampler;
			pMtrl->iDiffuseSamplerRegister = _material.m_vecMaterial[i][j]->iDiffuseSamplerRegister;

			if (pMtrl->pDiffuseTex)
				pMtrl->pDiffuseTex->AddRef();

			if (pMtrl-> pSampler)
				pMtrl->pSampler->AddRef();

			pMtrl->vecMultiTex = _material.m_vecMaterial[i][j]->vecMultiTex;

			for (size_t k = 0; k < pMtrl->vecMultiTex.size(); ++k)
			{
				if (pMtrl->vecMultiTex[k].pTex)
					pMtrl->vecMultiTex[k].pTex->AddRef();

				if (pMtrl->vecMultiTex[k].pSampler)
					pMtrl->vecMultiTex[k].pSampler->AddRef();
			}

			m_vecMaterial[i].push_back(pMtrl);
		}
	}
}

CMaterial::~CMaterial()
{
	for (size_t i = 0; i < m_vecMaterial.size(); ++i)
	{
		for (size_t j = 0; j < m_vecMaterial[i].size(); ++j)
		{
			for (size_t k = 0; k < m_vecMaterial[i][j]->vecMultiTex.size(); ++k)
			{
				SAFE_RELEASE(m_vecMaterial[i][j]->vecMultiTex[k].pTex);
				SAFE_RELEASE(m_vecMaterial[i][j]->vecMultiTex[k].pSampler);
			}

			SAFE_RELEASE(m_vecMaterial[i][j]->pDiffuseTex);
			SAFE_RELEASE(m_vecMaterial[i][j]->pSampler);
			SAFE_DELETE(m_vecMaterial[i][j]);
		}
		m_vecMaterial[i].clear();
	}
	m_vecMaterial.clear();
}

PSubsetMaterial CMaterial::CreateSubset()
{
	PSubsetMaterial pMtrl = new SubsetMaterial;

	// 기본 선형 필터링 샘플링 방식 설정
	pMtrl->pSampler = GET_SINGLE(CResourcesManager)->FindSampler(SAMPLER_LINEAR);
	return pMtrl;
}

void CMaterial::SetMaterial(const Vector4& _vDiffuse, int _iContainer, int _iSubset)
{
	// Container 
	// > _iContainer가 보관된 Material 크기보다 큰 경우 생성 후 보관
	// > _iContainer값은 순차적으로 들어와야한다. 그렇지 않은 경우 Subset을 찾을 경우 문제가 생긴다.
	if (_iContainer >= (int)m_vecMaterial.size())
	{
		vector<PSubsetMaterial> vec;
		m_vecMaterial.push_back(vec);
	}

	// Subset 
	if (_iSubset >= (int)m_vecMaterial[_iContainer].size())
	{
		m_vecMaterial[_iContainer].push_back(CreateSubset());
	}

	// Set Diffuse (난 반사광)
	// > 물체의 색상이 빛을 받아 반사하는 색상 
	m_vecMaterial[_iContainer][_iSubset]->tMaterial.vDiffuse = _vDiffuse;
}

void CMaterial::SetDiffuseTex(int _iRegister, const string& _strKey, int _iContainer, int _iSubset)
{
	// Container
	// > SetMaterial() 참조
	if (_iContainer >= (int)m_vecMaterial.size())
	{
		vector<PSubsetMaterial>	vec;
		m_vecMaterial.push_back(vec);
	}

	// Subset
	// > SetMaterial() 참조
	if (_iSubset >= (int)m_vecMaterial[_iContainer].size())
	{
		m_vecMaterial[_iContainer].push_back(CreateSubset());
	}

	// SubsetMaterial Texture 설정
	// > 기존에 Texture가 존재한다면 해제
	PSubsetMaterial pMaterial = m_vecMaterial[_iContainer][_iSubset];
	SAFE_RELEASE(pMaterial->pDiffuseTex);
	pMaterial->pDiffuseTex = GET_SINGLE(CResourcesManager)->FindTexture(_strKey);
	pMaterial->iDiffuseTexRegister = _iRegister;
}

void CMaterial::SetDiffuseTex(int _iRegister, const string& _strKey, const TCHAR* _pFileName, 
							  const string& _strPathKey, int _iContainer, int _iSubset)
{
	// Container
	if (_iContainer >= (int)m_vecMaterial.size())
	{
		vector<PSubsetMaterial>	vec;
		m_vecMaterial.push_back(vec);
	}

	// Subset
	if (_iSubset >= (int)m_vecMaterial[_iContainer].size())
	{
		m_vecMaterial[_iContainer].push_back(CreateSubset());
	}

	// SubsetMaterial Texture 설정
	// > Texture가 없는 경우 생성
	PSubsetMaterial pMaterial = m_vecMaterial[_iContainer][_iSubset];
	SAFE_RELEASE(pMaterial->pDiffuseTex);
	GET_SINGLE(CResourcesManager)->CreateTexture(_strKey, _pFileName, _strPathKey);
	pMaterial->pDiffuseTex = GET_SINGLE(CResourcesManager)->FindTexture(_strKey);
	pMaterial->iDiffuseTexRegister = _iRegister;
}

void CMaterial::SetDiffuseTexFromFullPath(int _iRegister, const string& _strKey, 
										  const TCHAR* _pFullPath, int _iContainer, int _iSubset)
{
	// Container
	if (_iContainer >= (int)m_vecMaterial.size())
	{
		vector<PSubsetMaterial>	vec;
		m_vecMaterial.push_back(vec);
	}

	// Subset
	if (_iSubset >= (int)m_vecMaterial[_iContainer].size())
	{
		m_vecMaterial[_iContainer].push_back(CreateSubset());
	}

	// SubsetMaterial Texture 설정
	// > Texture가 없는 경우 생성(FullPath)
	PSubsetMaterial pMaterial = m_vecMaterial[_iContainer][_iSubset];
	GET_SINGLE(CResourcesManager)->CreateTextureFromFullPath(_strKey, _pFullPath);
	pMaterial->pDiffuseTex = GET_SINGLE(CResourcesManager)->FindTexture(_strKey);
	pMaterial->iDiffuseTexRegister = _iRegister;
}

void CMaterial::SetDiffuseTex(int _iRegister, CTexture* _pTex, int _iContainer, int _isubSet)
{
	if (_iContainer >= (int)m_vecMaterial.size())
	{
		vector<PSubsetMaterial>	vec;
		m_vecMaterial.push_back(vec);
	}
	if (_isubSet >= (int)m_vecMaterial[_iContainer].size())
	{
		m_vecMaterial[_iContainer].push_back(CreateSubset());
	}

	PSubsetMaterial	pMaterial = m_vecMaterial[_iContainer][_isubSet];

	SAFE_RELEASE(pMaterial->pDiffuseTex);
	_pTex->AddRef();
	pMaterial->pDiffuseTex = _pTex;
	pMaterial->iDiffuseTexRegister = _iRegister;
}

void CMaterial::SetDiffuseTex(int iRegister, const string & strKey, const vector<const TCHAR*>& vecFileName, const string & strPathKey, int iContainer, int iSubset)
{
	if (iContainer >= m_vecMaterial.size())
	{
		vector<PSubsetMaterial>	vec;
		m_vecMaterial.push_back(vec);
	}

	if (iSubset >= m_vecMaterial[iContainer].size())
	{
		m_vecMaterial[iContainer].push_back(CreateSubset());
	}

	PSubsetMaterial	pMaterial = m_vecMaterial[iContainer][iSubset];

	SAFE_RELEASE(pMaterial->pDiffuseTex);
	GET_SINGLE(CResourcesManager)->CreateTexture(strKey, vecFileName, strPathKey);
	pMaterial->pDiffuseTex = GET_SINGLE(CResourcesManager)->FindTexture(strKey);
	pMaterial->iDiffuseTexRegister = iRegister;
}

void CMaterial::SetDiffuseTexFromFullPath(int iRegister, const string & strKey, const vector<const TCHAR*>& vecFullPath, int iContainer, int iSubset)
{
	if (iContainer >= m_vecMaterial.size())
	{
		vector<PSubsetMaterial>	vec;
		m_vecMaterial.push_back(vec);
	}

	if (iSubset >= m_vecMaterial[iContainer].size())
	{
		m_vecMaterial[iContainer].push_back(CreateSubset());
	}

	PSubsetMaterial	pMaterial = m_vecMaterial[iContainer][iSubset];

	SAFE_RELEASE(pMaterial->pDiffuseTex);
	GET_SINGLE(CResourcesManager)->CreateTextureFromFullPath(strKey, vecFullPath);
	pMaterial->pDiffuseTex = GET_SINGLE(CResourcesManager)->FindTexture(strKey);
	pMaterial->iDiffuseTexRegister = iRegister;
}

void CMaterial::SetSampler(int _iRegister, const string& _strKey, int _iContainer, int _iSubset)
{
	// Container
	if (_iContainer >= (int)m_vecMaterial.size())
	{
		vector<PSubsetMaterial>	vec;
		m_vecMaterial.push_back(vec);
	}

	// Subset
	if (_iSubset >= (int)m_vecMaterial[_iContainer].size())
	{
		m_vecMaterial[_iContainer].push_back(CreateSubset());
	}

	// SubsetMaterial Sampler 설정
	// > 기존에 Sampler가 존재한다면 해제
	PSubsetMaterial	pMaterial = m_vecMaterial[_iContainer][_iSubset];
	SAFE_RELEASE(pMaterial->pSampler);
	pMaterial->pSampler = GET_SINGLE(CResourcesManager)->FindSampler(_strKey);
	pMaterial->iDiffuseSamplerRegister = _iRegister;
}

void CMaterial::AddMultiTex(int iSmpRegister, const string & strSmpKey, int iRegister, const string & strKey, 
							const TCHAR * pFileName, const string & strPathKey, int iContainer, int iSubset)
{
	if (iContainer >= m_vecMaterial.size())
	{
		vector<PSubsetMaterial>	vec;
		m_vecMaterial.push_back(vec);
	}

	if (iSubset >= m_vecMaterial[iContainer].size())
	{
		m_vecMaterial[iContainer].push_back(CreateSubset());
	}

	PSubsetMaterial	pMaterial = m_vecMaterial[iContainer][iSubset];

	if (pMaterial->vecMultiTex.size() == 4)
		return;

	TextureSet	tSet = {};

	tSet.iRegister = iRegister;
	tSet.iSamplerRegister = iSmpRegister;

	GET_SINGLE(CResourcesManager)->CreateTexture(strKey, pFileName, strPathKey);
	tSet.pTex = GET_SINGLE(CResourcesManager)->FindTexture(strKey);

	tSet.pSampler = GET_SINGLE(CResourcesManager)->FindSampler(strKey);

	pMaterial->vecMultiTex.push_back(tSet);
}

void CMaterial::SetShader(int _iContainer, int _iSubset)
{
	// Pixel Shader - 텍스처 처리
	PSubsetMaterial pMaterial = m_vecMaterial[_iContainer][_iSubset];
	pMaterial->tMaterial.iMirrorX = m_isFlipX;
	pMaterial->tMaterial.iMirrorY = m_isFlipY;

	// 상수 버퍼 업데이트 
	// > Material Data
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Material", &pMaterial->tMaterial);

	// 셰이더 자원 준비 단계 - Diffuse (난 반사광)
	if (pMaterial->pDiffuseTex != nullptr)
	{
		pMaterial->pDiffuseTex->SetShader(pMaterial->iDiffuseTexRegister);
	}

	// 셰이더 자원 준비 단계 - Sampler (텍스처 필터링 방식)
	if (pMaterial->pSampler != nullptr)
	{
		pMaterial->pSampler->SetShader(pMaterial->iDiffuseSamplerRegister);
	}

	for (size_t i = 0; i < pMaterial->vecMultiTex.size(); ++i)
	{
		pMaterial->vecMultiTex[i].pTex->SetShader(pMaterial->vecMultiTex[i].iRegister);
		pMaterial->vecMultiTex[i].pSampler->SetShader(pMaterial->vecMultiTex[i].iSamplerRegister);
	}
}

bool CMaterial::Init()
{
	SetMaterial(Vector4::White);
	return true;
}

int CMaterial::Input(float _fTime)
{
	return 0;
}

int CMaterial::Update(float _fTime)
{
	return 0;
}

int CMaterial::LateUpdate(float _fTime)
{
	return 0;
}

void CMaterial::Collision(float _fTime)
{
}

void CMaterial::Render(float _fTime)
{
}

CMaterial* CMaterial::Clone()
{
	return new CMaterial(*this);
}