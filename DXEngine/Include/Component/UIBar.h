#pragma once
#include "UI.h"

ENGINE_BEGIN
class ENGINE_DLL CUIBar : public CUI
{
private:
	friend class CGameObject;

public:
	CUIBar();
	CUIBar(const CUIBar& _uiBar);
	virtual ~CUIBar();

private:
	float	m_fMinValue;
	float	m_fMaxValue;
	float	m_fValue;
	BAR_DIR	m_eDir;
	BarCBuffer	m_tCBuffer;

	float	m_fValueLength;
	Vector3	m_vScale;
	class CColliderRect*	m_pCollider;

public:
	void SetBarDir(BAR_DIR _eDir);
	void SetValue(float _fValue);
	void AddValue(float _fValue);
	void SetMinMaxValue(float _fMin, float _fMax);
	void SetScale(const Vector3& vScale);
	void SetScale(float x, float y, float z);
	void LightOn();
	void LightOff();

public:
	void Hit(class CCollider* pSrc, class CCollider* pDest, float fTime);
	void MouseOut(class CCollider* pSrc, class CCollider* pDest, float fTime);

public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CUIBar* Clone()					override;
};
ENGINE_END