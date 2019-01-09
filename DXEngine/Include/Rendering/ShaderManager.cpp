#include "PCHEngine.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "../Device.h"
#include "Utility/Types.h"
#include "Utility/Thread/ThreadManager.h"

ENGINE_USING
DEFINITION_SINGLE(CShaderManager);

CShaderManager::CShaderManager() : m_iInputSize(0)
{
}

CShaderManager::~CShaderManager()
{
	// _tagCBuffer
	unordered_map<string, PCBuffer>::iterator iter;
	unordered_map<string, PCBuffer>::iterator iterEnd = m_mapCBuffer.end();
	for (iter = m_mapCBuffer.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second->pBuffer); // ���� ���� �޸� ����
		SAFE_DELETE(iter->second);			 // new �Ҵ� ����
	}
	m_mapCBuffer.clear();

	// unordered_map<string, CShader*> ����
	Safe_Release_Map(m_mapShader);

	// unordered_map<string, ID3D11InputLayout*> ����
	Safe_Release_Map(m_mapInputLayout);
}

bool CShaderManager::Init()
{
	// ���̴� ������
	char* pEntry[ST_END] = {};
	pEntry[ST_VERTEX] = "StandardColorVS";
	pEntry[ST_PIXEL]  = "StandardColorPS";

	// ���̴� ���� �ε�
	// > Shader Key : "StandardColorShader"
	if (LoadShader(STANDARD_COLOR_SHADER, TEXT("Standard.fx"), pEntry) == false)
	{
		return false;
	}

	// Input Element(�Է� �Ӽ�) ����
	// StandardColorVS(VS_INPUT_COLOR input)
	// > VS_INPUT_COLOR(float3 position, float4 color), total 28byte
	//   12Byte = POSITION
	//   16Byte = COLOR
	// > StandardColorPS(VS_OUTPUT_COLOR input) ��� �뵵
	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12);
	AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16);

	// InputLayout ����
	// > Input Layout Key : "PosColorLayout"
	// > Shader Key : "StandardColorShader"
	if (CreateInputLayout(POS_COLOR_LAYOUT, STANDARD_COLOR_SHADER) == false)
	{
		return false;
	}

	///////////////////////////////////////////////////////////////////////////////////////// 

	pEntry[ST_VERTEX] = "StandardTexVS";
	pEntry[ST_PIXEL] = "StandardTexPS";
	if (LoadShader(STANDARD_TEX_SHADER, TEXT("Standard.fx"), pEntry) == false)
	{
		return false;
	}

	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12);
	AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 8);

	if (CreateInputLayout(POS_UV_LAYOUT, STANDARD_TEX_SHADER) == false)
		return false;

	/////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
	pEntry[ST_VERTEX] = "ColliderVS";
	pEntry[ST_PIXEL] = "ColliderPS";
	if (LoadShader(COLLIDER_SHADER, TEXT("Collider.fx"), pEntry) == false)
	{
		return false;
	}

	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12);

	if (CreateInputLayout(POS_LAYOUT, COLLIDER_SHADER) == false)
	{
		return false;
	}
