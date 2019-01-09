#include "PCHEngine.h"
#include "Renderer.h"
#include "Device.h"
#include "Resources/Mesh.h"
#include "Resources/ResourcesManager.h"
#include "Component/Transform.h"
#include "Rendering/Shader.h"
#include "Rendering/ShaderManager.h"
#include "Component/Camera.h"
#include "Scene/Scene.h"
#include "Component/Material.h"
#include "Object/GameObject.h"
#include "Rendering/RenderState.h"
#include "Rendering/RenderManager.h"
#include "Component/Animation2D.h"

ENGINE_USING

CRenderer::CRenderer() :
	m_pMesh(nullptr),
	m_pShader(nullptr),
	m_pLayout(nullptr),
	m_pMaterial(nullptr)
{
	m_eComType = CT_RENDERER;

	// 'Render State(렌더링 상태)' 포인터 배열 초기화
	memset(m_pRenderState, 0, sizeof(CRenderState*) * RS_END);

	// 2D 게임모드에서는 깊이 버퍼를 사용하지 않는다.
	// > 2D : 레이어와 오브젝트의 출력 순서로 결정한다.
	// > 3D : 깊이 버퍼를 사용하여 픽셀 여부를 출력한다.
	if (GET_SINGLE(CRenderManager)->GetGameMode() == GM_2D)
	{
		SetRenderState(DEPTH_DISABLE);
	}

	memset(&m_tComponentCBuffer, 0, sizeof(m_tComponentCBuffer));
}

CRenderer::CRenderer(const CRenderer& _renderer) :
	CComponent(_renderer)
{
	*this = _renderer;

	// Mesh, AddRef()
	// > 원본이 삭제될 때 '댕글링 포인터'를 방지하기 위해 RefCount를 증가시킨다.
	if (m_pMesh != nullptr)	
	{ 
		m_pMesh->AddRef(); 
	}

	// Shader, AddRef()
	if (m_pShader != nullptr)
	{ 
		m_pShader->AddRef(); 
	}

	// Render State, AddRef()
	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i] != nullptr)
		{
			m_pRenderState[i]->AddRef();
		}
	}

	// Material
	// > 재질은 다를 수 있다.
	m_pMaterial = nullptr;

	// Renderer ConstantBuffer
	m_mapCBuffer.clear();
	unordered_map<string, PRendererCBuffer>::const_iterator	iter;
	unordered_map<string, PRendererCBuffer>::const_iterator	iterEnd = _renderer.m_mapCBuffer.end();
	for (iter = _renderer.m_mapCBuffer.begin(); iter != iterEnd; ++iter)
	{
		PRendererCBuffer pBuffer = new RendererCBuffer;
		pBuffer->strName		 = iter->second->strName;
		pBuffer->pBuffer		 = new char[iter->second->iSize];
		pBuffer->iSize			 = iter->second->iSize;

		// 추가
		m_mapCBuffer.insert(make_pair(iter->first, pBuffer));
	}
}

CRenderer::~CRenderer()
{
	// 메모리 해제
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMaterial);
	for (int i = 0; i < RS_END; ++i)
	{
		SAFE_RELEASE(m_pRenderState[i]);
	}

	unordered_map<string, PRendererCBuffer>::iterator iter;
	unordered_map<string, PRendererCBuffer>::iterator iterEnd = m_mapCBuffer.end();
	for (iter = m_mapCBuffer.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE_ARRAY(iter->second->pBuffer); // []
		SAFE_DELETE(iter->second);				  // new
	}
	m_mapCBuffer.clear();
}

void CRenderer::SetMesh(CMesh* _pMesh)
{
	SAFE_RELEASE(m_pMesh);
	if (_pMesh != nullptr)
	{
		m_pMesh = _pMesh;
		m_pMesh->AddRef();
		
		// Mesh 객체가 ShaderKey와 Input Layout Key를 가지고 있다.
		SetShader(m_pMesh->GetShaderKey());
		SetInputLayout(m_pMesh->GetInputLayoutKey());
	}
}

