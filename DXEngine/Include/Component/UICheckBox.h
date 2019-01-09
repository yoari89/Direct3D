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

/* 컴포넌트 클래스 가상함수 재정의 */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CUICheckBox* Clone()			override;

/* 버튼 상태 */
private:
	bool m_isActive;

public:
	void SetActive(float _fTime);

public:
	void MouseOn(CCollider* _pSrc, CCollider* _pDest, float _fTime);
	void MouseLeave(CCollider* _pSrc, CCollider* _pDest, float _fTime);
};
ENGINE_END