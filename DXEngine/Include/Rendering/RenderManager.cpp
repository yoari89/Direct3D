#include "PCHEngine.h"
#include "RenderManager.h"
#include "ShaderManager.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "Utility/EnumFlag.h"

ENGINE_USING
DEFINITION_SINGLE(CRenderManager)

CRenderManager::CRenderManager() :
	m_pCreateState(nullptr)
{
	m_eGameMode = GM_3D;
}

CRenderManager::~CRenderManager()
{
	Safe_Release_Map<unordered_map<string, CRenderState*>>(m_mapRenderState);
}

GAME_MODE CRenderManager::GetGameMode() const
{
	return m_eGameMode;
}

void CRenderManager::SetGameMode(GAME_MODE _eGameMode)
{
	m_eGameMode = _eGameMode;
}

bool CRenderManager::Init()
{
	// ShaderManager 초기화
	if (GET_SINGLE(CShaderManager)->Init() == false)
	{
		return false;
	}

	// 블렌딩 생성
	// > Blend Key : "AlphaBlend"
	AddBlendTargetDesc(TRUE);
	CreateBlendState(ALPHA_BLEND);

	// > DepthStencil Key : "DepthDisable"
	CreateDepthStencilState(DEPTH_DISABLE, FALSE);

	return true;
}

void CRenderManager::AddBlendTargetDesc(BOOL _bEnable, D3D11_BLEND _srcBlend, D3D11_BLEND _destBlend, D3D11_BLEND_OP _blendOp,
										D3D11_BLEND _srcAlphaBlend, D3D11_BLEND _destAlphaBlend, D3D11_BLEND_OP _blendAlphaOp, UINT8 _iWriteMask)
{
	// 블렌딩 
	// > D3D11_RENDER_TARGET_BLEND_DESC 값(기본 값) 정의
	if (m_pCreateState == nullptr)
	{
		m_pCreateState = new CBlendState();
	}
	m_pCreateState->AddTargetDesc(_bEnable, _srcBlend, _destBlend, _blendOp, 
								  _srcAlphaBlend, _destAlphaBlend, _blendAlphaOp, _iWriteMask);
}

bool CRenderManager::CreateBlendState(const string& _strName, BOOL _bAlphaCoverage, BOOL _bIndependent)
{
	if (m_pCreateState == nullptr)
	{
		return false;
	}

	// 생성
	if (m_pCreateState->CreateState(_bAlphaCoverage, _bIndependent) == false)
	{
		SAFE_RELEASE(m_pCreateState);
		return false;
	}

	// 추가
	m_mapRenderState.insert(make_pair(_strName, m_pCreateState));
	m_pCreateState = nullptr;

	return true;
}

bool CRenderManager::CreateDepthStencilState(const string& _strKey, 
											 BOOL _bDepthEnable, 
											 D3D11_DEPTH_WRITE_MASK _eMask, 
											 D3D11_COMPARISON_FUNC _eDepthFunc, 
											 BOOL _bStencilEnable, 
											 UINT8 _iStencilReadMask, 
											 UINT8 _iStencilWriteMask, 
											 D3D11_DEPTH_STENCILOP_DESC _tFrontFace, 
											 D3D11_DEPTH_STENCILOP_DESC _tBackFace)
{
	CDepthStencilState* pDepth = (CDepthStencilState*)FindRenderState(_strKey);

	if (pDepth != nullptr)
	{
		return false;
	}

	// 생성
	pDepth = new CDepthStencilState;
	if (pDepth->CreateState(_bDepthEnable, _eMask,
							_eDepthFunc, _bStencilEnable, _iStencilReadMask, _iStencilWriteMask,
							_tFrontFace, _tBackFace) == false)
	{
		SAFE_RELEASE(pDepth);
		return false;
	}

	// 추가
	m_mapRenderState.insert(make_pair(_strKey, pDepth));

	return true;
}

CRenderState* CRenderManager::FindRenderState(const string& strName)
{
	unordered_map<string, CRenderState*>::iterator iter = m_mapRenderState.find(strName);
	if (iter == m_mapRenderState.end())
	{
		return nullptr;
	}
	iter->second->AddRef();
	return iter->second;
}