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

	// 'Render State(������ ����)' ������ �迭 �ʱ�ȭ
	memset(m_pRenderState, 0, sizeof(CRenderState*) * RS_END);

	// 2D ���Ӹ�忡���� ���� ���۸� ������� �ʴ´�.
	// > 2D : ���̾�� ������Ʈ�� ��� ������ �����Ѵ�.
	// > 3D : ���� ���۸� ����Ͽ� �ȼ� ���θ� ����Ѵ�.
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
	// > ������ ������ �� '��۸� ������'�� �����ϱ� ���� RefCount�� ������Ų��.
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
	// > ������ �ٸ� �� �ִ�.
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

		// �߰�
		m_mapCBuffer.insert(make_pair(iter->first, pBuffer));
	}
}

CRenderer::~CRenderer()
{
	// �޸� ����
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
		
		// Mesh ��ü�� ShaderKey�� Input Layout Key�� ������ �ִ�.
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

	// �߰�
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

		// �߰�
		m_mapCBuffer.insert(make_pair(_strName, pBuffer));
	}

	// �޸� ����
	// > �������� �ʿ��� ��� ����
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

	// Transform ��� ���� ����
	TransformCBuffer tCBuffer = {};
	tCBuffer.matWorld    = m_pTransform->GetWorldMatrix();
	tCBuffer.matView     = pCamera->GetViewMatrix();
	tCBuffer.matProj     = pCamera->GetProjMatrix();
	tCBuffer.matWV       = tCBuffer.matWorld * tCBuffer.matView;
	tCBuffer.matWVP      = tCBuffer.matWV * tCBuffer.matProj;
	tCBuffer.vPivot      = m_pTransform->GetPivot();
	tCBuffer.vLength     = m_pMesh->GetLength();

	// ���̴������� ��� ������ ���� ��ġ ���
	tCBuffer.matWorld.Transpose();
	tCBuffer.matView.Transpose();
	tCBuffer.matProj.Transpose();
	tCBuffer.matWV.Transpose();
	tCBuffer.matWVP.Transpose();

	// "Transform" ��� ���� ����
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Transform", &tCBuffer);
	SAFE_RELEASE(pCamera);
}

bool CRenderer::Init()
{
	// Renderer�� Material ������ �������� ���.
	// > Renderer ������Ʈ ���� �ʱ�ȭ �ܰ迡 Material ������Ʈ�� �����Ѵ�.
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

	// ���� ��ȯ
	// > ���� -> ���� -> �� -> ����
	UpdateTransform();

	// ������ ���� ����
	// > Alpha Blend, DepthStencil, Rasterizer, Sampler State
	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
		{
			m_pRenderState[i]->SetState();
		}
	}

	// ������ ��� ���� ����
	// > ���̴����� �������ϱ� ���� �ʿ��� ������ �����͵��� ���� �� �ִ�.
	unordered_map<string, PRendererCBuffer>::iterator iter;
	unordered_map<string, PRendererCBuffer>::iterator iterEnd = m_mapCBuffer.end();
	for (iter = m_mapCBuffer.begin(); iter != iterEnd; ++iter)
	{
		GET_SINGLE(CShaderManager)->UpdateCBuffer(iter->first, iter->second->pBuffer);
	}

	// ������Ʈ ��� ����
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Component", &m_tComponentCBuffer);

	// �Է� ���̾ƿ�
	// > ���̴����� ���� ������ �ľ��ϱ� ���� ���ȴ�.
	CONTEXT->IASetInputLayout(m_pLayout);

	// ���̴� ����
	// > �ε�� ���̴��� ������ ���������� �� ���̴� �ܰ迡 ����
	// > ���� : Vertex, Pixel Shader ���
	m_pShader->SetShader();

	// ���� ����, �޽� ������ ����
	// > ����(Material) : Texture*, Sampler* �ڿ��� ���̴��� �����Ѵ�.
	// > �޽�(Mesh) : ���� ������ ������������ ���۴ܰ�
	for (size_t i = 0; i < (int)m_pMesh->GetContainerCount(); ++i)
	{
		for (size_t j = 0; j < (int)m_pMesh->GetSubsetCount(i); ++j)
		{
			m_pMaterial->SetShader(i, j);
			m_pMesh->Render(i, j);
		}
	}

	// ������ ���� ����
	// > ������ ���� ������ �����⶧���� �ʿ���� ���� �������� �ʵ��� �Ѵ�.
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