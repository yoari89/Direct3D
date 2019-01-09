#include "PCHEngine.h"
#include "Texture.h"
#include "atlstr.h"  
#include "../Device.h"
#include "../Utility/Path/PathManager.h"
#include "../Utility/Thread/ThreadManager.h"

ENGINE_USING

CTexture::CTexture() :
	m_pSRV(nullptr)
{
	memset(m_strFullPath, 0, sizeof(TCHAR)* MAX_PATH);
}

CTexture::~CTexture()
{
	SAFE_RELEASE(m_pSRV);
	Safe_Delete_VecList<vector<ScratchImage*>>(m_vecImage);
}

bool CTexture::LoadTexture(const string& _strName, const TCHAR* _pFileName, const string& _strPathKey)
{
	// 텍스처 로드
	const TCHAR* pPath = GET_SINGLE(CPathManager)->FindPath(_strPathKey);
	TCHAR strFullPath[MAX_PATH] = {};
	if (pPath != nullptr)
	{
		lstrcpy(strFullPath, pPath);
	}
	lstrcat(strFullPath, _pFileName);

	return LoadTextureFromFullPath(_strName, strFullPath);
}

bool CTexture::LoadTextureFromFullPath(const string& _strName, const TCHAR* _pFullPath)
{
	SetTag(_strName);
	lstrcpy(m_strFullPath, _pFullPath);

	TCHAR strExt[_MAX_EXT]	= {};
	char  ext[_MAX_EXT]		= {};

#ifdef UNICODE
	_wsplitpath_s(_pFullPath, nullptr, 0, nullptr, 0, nullptr, 0, strExt, _MAX_EXT);
	WideCharToMultiByte(CP_UTF8, 0, strExt, -1, ext, lstrlen(strExt), 0, 0);
	
	// CW2A
	/*char* pChar = CW2A(strExt);
	strcpy_s(ext, pChar);*/

#else
	_splitpath_s(_pFullPath, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);
#endif // UNICODE

	_strupr_s(ext);
	ScratchImage* pImage = new ScratchImage;

	// 확장자 .DDS
	if (strcmp(ext, ".DDS") == 0)
	{
		if (FAILED(LoadFromDDSFile(_pFullPath, DDS_FLAGS_NONE, nullptr, *pImage)))
		{
			return false;
		}
	}

	// .TGA
	else if (strcmp(ext, ".TGA") == 0)
	{
		if (FAILED(LoadFromTGAFile(_pFullPath, nullptr, *pImage)))
		{
			return false;
		}
	}
	
	// 기타
	else
	{
		if (FAILED(LoadFromWICFile(_pFullPath, WIC_FLAGS_NONE, nullptr, *pImage)))
		{
			return false;
		}
	}

	// ScratchImage 객체 추가
	m_vecImage.push_back(pImage);

	// ShaderResourceView 생성
	return CreateShaderResource();
}

bool CTexture::LoadTexture(const string& _strName, const vector<const TCHAR*>& _vecFileName, const string& _strPathKey)
{
	SetTag(_strName);

	for (size_t i = 0; i < _vecFileName.size(); ++i)
	{
		const TCHAR* pPath = GET_SINGLE(CPathManager)->FindPath(_strPathKey);
		TCHAR strFullPath[MAX_PATH] = {};

		if (pPath)
		{
			lstrcpy(strFullPath, pPath);
		}
		lstrcat(strFullPath, _vecFileName[i]);
		memcpy(m_strFullPath, strFullPath, sizeof(TCHAR) * MAX_PATH);

		TCHAR strExt[_MAX_EXT]	= {};
		char  ext[_MAX_EXT]		= {};

#ifdef UNICODE
		_wsplitpath_s(_vecFileName[i], nullptr, 0, nullptr, 0, nullptr, 0, strExt, _MAX_EXT);
		WideCharToMultiByte(CP_UTF8, 0, strExt, -1, ext, lstrlen(strExt), 0, 0);
#else
		_splitpath_s(vecFileName[i], nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);
#endif // UNICODE

		_strupr_s(ext);
		ScratchImage* pImage = new ScratchImage;

		if (strcmp(ext, ".DDS") == 0)
		{
			if (FAILED(LoadFromDDSFile(m_strFullPath, DDS_FLAGS_NONE, nullptr, *pImage)))
			{
				return false;
			}
		}
		else if (strcmp(ext, ".TGA") == 0)
		{
			if (FAILED(LoadFromTGAFile(m_strFullPath, nullptr, *pImage)))
			{
				return false;
			}
		}
		else
		{
			if (FAILED(LoadFromWICFile(m_strFullPath, WIC_FLAGS_NONE, nullptr, *pImage)))
			{
				return false;
			}
		}
		m_vecImage.push_back(pImage);
	}

	return CreateShaderResourceArray();
}

