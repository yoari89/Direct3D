#pragma once
#include "Utility/EnumFlag.h"
#include "ReferenceCnt.h"

ENGINE_BEGIN
class ENGINE_DLL CShader : public CReferenceCnt
{
public:
	CShader();
	~CShader();

private:
	friend class CShaderManager;

private:
	string m_strName;

/* ���̴� */
public:
	bool LoadShader(const string& _strName, const wchar_t* _pFileName,
					char* _pEntry[ST_END],  const string&  _strPathKey = SHADER_PATH);
	void SetShader();

/* ���ؽ� ���̴� */
private:
	ID3DBlob*			m_pVSBlob;
	ID3D11VertexShader* m_pVS;
	bool LoadVertexShader(const wchar_t* _pFileName, char* pEntry, const string& _strPathKey = SHADER_PATH);

public:
	void*  GetVSCode()		const;
	SIZE_T GetVSCodeSize()	const;

/* �ȼ� ���̴� */
private:
	ID3DBlob*			m_pPSBlob;
	ID3D11PixelShader*	m_pPS;
	bool LoadPixelShader(const wchar_t* _pFileName, char* pEntry, const string& _strPathKey = SHADER_PATH);
};
ENGINE_END