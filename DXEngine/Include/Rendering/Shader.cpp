#include "PCHEngine.h"
#include "Shader.h"
#include "../Device.h"
#include "Utility/Path/PathManager.h"

ENGINE_USING

CShader::CShader() :
	m_pVSBlob(nullptr),
	m_pPSBlob(nullptr),
	m_pVS(nullptr),
	m_pPS(nullptr)
{
}

CShader::~CShader()
{
	SAFE_RELEASE(m_pVSBlob);
	SAFE_RELEASE(m_pPSBlob);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
}

bool CShader::LoadShader(const string& _strName, const wchar_t* _pFileName,
						 char* _pEntry[ST_END], const string& _strPathKey)
{
	// 태그명
	SetTag(_strName);

	// 셰이더 파일 경로
	const wchar_t* pPath = GET_SINGLE(CPathManager)->FindPath(_strPathKey);
	wstring strFullPath = L"";
	if (pPath != nullptr)
	{
		strFullPath += pPath;
	}
	strFullPath += _pFileName;

	// 버텍스 셰이더 로드
	if (LoadVertexShader(_pFileName, _pEntry[ST_VERTEX], _strPathKey) == false)
	{
		return false;
	}

	// 픽셀 셰이더 로드
	if (LoadPixelShader(_pFileName, _pEntry[ST_PIXEL], _strPathKey) == false)
	{
		return false;
	}

	return true;
}

void CShader::SetShader()
{
	// 렌더링 파이프라인(각 셰이더 단계)에 로드된 셰이더를 결합
	CONTEXT->VSSetShader(m_pVS, nullptr, 0); // Vertex Shader
	CONTEXT->PSSetShader(m_pPS, nullptr, 0); // Pixel Shader
}

bool CShader::LoadVertexShader(const wchar_t* _pFileName, char* _pEntry, const string& _strPathKey)
{
	// 셰이더 파일명
	const wchar_t* pPath = GET_SINGLE(CPathManager)->FindPath(_strPathKey);
	wstring strFullPath;
	if (pPath != nullptr)
	{
		strFullPath = pPath;
	}
	strFullPath += _pFileName;

	// HLSL 파일 컴파일
	DWORD dwFlag = 0;
#ifdef _DEBUG
	dwFlag = D3DCOMPILE_DEBUG;
#endif // _DEBUG

	ID3DBlob* pError = nullptr;
	if (FAILED(D3DCompileFromFile(strFullPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
								  _pEntry, "vs_5_0", dwFlag, 0, &m_pVSBlob, &pError)))
	{
		OutputDebugStringA((char*)pError->GetBufferPointer());
		return false;
	}

	// Blob 정보를 이용하여 버텍스 셰이더 생성
	if (FAILED(DEVICE->CreateVertexShader(m_pVSBlob->GetBufferPointer(),
										  m_pVSBlob->GetBufferSize(), nullptr, &m_pVS)))
	{
		return false;
	}
	
	return true;
}

void* CShader::GetVSCode() const
{
	return m_pVSBlob->GetBufferPointer();
}

SIZE_T CShader::GetVSCodeSize() const
{
	return m_pVSBlob->GetBufferSize();
}

bool CShader::LoadPixelShader(const wchar_t* _pFileName, char* _pEntry, const string& _strPathKey)
{
	// 셰이더 파일명
	const wchar_t* pPath = GET_SINGLE(CPathManager)->FindPath(_strPathKey);
	wstring strFullPath;
	if (pPath != nullptr)
	{
		strFullPath = pPath;
	}
	strFullPath += _pFileName;

	// HLSL 파일을 컴파일
	DWORD dwFlag = 0;

#ifdef _DEBUG
	dwFlag = D3DCOMPILE_DEBUG;
#endif // _DEBUG

	ID3DBlob* pError = nullptr;
	if (FAILED(D3DCompileFromFile(strFullPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
								  _pEntry, "ps_5_0", dwFlag, 0, &m_pPSBlob, &pError)))
	{
		OutputDebugStringA((char*)pError->GetBufferPointer());
		return false;
	}

	// Blob 정보를 이용하여 픽셀 셰이더 생성
	if (FAILED(DEVICE->CreatePixelShader(m_pPSBlob->GetBufferPointer(),
										 m_pPSBlob->GetBufferSize(), nullptr, &m_pPS)))
	{
		return false;
	}
	
	return true;
}