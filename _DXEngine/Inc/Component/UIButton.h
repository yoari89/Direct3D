#pragma once
#include "UI.h"

ENGINE_BEGIN
class CCollider;
class ENGINE_DLL CUIButton : public CUI
{
private:
	friend class CGameObject;

protected:
	CUIButton();
	CUIButton(const CUIButton& _button);
	virtual ~CUIButton();

/* 컴포넌트 클래스 가상함수 재정의 */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CUIButton* Clone()				override;

/* 버튼 상수 버퍼 */
private:
	ButtonCBuffer m_tCBuffer;

/* 버튼 상태 */
protected:
	BUTTON_STATE  m_eState;
	Vector4		  m_vBSColor[BS_END];

public:
	void Enable();
	void Disable();
	void CreateDefaultButton();
	void SetStateColor(BUTTON_STATE _eState, const Vector4& _vColor);
	void SetStateColor(BUTTON_STATE _eState, unsigned char _r, unsigned char _g,
					   unsigned char _b, unsigned char _a);
	BUTTON_STATE GetState();

/* Collision */
public:
	void MouseOn(CCollider* _pSrc, CCollider* _pDest, float _fTime);
	void MouseLeave(CCollider* _pSrc, CCollider* _pDest, float _fTime);

/* CallBack */
protected:
	function<void(float)> m_clickCallBack;

public:
	void SetCallBack(void(*_pFunc)(float));
	template<typename T>
	void SetCallBack(void(T::*_pFunc)(float), T* _pObj)
	{
		m_clickCallBack = bind(_pFunc, _pObj, placeholders::_1);
	}
};
ENGINE_END