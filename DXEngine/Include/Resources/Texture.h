#pragma once
#include "../ReferenceCnt.h"
#include "DirectXTex.h"

ENGINE_BEGIN
class ENGINE_DLL CTexture : public CReferenceCnt
{
private:
	friend class CResourcesManager;

private:
	CTexture();
	~CTexture();

/* 객체 */
// > ScratchImage : 이미지 또는 이미지 집합의 메모리를 관리
//   해당 객체를 이용하여 ShaderResourceView를 생성할 수 있다.
// > ID3D11ShaderResourceView : Direct3D11 에서 세이더에 리소스를 할당하는 인터페이스
private:
	vector<ScratchImage*>		m_vecImage;
	ID3D11ShaderResourceView*	m_pSRV;
	TCHAR m_strFullPath[MAX_PATH];

/* 텍스처 로드 */
public:
	// > Atlas
	bool LoadTexture(const string& _strName, const TCHAR* _pFileName,
					 const string& _strPathKey = TEXTURE_PATH);
	bool LoadTextureFromFullPath(const string& _strName, const TCHAR* _pFullPath);

	// > Frame
	bool LoadTexture(const string& _strName, const vector<const TCHAR*>& _vecFileName,
					 const string& _strPathKey = TEXTURE_PATH);
	bool LoadTextureFromFullPath(const string& _strName, const vector<const TCHAR*>& _vecFullPath);
	void SetShader(int _iRegister);

/* 텍스처 정보 */
public:
	int GetWidth()				const;
	int GetHeight()				const;
	const uint8_t* GetPixels()	const;
	size_t GetPixelSize()		const;

private:
	bool CreateShaderResource();
	bool CreateShaderResourceArray();
};
ENGINE_END