void CRenderer::SetMesh(const string& _strKey)
{
	SAFE_RELEASE(m_pMesh);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(_strKey);
	if (m_pMesh != nullptr)
	{
		SetShader(m_pMesh->GetShaderKey());
		SetInputLayout(m_pMesh->GetInputLayoutKey());
	}
}

void CRenderer::SetShader(CShader* _pshader)
{
	SAFE_RELEASE(m_pShader);
	if (_pshader != nullptr)
	{
		m_pShader = _pshader;
		m_pShader->AddRef();
	}
}

void CRenderer::SetShader(const string& _strKey)
{
	SAFE_RELEASE(m_pShader);
	m_pShader = GET_SINGLE(CShaderManager)->FindShader(_strKey);
}

void CRenderer::SetRenderState(const string& _strKey)
{
	CRenderState* pState = GET_SINGLE(CRenderManager)->FindRenderState(_strKey);
	if (pState == nullptr)
	{
		return;
	}
	m_pRenderState[pState->GetRenderState()] = pState;
}

bool CRenderer::CreateRendererCBuffer(const string& _strName, int _iSize)
{
	PRendererCBuffer pBuffer = FindRendererCBuffer(_strName);
	if (pBuffer != nullptr)
	{
		return false;
	}

	pBuffer			 = new RendererCBuffer;
	pBuffer->strName = _strName;
	pBuffer->pBuffer = new char[_iSize];
	pBuffer->iSize   = _iSize;

	// 추가
	m_mapCBuffer.insert(make_pair(_strName, pBuffer));

	return true;
}

void CRenderer::UpdateRendererCBuffer(const string& _strName, void* _pData, int _iSize)
{
	PRendererCBuffer pBuffer = FindRendererCBuffer(_strName);
	if (pBuffer == nullptr)
	{
		pBuffer			 = new RendererCBuffer;
		pBuffer->strName = _strName;
		pBuffer->pBuffer = new char[_iSize];
		pBuffer->iSize   = _iSize;

		// 추가
		m_mapCBuffer.insert(make_pair(_strName, pBuffer));
	}

	// 메모리 복사
	// > 렌더링에 필요한 상수 버퍼
	memcpy(pBuffer->pBuffer, _pData, _iSize);
}

PRendererCBuffer CRenderer::FindRendererCBuffer(const string& _strName)
{
	unordered_map<string, PRendererCBuffer>::iterator iter = m_mapCBuffer.find(_strName);
	if (iter == m_mapCBuffer.end())
	{
		return nullptr;
	}
	return iter->second;
}

void CRenderer::SetInputLayout(const string& _strKey)
{
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(_strKey);
}

void CRenderer::CheckComponent()
{
	const list<CComponent*>* pList = m_pObject->GetComponentList();
	list<CComponent*>::const_iterator iter;
	list<CComponent*>::const_iterator iterEnd = pList->end();
	for (iter = pList->begin(); iter != iterEnd; ++iter)
	{
		switch ((*iter)->GetComponentType())
		{
			case CT_ANIMATION2D:
			{
				m_tComponentCBuffer.iTextureAnimation2D = 1;
				break;
			}
		}
	}
}

void CRenderer::DeleteComponentCBuffer(CComponent * pComponent)
{
	switch (pComponent->GetComponentType())
	{
		case CT_ANIMATION2D:
		{
			m_tComponentCBuffer.iTextureAnimation2D = 0;
			break;
		}
	}
}

void CRenderer::UpdateAnimationType(int iType)
{
	m_tComponentCBuffer.iAnimationType = iType;
}

