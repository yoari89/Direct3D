#pragma once

ENGINE_BEGIN
class CRenderState;
class CBlendState;
class ENGINE_DLL CRenderManager
{
DECLARE_SINGLE(CRenderManager)

/* Render Mode*/
private:
	GAME_MODE m_eGameMode;

public:
	GAME_MODE GetGameMode() const;
	void SetGameMode(GAME_MODE _eGameMode);

private:
	CBlendState* m_pCreateState;
	unordered_map<string, CRenderState*> m_mapRenderState;

/* Blend State */
public:
	bool Init();
	void AddBlendTargetDesc(BOOL _bEnable,
							D3D11_BLEND _srcBlend		 = D3D11_BLEND_SRC_ALPHA,
							D3D11_BLEND _destBlend		 = D3D11_BLEND_INV_SRC_ALPHA,
							D3D11_BLEND_OP _blendOp		 = D3D11_BLEND_OP_ADD,
							D3D11_BLEND _srcAlphaBlend	 = D3D11_BLEND_ONE,
							D3D11_BLEND _destAlphaBlend	 = D3D11_BLEND_ZERO,
							D3D11_BLEND_OP _blendAlphaOp = D3D11_BLEND_OP_ADD,
							UINT8 _iWriteMask			 = D3D11_COLOR_WRITE_ENABLE_ALL);
	bool CreateBlendState(const string& _strName, BOOL _bAlphaCoverage = FALSE, BOOL _bIndependent = FALSE);

/* DepthStencil State */
public:
	bool CreateDepthStencilState(const string& _strKey, 
								 BOOL _bDepthEnable,
								 D3D11_DEPTH_WRITE_MASK _eMask			= D3D11_DEPTH_WRITE_MASK_ALL,
								 D3D11_COMPARISON_FUNC _eDepthFunc		= D3D11_COMPARISON_LESS,
								 BOOL _bStencilEnable					= FALSE,
								 UINT8 _iStencilReadMask				= 0, 
								 UINT8 _iStencilWriteMask				= 0,
								 D3D11_DEPTH_STENCILOP_DESC _tFrontFace = {},
								 D3D11_DEPTH_STENCILOP_DESC _tBackFace	= {});

/* Render State Find */
public:
	CRenderState* FindRenderState(const string& strName);

};
ENGINE_END