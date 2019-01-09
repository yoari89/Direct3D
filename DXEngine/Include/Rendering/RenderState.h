#pragma once
#include "../ReferenceCnt.h"

ENGINE_BEGIN
class ENGINE_DLL CRenderState : public CReferenceCnt
{
private:
	friend class CRenderManager;

protected:
	CRenderState();
	virtual ~CRenderState() = 0;

protected:
	ID3D11DeviceChild* m_pState;
	ID3D11DeviceChild* m_pOldState;
	RENDER_STATE	   m_eState;

/* Render State */
public:
	RENDER_STATE GetRenderState() const;

public:
	virtual void SetState()	  = 0;
	virtual void ResetState() = 0;
};
ENGINE_END