#endif	// _DEBUG

	pEntry[ST_VERTEX] = "UIVS";
	pEntry[ST_PIXEL] = "ButtonPS";
	if (!LoadShader(BUTTON_SHADER, TEXT("UI.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "UIVS";
	pEntry[ST_PIXEL] = "BarPS";
	if (!LoadShader(BAR_SHADER, TEXT("UI.fx"), pEntry))
		return false;

	// Transform ��� ���� ����
	CreateCBuffer("Transform", 0, sizeof(TransformCBuffer), CST_VERTEX | CST_PIXEL);

	// Material ��� ���� ����
	CreateCBuffer("Material", 1, sizeof(Material), CST_VERTEX | CST_PIXEL);
	CreateCBuffer("Component", 2, sizeof(ComponentCBuffer), CST_VERTEX | CST_PIXEL);

	// Animation2D ��� ���� ����
	CreateCBuffer("Animation2D", 8, sizeof(Animation2DCBuffer), CST_VERTEX | CST_PIXEL);

	CreateCBuffer("Collider", 8, sizeof(Vector4), CST_VERTEX | CST_PIXEL);

	CreateCBuffer("Button", 9, sizeof(ButtonCBuffer), CST_VERTEX | CST_PIXEL);
	CreateCBuffer("Bar", 9, sizeof(BarCBuffer), CST_VERTEX | CST_PIXEL);



	LoadLogoShader();
	LoadFadeShader();

	return true;
}

void CShaderManager::LoadLogoShader()
{
	// MapleLogo Shader �ε�
	char* pEntry[ST_END] = {};
	pEntry[ST_VERTEX]	 = "MapleLogoVS";
	pEntry[ST_PIXEL]	 = "MapleLogoPS";
	if (LoadShader(MAPLELOGO_SHADER, TEXT("MapleLogoShader.fx"), pEntry) == false)
	{
		tassert(true);
		return;
	}

	// MapleLogoShader.fx ���Ͽ��� ����ϴ� ��� ����
	// > distance(float), empty(float3)
	// > ShaderKey : "MapleLogoCBuffer"
	int registerSlotNum = 0;
	CreateCBuffer("MapleLogoCBuffer", registerSlotNum, sizeof(MapleLogoCBuffer), CST_PIXEL);
}

void CShaderManager::LoadFadeShader()
{
	// Fade Shader �ε�
	char* pEntry[ST_END] = {};
	pEntry[ST_VERTEX] = "FadeVS";
	pEntry[ST_PIXEL]  = "FadePS";
	if (LoadShader(FADE_SHADER, TEXT("ScreenFade.fx"), pEntry) == false)
	{
		tassert(true);
		return;
	}

	int registerSlotNum = 0;
	CreateCBuffer("FadeCBuffer", registerSlotNum, sizeof(FadeCBuffer), CST_PIXEL);
}

CShader* CShaderManager::FindShader(const string& _strName)
{
	unordered_map<string, CShader*>::iterator iter = m_mapShader.find(_strName);
	if (iter == m_mapShader.end())
	{
		return nullptr;
	}
	iter->second->AddRef();
	return iter->second;
}

bool CShaderManager::LoadShader(const string& _strName, const wchar_t* _pFileName, 
								char* _pEntry[ST_END], const string& _strPathKey)
{
	// ��ϵ� ���̴��� ����
	CShader* pShader = FindShader(_strName);
	if (pShader != nullptr)
	{
		// Ref Count -1
		// > FindShader() �Լ� ȣ�� �� Ref Count +1 ���� ����
		SAFE_RELEASE(pShader); 
		return true;
	}

	// �ε�
	pShader = new CShader();
	if (pShader->LoadShader(_strName, _pFileName, _pEntry, _strPathKey) == false)
	{
		// ���н� Ref Count -1
		SAFE_RELEASE(pShader); 
		return false;
	}

	// �߰�
	m_mapShader.insert(make_pair(_strName, pShader));
	return true;
}

ID3D11InputLayout* CShaderManager::FindInputLayout(const string& _strName)
{
	unordered_map<string, ID3D11InputLayout*>::iterator iter = m_mapInputLayout.find(_strName);
	if (iter == m_mapInputLayout.end())
	{
		return nullptr;
	}
	return iter->second;
}

void CShaderManager::AddInputElement(char* _pSemantic, int _iIdx, DXGI_FORMAT _eFmt, int _iSize, int _iInputSlot, 
									 D3D11_INPUT_CLASSIFICATION _eInputClass, int _iInstanceStepRate)
{
	// InputLayout�� ����� ���� Input Element �߰�
	D3D11_INPUT_ELEMENT_DESC tDesc	= {};
	tDesc.SemanticName				= _pSemantic;
	tDesc.SemanticIndex				= _iIdx;
	tDesc.Format					= _eFmt;
	tDesc.AlignedByteOffset			= m_iInputSize;
	tDesc.InputSlot					= _iInputSlot;
	tDesc.InputSlotClass			= _eInputClass;
	tDesc.InstanceDataStepRate		= _iInstanceStepRate;

	// ���� �ʵ� ������ ���� ũ�� ����
	// > "��� ~ �������� ���ؽ��� (��ġ or ���� ..���) ������!
	m_iInputSize += _iSize;

	// �߰�
	m_vecInputDesc.push_back(tDesc);
}

bool CShaderManager::CreateInputLayout(const string& _strName, const string& _strShaderKey)
{
	// ���̴��� ã�� ���� ���
	CShader* pShader = FindShader(_strShaderKey);
	if (pShader == nullptr)
	{
		return false;
	}

	// ������� �ϴ� Input Layout ��ü�� �̹� �����ϴ� ���
	ID3D11InputLayout* pLayout = FindInputLayout(_strName);
	if (pLayout != nullptr)
	{
		return false;
	}

	// Input Layout ����
	// : Blob(������ �� ���̴�) ������ �ʿ��ϴ�.
	if (FAILED(DEVICE->CreateInputLayout(&m_vecInputDesc[0], m_vecInputDesc.size(), 
										 pShader->GetVSCode(), pShader->GetVSCodeSize(), &pLayout)))
	{
		return false;
	}

	SAFE_RELEASE(pShader);

	// Input Element ���� ���� �ʱ�ȭ
	m_iInputSize = 0;
	m_vecInputDesc.clear();

	// �߰�
	m_mapInputLayout.insert(make_pair(_strName, pLayout));
	return true;
}

PCBuffer CShaderManager::FindCBuffer(const string& strName)
{
	unordered_map<string, PCBuffer>::iterator iter = m_mapCBuffer.find(strName);
	if (iter == m_mapCBuffer.end())
	{
		return nullptr;
	}
	return iter->second;
}

bool CShaderManager::CreateCBuffer(const string& _strName, int _iRegister, int _iSize, int _iShaderType)
{
	// ������� �ϴ� ��� ���۰� �̹� �����ϴ� ���
	PCBuffer pBuffer = FindCBuffer(_strName);
	if (pBuffer != nullptr)
	{
		return false;
	}

	// ��� ���� ����
	pBuffer = new CBuffer;
	pBuffer->pBuffer		= nullptr;
	pBuffer->iSize			= _iSize;
	pBuffer->iShaderType	= _iShaderType;
	pBuffer->iRegister		= _iRegister;
	D3D11_BUFFER_DESC tDesc = {};
	tDesc.ByteWidth			= _iSize;
	tDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER; // ��� ���� �뵵
	tDesc.Usage				= D3D11_USAGE_DYNAMIC;		  // Map, UnMap ����Ͽ� ���ҽ� ������Ʈ ���
	tDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	if (FAILED(DEVICE->CreateBuffer(&tDesc, nullptr, &pBuffer->pBuffer)))
	{
		return false;
	}
	
	// �߰�
	m_mapCBuffer.insert(make_pair(_strName, pBuffer));
	return true;
}

bool CShaderManager::UpdateCBuffer(const string& _strName, void* _pData)
{
	PCBuffer pBuffer = FindCBuffer(_strName);
	if (pBuffer == false)
	{
		return false;
	}

	CRITICAL_SECTION*	pCrt = GET_SINGLE(CThreadManager)->GetLoadSafe();
	if (pCrt)
		EnterCriticalSection(pCrt);

	{
		// ��� ���� ������ ����
		// Map
		// > ID3D11Resourcee*		  : �б�, ���� ������ ���ϴ� ���ҽ�(����� ����)�� ����Ű�� ������
		// > UINT					  : �κ��ڿ��� ���� ���X (0)
		// > D3D11_MAP				  : ���ҽ� ���� �÷���
		// > UINT					  : �߰� �÷��� ���X (0)
		// > D3D11_MAPPED_SUBRESOURCE : D3D11_MAPPED_SUBRESOURCE ����Ű�� �����͸� �����ش�. 
		//                              �̸� ���ؼ� ���ۿ� �ڷḦ ����ϰų� ����

		// > ���� : �����̶� �ش� ���� �ٸ� ���� ����Ű���� �ϴ� ���̴�.
		// > ���� ���ҽ��� D3D11_MAPPED_SUBRESOURCE�� pBuffer->pBuffer�� ���εǾ���.
		//   D3D11_MAPPED_SUBRESOURCE ������ �����͸� �����ص� pBuffer->pBuffer�� ���εǾ��� ������
		//   ���̴��� �����͸� ���� �� �Ź� �ֽ��� ���� �����͸� ������ �ȴ�.

		// 1. Map() �Լ��� ȣ���Ͽ� ������ ���� ������ ���� pBuffer->pBuffer ��ü�� 
		//    D3D11_MAPPED_SUBRESOURCE ��ü�� ���ν�Ų��.

		// 2. ��� ���� ������(_pData)�� D3D11_MAPPED_SUBRESOURCE ��ü ����� pData�� �޸� �����Ѵ�.
		//    tMap.pData �޸𸮿��� (void* _pData) ������ ����Ǿ�����.

		// 3. Unmap() �Լ� ȣ��� ������ ���� ������ ���´�.

		D3D11_MAPPED_SUBRESOURCE tMap = {};
		CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);
		memcpy(tMap.pData, _pData, pBuffer->iSize);
		CONTEXT->Unmap(pBuffer->pBuffer, 0);
	}

	if (pCrt)
		LeaveCriticalSection(pCrt);

	// ���ؽ� ���̴� ��� ����
	if (pBuffer->iShaderType & CST_VERTEX)
	{
		CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	}

	// �ȼ� ���̴� ��� ����
	if (pBuffer->iShaderType & CST_PIXEL)
	{
		CONTEXT->PSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	}
	
	return true;
}