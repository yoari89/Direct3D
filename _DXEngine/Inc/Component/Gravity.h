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

// Component ������ �Լ�
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
	// ������ �̿��� �߷� ���
	bool  m_isAddForceY;
	float m_gravity;		// �߷�
	float m_accelTime;		// ���ӵ� �ð�
	float m_accelPower;		// ���ӷ�
	float m_playerInitPosY;	// �÷��̾� �ʱ� ��ġ(Y) �� 
	float m_addForeceY;
	*/
};
ENGINE_END