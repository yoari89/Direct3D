#pragma once
#include "UIButton.h"

ENGINE_BEGIN
class ENGINE_DLL CUICheckBox : public CUIButton
{
private:
	friend class CGameObject;

public:
	CUICheckBox();
	CUICheckBox(const CUICheckBox& _checkBox);
	~CUICheckBox();

/* ������Ʈ Ŭ���� �����Լ� ������ */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CUICheckBox* Clone()			override;

/* ��ư ���� */
private:
	bool m_isActive;

public:
	void SetActive(float _fTime);

public:
	void MouseOn(CCollider* _pSrc, CCollider* _pDest, float _fTime);
	void MouseLeave(CCollider* _pSrc, CCollider* _pDest, float _fTime);
};
ENGINE_END