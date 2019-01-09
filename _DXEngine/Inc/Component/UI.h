#pragma once
#include "Component/Component.h"

ENGINE_BEGIN
class ENGINE_DLL CUI : public CComponent
{
private:
	friend class CGameObject;

protected:
	CUI();
	CUI(const CUI& _ui);
	virtual ~CUI() = 0;

/* ������Ʈ Ŭ���� �����Լ� ������ */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	virtual CUI* Clone() = 0;

protected:
	UI_TYPE m_eUIType;

public:
	UI_TYPE GetUIType() const;
};
ENGINE_END