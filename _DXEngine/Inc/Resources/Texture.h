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

/* ��ü */
// > ScratchImage : �̹��� �Ǵ� �̹��� ������ �޸𸮸� ����
//   �ش� ��ü�� �̿��Ͽ� ShaderResourceView�� ������ �� �ִ�.
// > ID3D11ShaderResourceView : Direct3D11 ���� ���̴��� ���ҽ��� �Ҵ��ϴ� �������̽�
private:
	vector<ScratchImage*>		m_vecImage;
	ID3D11ShaderResourceView*	m_pSRV;
	TCHAR m_strFullPath[MAX_PATH];

/* �ؽ�ó �ε� */
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

/* �ؽ�ó ���� */
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