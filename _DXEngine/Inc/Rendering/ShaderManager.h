#pragma once

ENGINE_BEGIN
class CShader;
class ENGINE_DLL CShaderManager
{
DECLARE_SINGLE(CShaderManager)

/* Shader 관리 */
private:
	unordered_map<string, CShader*>	m_mapShader; 

public:
	bool	 Init();
	void	 LoadLogoShader();
	void	 LoadFadeShader();

	CShader* FindShader(const string& _strName);
	bool	 LoadShader(const string& _strName, const wchar_t* _pFileName,
						char* _pEntry[ST_END], const string& _strPathKey = SHADER_PATH);

/* Input Layout(입력 배치) */
private:
	int	m_iInputSize;
	unordered_map<string, ID3D11InputLayout*> m_mapInputLayout;
	vector<D3D11_INPUT_ELEMENT_DESC>		  m_vecInputDesc;

public:
	ID3D11InputLayout* FindInputLayout(const string& _strName);
	void AddInputElement(char* _pSemantic, int _iIdx, DXGI_FORMAT _eFmt, int _iSize, int _iInputSlot = 0,
						 D3D11_INPUT_CLASSIFICATION _eInputClass = D3D11_INPUT_PER_VERTEX_DATA, 
						 int _iInstanceStepRate = 0);
	bool CreateInputLayout(const string& _strName, const string& _strShaderKey);

/* 상수 버퍼 */
private:
	unordered_map<string, PCBuffer>	m_mapCBuffer;

private:
	PCBuffer FindCBuffer(const string& strName);

public:
	bool CreateCBuffer(const string& _strName, int _iRegister, int _iSize, int _iShaderType);
	bool UpdateCBuffer(const string& _strName, void* _pData);
};
ENGINE_END