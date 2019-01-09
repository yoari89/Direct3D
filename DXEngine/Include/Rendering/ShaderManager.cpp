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
		SAFE_RELEASE(iter->second->pBuffer); // 버퍼 생성 메모리 해제
		SAFE_DELETE(iter->second);			 // new 할당 해제
	}
	m_mapCBuffer.clear();

	// unordered_map<string, CShader*> 해제
	Safe_Release_Map(m_mapShader);

	// unordered_map<string, ID3D11InputLayout*> 해제
	Safe_Release_Map(m_mapInputLayout);
}

bool CShaderManager::Init()
{
	// 셰이더 진입점
	char* pEntry[ST_END] = {};
	pEntry[ST_VERTEX] = "StandardColorVS";
	pEntry[ST_PIXEL]  = "StandardColorPS";

	// 셰이더 파일 로드
	// > Shader Key : "StandardColorShader"
	if (LoadShader(STANDARD_COLOR_SHADER, TEXT("Standard.fx"), pEntry) == false)
	{
		return false;
	}

	// Input Element(입력 속성) 설정
	// StandardColorVS(VS_INPUT_COLOR input)
	// > VS_INPUT_COLOR(float3 position, float4 color), total 28byte
	//   12Byte = POSITION
	//   16Byte = COLOR
	// > StandardColorPS(VS_OUTPUT_COLOR input) 출력 용도
	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12);
	AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16);

	// InputLayout 생성
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

	// Transform 상수 버퍼 생성
	CreateCBuffer("Transform", 0, sizeof(TransformCBuffer), CST_VERTEX | CST_PIXEL);

	// Material 상수 버퍼 생성
	CreateCBuffer("Material", 1, sizeof(Material), CST_VERTEX | CST_PIXEL);
	CreateCBuffer("Component", 2, sizeof(ComponentCBuffer), CST_VERTEX | CST_PIXEL);

	// Animation2D 상수 버퍼 생성
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
	// MapleLogo Shader 로드
	char* pEntry[ST_END] = {};
	pEntry[ST_VERTEX]	 = "MapleLogoVS";
	pEntry[ST_PIXEL]	 = "MapleLogoPS";
	if (LoadShader(MAPLELOGO_SHADER, TEXT("MapleLogoShader.fx"), pEntry) == false)
	{
		tassert(true);
		return;
	}

	// MapleLogoShader.fx 파일에서 사용하는 상수 버퍼
	// > distance(float), empty(float3)
	// > ShaderKey : "MapleLogoCBuffer"
	int registerSlotNum = 0;
	CreateCBuffer("MapleLogoCBuffer", registerSlotNum, sizeof(MapleLogoCBuffer), CST_PIXEL);
}

void CShaderManager::LoadFadeShader()
{
	// Fade Shader 로드
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
	// 등록된 셰이더가 존재
	CShader* pShader = FindShader(_strName);
	if (pShader != nullptr)
	{
		// Ref Count -1
		// > FindShader() 함수 호출 시 Ref Count +1 증가 상태
		SAFE_RELEASE(pShader); 
		return true;
	}

	// 로드
	pShader = new CShader();
	if (pShader->LoadShader(_strName, _pFileName, _pEntry, _strPathKey) == false)
	{
		// 실패시 Ref Count -1
		SAFE_RELEASE(pShader); 
		return false;
	}

	// 추가
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
	// InputLayout을 만들기 위한 Input Element 추가
	D3D11_INPUT_ELEMENT_DESC tDesc	= {};
	tDesc.SemanticName				= _pSemantic;
	tDesc.SemanticIndex				= _iIdx;
	tDesc.Format					= _eFmt;
	tDesc.AlignedByteOffset			= m_iInputSize;
	tDesc.InputSlot					= _iInputSlot;
	tDesc.InputSlotClass			= _eInputClass;
	tDesc.InstanceDataStepRate		= _iInstanceStepRate;

	// 다음 필드 정보를 위해 크기 연산
	// > "어디서 ~ 어디까지는 버텍스의 (위치 or 색상 ..등등) 정보야!
	m_iInputSize += _iSize;

	// 추가
	m_vecInputDesc.push_back(tDesc);
}

