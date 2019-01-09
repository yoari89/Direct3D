#pragma once

ENGINE_BEGIN
class CMesh;
class CTexture;
class CSampler;
class ENGINE_DLL CResourcesManager
{
DECLARE_SINGLE(CResourcesManager)

public:
	bool Init();

/* 메시 */
private:
	unordered_map<string, CMesh*> m_mapMesh;

public:
	bool CreateDefaultTriMesh();
	bool CreateDefaultRectMesh();
	bool CreateDefaultTexMesh();
	bool CreateColliderMesh();
	bool CreateMesh(const string& _strName, const string& _strShaderKey, const string& _strLayoutKey,
					void* _pVtx, int _iVtxCount, int _iVtxSize, D3D11_USAGE _eVtxUsage,
					D3D11_PRIMITIVE_TOPOLOGY _ePrimitive, void* _pIdx = nullptr, int _iIdxCount = 0,
					int _iIdxSize = 0, D3D11_USAGE _eIdxUsage = D3D11_USAGE_DEFAULT,
					DXGI_FORMAT eFmt = DXGI_FORMAT_R16_UINT);
	CMesh* FindMesh(const string& _strName);

/* 텍스처 */
private:
	unordered_map<string, CTexture*> m_mapTexture;
	
public:
	bool CreateTexture(const string& _strName, const TCHAR* _pFileName,
					   const string& _strPathKey = TEXTURE_PATH);
	bool CreateTexture(const char* _strName, const TCHAR* _pFileName,
						const string& _strPathKey = TEXTURE_PATH);

	bool CreateTextureFromFullPath(const string& _strName, const TCHAR* _pFullPath);
	
	bool CreateTexture(const string& strName, const vector<const TCHAR*>& vecFileName,
					   const string& strPathKey = TEXTURE_PATH);
	bool CreateTextureFromFullPath(const string& strName, const vector<const TCHAR*>& vecFullPath);

	CTexture* FindTexture(const string& _strName);

/* 샘플러 */
private:
	unordered_map<string, CSampler*> m_mapSampler;

public:
	bool CreateSampler(const string& _strName,
					   D3D11_FILTER _eFilter		  = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
					   D3D11_TEXTURE_ADDRESS_MODE _eU = D3D11_TEXTURE_ADDRESS_WRAP,
					   D3D11_TEXTURE_ADDRESS_MODE _eV = D3D11_TEXTURE_ADDRESS_WRAP,
					   D3D11_TEXTURE_ADDRESS_MODE _eW = D3D11_TEXTURE_ADDRESS_WRAP);
	CSampler* FindSampler(const string& _strName);
};
ENGINE_END