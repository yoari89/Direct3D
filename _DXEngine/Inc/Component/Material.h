#pragma once
#include "Component/Component.h"
#include "Resources/ResourcesManager.h"

ENGINE_BEGIN
class CTexture;
class CSampler;

/* Subset Matrerial */
// > 부위마다 세부 재질이 들어갈 수 있다.

typedef struct ENGINE_DLL _tagTextureSet
{
	class CTexture*	pTex;
	int				iRegister;
	class CSampler*	pSampler;
	int				iSamplerRegister;

	_tagTextureSet() :
		pTex(nullptr),
		iRegister(-1),
		pSampler(nullptr),
		iSamplerRegister(0)
	{
	}
}TextureSet, *PTextureSet;

typedef struct ENGINE_DLL _tagSubsetMaterial
{
	Material	tMaterial;
	CTexture*	pDiffuseTex;				// Texture
	CSampler*	pSampler;					// Sampler
	int			iDiffuseTexRegister;		// Texture Register Slot
	int			iDiffuseSamplerRegister;	// Sampler Register Slot
	vector<TextureSet>	vecMultiTex;

	_tagSubsetMaterial() :
		pDiffuseTex(nullptr),
		iDiffuseTexRegister(-1),
		pSampler(nullptr),
		iDiffuseSamplerRegister(0)
	{
	}
}SubsetMaterial, *PSubsetMaterial;

// ============================================================================================= //

class ENGINE_DLL CMaterial : public CComponent
{
private:
	friend class CGameObject;

private :
	CMaterial();
	CMaterial(const CMaterial& _material);
	~CMaterial();

private:
	vector<vector<PSubsetMaterial>> m_vecMaterial;

/* Material 정보 */
private:
	// > Subset
	PSubsetMaterial CreateSubset();

public:
	// > Material(재질)
	void SetMaterial(const Vector4& _vDiffuse, int _iContainer = 0, int _iSubset = 0);

	// > Diffuse(난 반사광)
	void SetDiffuseTex(int _iRegister, const string& _strKey, int _iContainer = 0, int _iSubset = 0);
	void SetDiffuseTex(int _iRegister, const string& _strKey, const TCHAR* _pFileName,
						const string& _strPathKey = TEXTURE_PATH, int _iContainer = 0, int _iSubset = 0);
	void SetDiffuseTexFromFullPath(int _iRegister, const string& _strKey, const TCHAR* _pFullPath,
								   int _iContainer = 0, int _iSubset = 0);
	void SetDiffuseTex(int _iRegister, CTexture* _pTex, int _iContainer = 0, int _isubSet = 0);

	void SetDiffuseTex(int iRegister, const string& strKey, const vector<const TCHAR*>& vecFileName,
					   const string& strPathKey = TEXTURE_PATH,
					   int iContainer = 0, int iSubset = 0);
	void SetDiffuseTexFromFullPath(int iRegister, const string& strKey,
								   const vector<const TCHAR*>& vecFullPath, int iContainer = 0, int iSubset = 0);


	

	/*void AddMultiTex(int iRegister, const string& strKey, int iContainer = 0, int iSubset = 0);
	void AddMultiTex(int iRegister, const string& strKey, const TCHAR* pFileName,
					 const string& strPathKey = TEXTURE_PATH, int iContainer = 0, int iSubset = 0);
	void AddMultiTexFromFullPath(int iRegister, const string& strKey,
								 const TCHAR* pFullPath, int iContainer = 0, int iSubset = 0);
	void AddMultiTex(int iRegister, const string& strKey, const vector<const TCHAR*>& vecFileName,
					 const string& strPathKey = TEXTURE_PATH, int iContainer = 0, int iSubset = 0);
	void AddMultiTexFromFullPath(int iRegister, const string& strKey,
								 const vector<const TCHAR*>& vecFullPath, int iContainer = 0, int iSubset = 0);
	void AddMultiTex(int iRegister, class CTexture* pTex, int iContainer = 0, int iSubset = 0);
	void AddMultiSampler(int iRegister, const string& strKey, int iContainer = 0, int iSubset = 0);*/
	
	// > 샘플링(필터 방식)
	void SetSampler(int _iRegister, const string& _strKey, int _iContainer = 0, int _iSubset = 0);
	void AddMultiTex(int iSmpRegister, const string& strSmpKey, int iRegister,
					 const string& strKey, const TCHAR* pFileName, const string& strPathKey = TEXTURE_PATH,
					 int iContainer = 0, int iSubset = 0);

/* 셰이더 결합 */
public:
	void SetShader(int _iContainer, int _iSubset);

public:
	int m_isFlipX = 0;
	int m_isFlipY = 0;
	void SetFlipX(int _isFlipX)
	{
		m_isFlipX = _isFlipX;
	}
	void SetFlipY(int _isFlipY)
	{
		m_isFlipY = _isFlipY;
	}

/* 상속 받은 재정의 함수 */
public:
	bool Init()					  override;
	int  Input(float _fTime)	  override;
	int  Update(float _fTime)	  override;
	int  LateUpdate(float _fTime) override;
	void Collision(float _fTime)  override;
	void Render(float _fTime)	  override;
	CMaterial* Clone()			  override;
};
ENGINE_END