bool CShaderManager::CreateInputLayout(const string& _strName, const string& _strShaderKey)
{
	// 셰이더를 찾지 못한 경우
	CShader* pShader = FindShader(_strShaderKey);
	if (pShader == nullptr)
	{
		return false;
	}

	// 만들고자 하는 Input Layout 객체가 이미 존재하는 경우
	ID3D11InputLayout* pLayout = FindInputLayout(_strName);
	if (pLayout != nullptr)
	{
		return false;
	}

	// Input Layout 생성
	// : Blob(컴파일 된 셰이더) 정보가 필요하다.
	if (FAILED(DEVICE->CreateInputLayout(&m_vecInputDesc[0], m_vecInputDesc.size(), 
										 pShader->GetVSCode(), pShader->GetVSCodeSize(), &pLayout)))
	{
		return false;
	}

	SAFE_RELEASE(pShader);

	// Input Element 관련 변수 초기화
	m_iInputSize = 0;
	m_vecInputDesc.clear();

	// 추가
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
	// 만들고자 하는 상수 버퍼가 이미 존재하는 경우
	PCBuffer pBuffer = FindCBuffer(_strName);
	if (pBuffer != nullptr)
	{
		return false;
	}

	// 상수 버퍼 생성
	pBuffer = new CBuffer;
	pBuffer->pBuffer		= nullptr;
	pBuffer->iSize			= _iSize;
	pBuffer->iShaderType	= _iShaderType;
	pBuffer->iRegister		= _iRegister;
	D3D11_BUFFER_DESC tDesc = {};
	tDesc.ByteWidth			= _iSize;
	tDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER; // 상수 버퍼 용도
	tDesc.Usage				= D3D11_USAGE_DYNAMIC;		  // Map, UnMap 사용하여 리소스 업데이트 방식
	tDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	if (FAILED(DEVICE->CreateBuffer(&tDesc, nullptr, &pBuffer->pBuffer)))
	{
		return false;
	}
	
	// 추가
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
		// 상수 버퍼 데이터 쓰기
		// Map
		// > ID3D11Resourcee*		  : 읽기, 쓰기 접근을 원하는 리소스(현재는 버퍼)를 가리키는 포인터
		// > UINT					  : 부분자원의 색인 사용X (0)
		// > D3D11_MAP				  : 리소스 공통 플래그
		// > UINT					  : 추가 플래그 사용X (0)
		// > D3D11_MAPPED_SUBRESOURCE : D3D11_MAPPED_SUBRESOURCE 가르키는 포인터를 돌려준다. 
		//                              이를 통해서 버퍼에 자료를 기록하거나 읽음

		// > 매핑 : 매핑이란 해당 값이 다른 값을 가리키도록 하는 것이다.
		// > 서브 리소스인 D3D11_MAPPED_SUBRESOURCE는 pBuffer->pBuffer에 매핑되었다.
		//   D3D11_MAPPED_SUBRESOURCE 버퍼의 데이터를 변경해도 pBuffer->pBuffer에 매핑되었기 때문에
		//   셰이더에 데이터를 보낼 때 매번 최신의 버퍼 데이터를 보내게 된다.

		// 1. Map() 함수를 호출하여 포인터 접근 권한을 열고 pBuffer->pBuffer 객체에 
		//    D3D11_MAPPED_SUBRESOURCE 객체를 매핑시킨다.

		// 2. 상수 버퍼 데이터(_pData)를 D3D11_MAPPED_SUBRESOURCE 객체 멤버인 pData에 메모리 복사한다.
		//    tMap.pData 메모리에는 (void* _pData) 내용이 복사되어진다.

		// 3. Unmap() 함수 호출로 포인터 접근 권한을 막는다.

		D3D11_MAPPED_SUBRESOURCE tMap = {};
		CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);
		memcpy(tMap.pData, _pData, pBuffer->iSize);
		CONTEXT->Unmap(pBuffer->pBuffer, 0);
	}

	if (pCrt)
		LeaveCriticalSection(pCrt);

	// 버텍스 셰이더 상수 버퍼
	if (pBuffer->iShaderType & CST_VERTEX)
	{
		CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	}

	// 픽셀 셰이더 상수 버퍼
	if (pBuffer->iShaderType & CST_PIXEL)
	{
		CONTEXT->PSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	}
	
	return true;
}