void CRenderer::UpdateTransform()
{
	CCamera* pCamera = nullptr;
	if (CheckComponentFromType(CT_UI) == true)
	{
		pCamera = m_pScene->GetUICamera();
	}
	else
	{
		pCamera = m_pScene->GetMainCamera();
	}

	// Transform 상수 버퍼 생성
	TransformCBuffer tCBuffer = {};
	tCBuffer.matWorld    = m_pTransform->GetWorldMatrix();
	tCBuffer.matView     = pCamera->GetViewMatrix();
	tCBuffer.matProj     = pCamera->GetProjMatrix();
	tCBuffer.matWV       = tCBuffer.matWorld * tCBuffer.matView;
	tCBuffer.matWVP      = tCBuffer.matWV * tCBuffer.matProj;
	tCBuffer.vPivot      = m_pTransform->GetPivot();
	tCBuffer.vLength     = m_pMesh->GetLength();

	// 셰이더에서의 행렬 연산을 위해 전치 행렬
	tCBuffer.matWorld.Transpose();
	tCBuffer.matView.Transpose();
	tCBuffer.matProj.Transpose();
	tCBuffer.matWV.Transpose();
	tCBuffer.matWVP.Transpose();

	// "Transform" 상수 버퍼 갱신
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Transform", &tCBuffer);
	SAFE_RELEASE(pCamera);
}

bool CRenderer::Init()
{
	// Renderer와 Material 정보는 연관성이 깊다.
	// > Renderer 컴포넌트 생성 초기화 단계에 Material 컴포넌트도 생성한다.
	m_pMaterial = AddComponent<CMaterial>("Material");

	CheckComponent();

	return true;
}

int CRenderer::Update(float _fTime)
{
	return 0;
}

int CRenderer::LateUpdate(float _fTime)
{
	return 0;
}

void CRenderer::Collision(float _fTime)
{
}

void CRenderer::Render(float _fTime)
{
	if (m_pMaterial == nullptr)
	{
		m_pMaterial = FindComponentFromType<CMaterial>(CT_MATERIAL);
	}

	// 공간 변환
	// > 로컬 -> 월드 -> 뷰 -> 투영
	UpdateTransform();

	// 렌더링 상태 설정
	// > Alpha Blend, DepthStencil, Rasterizer, Sampler State
	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
		{
			m_pRenderState[i]->SetState();
		}
	}

	// 렌더링 상수 버퍼 갱신
	// > 셰이더에서 렌더링하기 위해 필요한 렌더링 데이터들이 있을 수 있다.
	unordered_map<string, PRendererCBuffer>::iterator iter;
	unordered_map<string, PRendererCBuffer>::iterator iterEnd = m_mapCBuffer.end();
	for (iter = m_mapCBuffer.begin(); iter != iterEnd; ++iter)
	{
		GET_SINGLE(CShaderManager)->UpdateCBuffer(iter->first, iter->second->pBuffer);
	}

	// 컴포넌트 상수 버퍼
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Component", &m_tComponentCBuffer);

	// 입력 레이아웃
	// > 셰이더에서 정점 정보를 파악하기 위해 사용된다.
	CONTEXT->IASetInputLayout(m_pLayout);

	// 셰이더 결합
	// > 로드된 셰이더를 렌더링 파이프라인 각 셰이더 단계에 결합
	// > 현재 : Vertex, Pixel Shader 사용
	m_pShader->SetShader();

	// 재질 설정, 메시 렌더링 시작
	// > 재질(Material) : Texture*, Sampler* 자원을 셰이더에 연결한다.
	// > 메시(Mesh) : 실제 렌더링 파이프라인의 시작단계
	for (size_t i = 0; i < (int)m_pMesh->GetContainerCount(); ++i)
	{
		for (size_t j = 0; j < (int)m_pMesh->GetSubsetCount(i); ++j)
		{
			m_pMaterial->SetShader(i, j);
			m_pMesh->Render(i, j);
		}
	}

	// 렌더링 상태 리셋
	// > 렌더링 상태 연산은 느리기때문에 필요없는 경우는 연산하지 않도록 한다.
	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
		{
			m_pRenderState[i]->ResetState();
		}
	}
}

void CRenderer::AfterClone()
{
	m_pMaterial = FindComponentFromType<CMaterial>(CT_MATERIAL);
}

CRenderer* CRenderer::Clone()
{
	return new CRenderer(*this);
}