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
	// �±׸�
	SetTag(_strName);

	// ���̴� ���� ���
	const wchar_t* pPath = GET_SINGLE(CPathManager)->FindPath(_strPathKey);
	wstring strFullPath = L"";
	if (pPath != nullptr)
	{
		strFullPath += pPath;
	}
	strFullPath += _pFileName;

	// ���ؽ� ���̴� �ε�
	if (LoadVertexShader(_pFileName, _pEntry[ST_VERTEX], _strPathKey) == false)
	{
		return false;
	}

	// �ȼ� ���̴� �ε�
	if (LoadPixelShader(_pFileName, _pEntry[ST_PIXEL], _strPathKey) == false)
	{
		return false;
	}

	return true;
}

void CShader::SetShader()
{
	// ������ ����������(�� ���̴� �ܰ�)�� �ε�� ���̴��� ����
	CONTEXT->VSSetShader(m_pVS, nullptr, 0); // Vertex Shader
	CONTEXT->PSSetShader(m_pPS, nullptr, 0); // Pixel Shader
}

bool CShader::LoadVertexShader(const wchar_t* _pFileName, char* _pEntry, const string& _strPathKey)
{
	// ���̴� ���ϸ�
	const wchar_t* pPath = GET_SINGLE(CPathManager)->FindPath(_strPathKey);
	wstring strFullPath;
	if (pPath != nullptr)
	{
		strFullPath = pPath;
	}
	strFullPath += _pFileName;

	// HLSL ���� ������
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

	// Blob ������ �̿��Ͽ� ���ؽ� ���̴� ����
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
	// ���̴� ���ϸ�
	const wchar_t* pPath = GET_SINGLE(CPathManager)->FindPath(_strPathKey);
	wstring strFullPath;
	if (pPath != nullptr)
	{
		strFullPath = pPath;
	}
	strFullPath += _pFileName;

	// HLSL ������ ������
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

	// Blob ������ �̿��Ͽ� �ȼ� ���̴� ����
	if (FAILED(DEVICE->CreatePixelShader(m_pPSBlob->GetBufferPointer(),
										 m_pPSBlob->GetBufferSize(), nullptr, &m_pPS)))
	{
		return false;
	}
	
	return true;
}