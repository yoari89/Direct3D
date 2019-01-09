#pragma once
#include "Component/Component.h"

ENGINE_BEGIN
class ENGINE_DLL CGravity : public CComponent
{
private:
	friend class CGameObject;

public:
	CGravity();
	CGravity(const CGravity& _gravity);
	~CGravity();

// Component 재정의 함수
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	CGravity* Clone()				override;

private:
	float m_jumpVelocity;
	float m_gravityLimit;
	float m_gravityVelocity;
	float m_doubleJumpVelocity;

public:
	void SetJumpVelocityZero();
	void SetJumpVelocity(float _jump);
	void SetGravityLimit(float _limit);
	void SetGravityVelocity(float _gravity);
	void SetDoubleJumpVelocity(bool _isLadder, float _doubleJump = 0.f);

	/*
	// 적분을 이용한 중력 계산
	bool  m_isAddForceY;
	float m_gravity;		// 중력
	float m_accelTime;		// 가속도 시간
	float m_accelPower;		// 가속력
	float m_playerInitPosY;	// 플레이어 초기 위치(Y) 값 
	float m_addForeceY;
	*/
};
ENGINE_END