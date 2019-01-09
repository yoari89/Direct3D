#include "PCHEngine.h"
#include "Gravity.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"

ENGINE_USING

CGravity::CGravity() :
	m_gravityLimit(0.f),
	m_jumpVelocity(0.f),
	m_gravityVelocity(0.f),
	m_doubleJumpVelocity(0.f)
{
}

CGravity::CGravity(const CGravity& _gravity) : 
	CComponent(_gravity)
{
}

CGravity::~CGravity()
{
}

bool CGravity::Init()
{
	return true;
}

int CGravity::Input(float _fTime)
{
	return 0;
}

int CGravity::Update(float _fTime)
{
	m_jumpVelocity -= m_gravityVelocity * _fTime;
	if (m_jumpVelocity < m_gravityLimit)
	{
		m_jumpVelocity = m_gravityLimit;
	}

	m_pTransform->Move(AXIS_Y, m_jumpVelocity, _fTime);

	/*
	Vector3 vPosition = m_pTransform->GetWorldPos();
	// ���� ��ġ�� �ٲ��ָ� �Ǵ°ſ��µ�.........��_��...�� ����.......
	if (m_isAddForceY == true)
	{
		m_playerInitPosY += 1.f * m_addForeceY;
	}
	
	// y = -a * x + b (a: �߷°��ӵ�, b: �ʱ� �����ӵ�)
	// �����Ͽ� y = (-a / 2) * x * x  + (b * x) ������ ��´�.(x: �����ð�, y: ������Ʈ�� ����)
	float height = (m_accelTime * m_accelTime * (-m_gravity) / 2.f) + (m_accelTime * m_accelPower);
	vPosition.y  = m_playerInitPosY + height;
	m_accelTime += _fTime;
	m_pTransform->SetWorldPos(vPosition);
	*/

	return 0;
}

int CGravity::LateUpdate(float _fTime)
{
	return 0;
}

void CGravity::Collision(float _fTime)
{
}

void CGravity::Render(float _fTime)
{
}

CGravity* CGravity::Clone()
{
	return new CGravity(*this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGravity::SetJumpVelocityZero()
{
	m_jumpVelocity = 0.f;
}

void CGravity::SetJumpVelocity(float _jump)
{
	m_jumpVelocity = _jump;
}

void CGravity::SetGravityLimit(float _limit)
{
	m_gravityLimit = _limit;
}

void CGravity::SetGravityVelocity(float _gravity)
{
	m_gravityVelocity = _gravity;
}

void CGravity::SetDoubleJumpVelocity(bool _isLadder, float _doubleJump)
{
	if (_isLadder == false)
	{
		if (m_jumpVelocity < -100.f)
		{
			return;
		}
	}

	// ��ٸ� �Ǵ� ���ٿ� �Ŵ޷� �ִ� ���
	m_jumpVelocity += _doubleJump;
}