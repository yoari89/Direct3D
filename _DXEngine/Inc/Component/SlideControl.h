#pragma once
#include "UI.h"
#include "Scene/Layer.h"

ENGINE_BEGIN

enum eControllerState
{
	CONTROLLER_NORMAL,
	CONTROLLER_OVER,
	CONTROLLER_CLICK,
	CONTROLLER_DRAG,
	CONTROLLER_MAX,
};

class CTexture;
class CCollider;
class CTransform;
class ENGINE_DLL CSlideControl : public CUI
{
private:
	friend class CGameObject;

public:
	CSlideControl();
	CSlideControl(const CSlideControl& _sildeControl);
	virtual ~CSlideControl();

/* 컴포넌트 클래스 가상함수 재정의 */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CSlideControl* Clone()			override;

private:
	bool  m_isPush;
	float m_startMosePosX;
	float m_beforeDistance;
	float m_ratioEndPosX;
	CTransform*		 m_ratioTr;
	CTransform*		 m_controllerTr;
	eControllerState m_controllerState;

public:
	void  CreateSlideContol(const string _pTexRatioName, Vector3 _vRatioPos, Vector3 _vRatioSize,
						   const string _pTexControlName, Vector3 _vControlPos, Vector3 _vControlSize,
						   const string _pTexBorderName = "", Vector3 _vBorderPos = Vector3::Zero, 
						   Vector3 _vBorderSize = Vector3::Zero);
	void  CreateBorder(const string _pTexBorderName = "", 
		 			  Vector3 _vBorderPos = Vector3::Zero, Vector3 _vBorderSize = Vector3::Zero);
	void  CreateRatio(const string _pTexRatioName, Vector3 _vRatioPos, Vector3 _vRatioSize);
	void  CreateController(const string _pTexControlName, Vector3 _vControlPos, Vector3 _vControlSize);
	float FromZeroToOnePercent();
	
public:
	void ControllerOn(CCollider* _pSrc, CCollider* _pDest, float _fTime);
	void ControllerLeave(CCollider* _pSrc, CCollider* _pDest, float _fTime);
};
ENGINE_END