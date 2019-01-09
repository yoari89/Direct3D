#include "PCHEngine.h"
#include "Utility/Types.h"
#include "RenderState.h"

ENGINE_USING

CRenderState::CRenderState() :
	m_pState(nullptr),
	m_pOldState(nullptr)
{
}

CRenderState::~CRenderState()
{ 
	SAFE_RELEASE(m_pOldState);
	SAFE_RELEASE(m_pState);
}

RENDER_STATE CRenderState::GetRenderState() const
{
	return m_eState;
}