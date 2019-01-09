#pragma once
#include "UI.h"

ENGINE_BEGIN
class ENGINE_DLL CFadeScreen : public CUI
{
private:
	friend class CGameObject;

public:
	CFadeScreen();
	CFadeScreen(const CFadeScreen& _screenFade);
	~CFadeScreen();

/* 컴포넌트 클래스 가상함수 재정의 */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CFadeScreen* Clone()			override;

private:
	eFadeInState	m_eFadeInState;
	eFadeOutState	m_eFadeOutState;
	float			m_colorState;
	FadeCBuffer		m_fadeCBuffer;
	void (CFadeScreen::*m_fp)(float);

public:
	void VariableClear();
	void SetFadeType(eFadeType _eType);
	eFadeInState  GetFadeInState()	const;
	eFadeOutState GetFadeOutState() const;

	// > Function Pointer
	void SetFuncPointer(void(CFadeScreen::*_fp)(float));

public:
	void FadeIn(float _fTime);
	void FadeOut(float _fTime);
};
ENGINE_END