bool CTexture::LoadTextureFromFullPath(const string& _strName, const vector<const TCHAR*>& _vecFullPath)
{
	SetTag(_strName);

	for (size_t i = 0; i < _vecFullPath.size(); ++i)
	{
		memcpy(m_strFullPath, _vecFullPath[i], sizeof(TCHAR) * MAX_PATH);

		TCHAR	strExt[_MAX_EXT] = {};
		char	ext[_MAX_EXT] = {};

#ifdef UNICODE
		_wsplitpath_s(_vecFullPath[i], nullptr, 0, nullptr, 0, nullptr, 0, strExt, _MAX_EXT);
		WideCharToMultiByte(CP_UTF8, 0, strExt, -1, ext, lstrlen(strExt), 0, 0);
#else
		_splitpath_s(vecFullPath[i], nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);
#endif // UNICODE

		_strupr_s(ext);

		ScratchImage* pImage = new ScratchImage;

		if (strcmp(ext, ".DDS") == 0)
		{
			if (FAILED(LoadFromDDSFile(m_strFullPath, DDS_FLAGS_NONE, nullptr, *pImage)))
			{
				return false;
			}
		}
		else if (strcmp(ext, ".TGA") == 0)
		{
			if (FAILED(LoadFromTGAFile(m_strFullPath, nullptr, *pImage)))
			{
				return false;
			}
		}
		else
		{
			if (FAILED(LoadFromWICFile(m_strFullPath, WIC_FLAGS_NONE, nullptr, *pImage)))
			{
				return false;
			}
		}
		m_vecImage.push_back(pImage);
	}

	return CreateShaderResourceArray();
}

void CTexture::SetShader(int _iRegister)
{
	// 픽셀 셰이더 SlotNum(iRegister)에 ShaderResourceView 설정
	CONTEXT->PSSetShaderResources(_iRegister, 1, &m_pSRV);
}

int CTexture::GetWidth() const
{
	return (int)m_vecImage[0]->GetImage(0, 0, 0)->width;
}

int CTexture::GetHeight() const
{
	return (int)m_vecImage[0]->GetImage(0, 0, 0)->height;
}

const uint8_t* CTexture::GetPixels() const
{
	return m_vecImage[0]->GetPixels();
}

size_t CTexture::GetPixelSize() const
{
	return m_vecImage[0]->GetPixelsSize();
}

bool CTexture::CreateShaderResource()
{
	if (FAILED(CreateShaderResourceView(DEVICE, m_vecImage[0]->GetImages(),
										m_vecImage[0]->GetImageCount(),
										m_vecImage[0]->GetMetadata(), &m_pSRV)))
	{
		return false;
	}

	return true;
}

bool CTexture::CreateShaderResourceArray()
{
	vector<ID3D11Texture2D*>	vecTexture;

	for (size_t i = 0; i < m_vecImage.size(); ++i)
	{
		ID3D11Texture2D*	pTexture = NULL;

		if (FAILED(DirectX::CreateTextureEx(DEVICE, m_vecImage[i]->GetImages(),
			m_vecImage[i]->GetImageCount(),
			m_vecImage[i]->GetMetadata(),
			D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ, 0, FALSE,
			(ID3D11Resource**)&pTexture)))
		{
			assert(false);
			return false;
		}

		vecTexture.push_back(pTexture);
	}

	D3D11_TEXTURE2D_DESC	tTexDesc = {};
	vecTexture[0]->GetDesc(&tTexDesc);

	// Array Texture를 만들기 위한 Desc 구조체를 생성한다.
	D3D11_TEXTURE2D_DESC	tArrayDesc = {};
	tArrayDesc.Width = tTexDesc.Width;
	tArrayDesc.Height = tTexDesc.Height;
	tArrayDesc.MipLevels = tTexDesc.MipLevels;
	tArrayDesc.ArraySize = vecTexture.size();
	tArrayDesc.Format = tTexDesc.Format;
	tArrayDesc.SampleDesc.Count = 1;
	tArrayDesc.SampleDesc.Quality = 0;
	tArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	tArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// 위에서 설정한 정보를 이용해서 비어있는 Array Texture를 생성한다.
	ID3D11Texture2D*	pArrayTexture = NULL;
	if (FAILED(DEVICE->CreateTexture2D(&tArrayDesc, NULL, &pArrayTexture)))
	{
		assert(false);
		return false;
	}

	CRITICAL_SECTION*	pCrt = GET_SINGLE(CThreadManager)->GetLoadSafe();
	if (pCrt)
		EnterCriticalSection(pCrt);

	// Array Texture의 픽셀정보를 불러온 텍스쳐의 픽셀정보로 채워준다.
	for (size_t i = 0; i < vecTexture.size(); ++i)
	{
		// 각각의 텍스쳐들을 밉맵 수준만큼 반복한다.
		for (size_t mip = 0; mip < tTexDesc.MipLevels; ++mip)
		{
			// 텍스처를 Map을 걸어준다.
			D3D11_MAPPED_SUBRESOURCE	tMap = {};
			CONTEXT->Map(vecTexture[i], mip, D3D11_MAP_READ, 0, &tMap);

			CONTEXT->UpdateSubresource(pArrayTexture,
				D3D11CalcSubresource(mip, i, tTexDesc.MipLevels),
				NULL, tMap.pData, tMap.RowPitch, tMap.DepthPitch);

			CONTEXT->Unmap(vecTexture[i], mip);
		}
	}

	if (pCrt)
		LeaveCriticalSection(pCrt);

	D3D11_SHADER_RESOURCE_VIEW_DESC	tViewDesc = {};
	tViewDesc.Format = tArrayDesc.Format;
	tViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	tViewDesc.Texture2DArray.MostDetailedMip = 0;
	tViewDesc.Texture2DArray.MipLevels = tArrayDesc.MipLevels;
	tViewDesc.Texture2DArray.FirstArraySlice = 0;
	tViewDesc.Texture2DArray.ArraySize = vecTexture.size();

	if (FAILED(DEVICE->CreateShaderResourceView(pArrayTexture, &tViewDesc, &m_pSRV)))
	{
		assert(false);
		return false;
	}

	SAFE_RELEASE(pArrayTexture);

	Safe_Release_VecList(vecTexture);

	return true;
}
