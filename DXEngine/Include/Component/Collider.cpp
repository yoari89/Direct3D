#include "PCHEngine.h"
#include "Collider.h"
#include "Device.h"
#include "Resources/Mesh.h"
#include "Rendering/Shader.h"
#include "Rendering/RenderState.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/RenderManager.h"

ENGINE_USING

CCollider::CCollider() :
	m_userColType(UCT_NONE)
{
	m_eComType			= CT_COLLIDER;
	m_strCollisionGroup = "Default";
	m_bUpdateCollision	= true;

#ifdef _DEBUG
	m_vColor		= Vector4::Green;
	m_pMesh			= nullptr;
	m_pShader		= GET_SINGLE(CShaderManager)->FindShader(COLLIDER_SHADER);
	m_pLayout		= GET_SINGLE(CShaderManager)->FindInputLayout(POS_LAYOUT);
	m_pDepthDisable = nullptr;
#endif // _DEBUG
}

CCollider::CCollider(const CCollider& _com) : CComponent(_com)
{
	*this = _com;

#ifdef _DEBUG
	m_vColor		= _com.m_vColor;
	m_pMesh			= _com.m_pMesh;
	m_pShader		= _com.m_pShader;
	m_pLayout		= _com.m_pLayout;
	m_pDepthDisable = _com.m_pDepthDisable;

	if (m_pMesh != nullptr)			{ m_pMesh->AddRef();		 } // Mesh
	if (m_pShader !=  nullptr)		{ m_pShader->AddRef();		 } // Shader
	if (m_pDepthDisable != nullptr) { m_pDepthDisable->AddRef(); } // Depth
#endif // _DEBUG
}

CCollider::~CCollider()
{
#ifdef _DEBUG
	// �޸� ����
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pDepthDisable);
#endif // _DEBUG

	// �浹ü �Ҹ�� �ڽŰ� �浹���� ������Ʈ(m_prevCollider)�� ����Ʈ���� ����
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_prevCollider.end();
	for (iter = m_prevCollider.begin(); iter != iterEnd; ++iter)
	{
		// �Ҹ�� �浹ü, ���� �浹ü�� ���Ͽ� OnCollisionLeave ȣ��
		OnCollisionLeave((*iter), 0.f);
		(*iter)->OnCollisionLeave(this, 0.f);

		// ���� �浹ü���� �Ҹ�� �浹ü�� ������Ų��.
		(*iter)->ErasePrevCollider(this);
	}
}

bool CCollider::Init()
{
	return true;
}

int CCollider::Input(float fTime)
{
	return 0;
}

int CCollider::Update(float fTime)
{
	return 0;
}

int CCollider::LateUpdate(float fTime)
{
	return 0;
}

void CCollider::Collision(float fTime)
{
}

void CCollider::Render(float fTime)
{
#ifdef _DEBUG
	// �� �浹ü(Point, Rect, Circle, OBB, Pixel ...)�� �ϳ��� ������Ʈ
	// ��, ���� ���� ������ ���ؼ� �Լ��� ȣ��Ǹ�, ȣ���ڴ� ���� ����� �ڽĵ��� ȣ���ڰ� �ȴ�.
	// �� ȣ���ڴ� �ڽŸ��� �޽�, ���̴�, ������ ����, �Է� ���̾ƿ��� ������ �� �ִ�.
	// �̸� ���Ͽ� ���������� ������ �غ� ���¸� ��ġ��, �޽ø� ���Ͽ� �������� �����Ѵ�.

	if (m_prevCollider.empty() == true) { m_vColor = Vector4::Green; }
	else								{ m_vColor = Vector4::Red; }

	// �浹ü ���� ���̴� (Collider.fx)
	m_pShader->SetShader();

	// �Է� ���̾ƿ� 
	// > POS_LAYOUT : Vector3 ������ ������ ���ؽ� ���� ����
	CONTEXT->IASetInputLayout(m_pLayout);

	// ��� ���� ������Ʈ
	// > float4 Ÿ�Ը� ���� ��� ����, ���� ������ ��� ���� ��� ����
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Collider", &m_vColor);

	// ������ ���� ����
	// > AlphaBlend   = ��� ���� ����
	// > DepthStencil = 2D���ӿ����� ������� �ʴ´�.
	if (m_pDepthDisable != nullptr)
	{
		m_pDepthDisable->SetState();
	}

	// ������ ����
	m_pMesh->Render();

	// ������ ���� �⺻ ������ ����
	if (m_pDepthDisable != nullptr)
	{
		m_pDepthDisable->ResetState();
	}
#endif
}

void CCollider::SetUserColType(eUserColType _type)
{
	m_userColType = _type;
}

eUserColType CCollider::GetUserColType()
{
	return m_userColType;
}

// ========== �浹ü ���� �������� ==========

bool CCollider::GetUpdateCollision() const
{
	return m_bUpdateCollision;
}

string CCollider::GetCollisionGroup() const
{
	return m_strCollisionGroup;
}

COLLIDER_TYPE CCollider::GetColliderType() const
{
	return m_eCollType;
}

Vector3 CCollider::GetColliderSizeMin() const
{
	return m_vColliderSizeMin;
}

Vector3 CCollider::GetColliderSizeMax() const
{
	return m_vColliderSizeMax;
}

const list<int>* CCollider::GetColisionSection() const
{
	return &m_collisionSectionList;
}

// ========== �浹 �׷�, �浹ü ���� ==========

void CCollider::ClearCollisionSection()
{
	// ���� ����Ʈ �ʱ�ȭ
	m_collisionSectionList.clear();
}

void CCollider::AddCollisionSection(int _iSection)
{
	// ���ǿ� �ε��� �߰�
	m_collisionSectionList.push_back(_iSection);
}

void CCollider::SetCollisionGroup(const string& _strCollisionGroup)
{
	// �浹 �׷� ����
	m_strCollisionGroup = _strCollisionGroup;
}

void CCollider::SetPivot(const Vector3& _vPivot)
{
	// �浹ü �߽��� ����(Vector3 Type)
	m_vPivot = _vPivot;
}

void CCollider::SetPivot(float _x, float _y, float _z)
{
	// �浹ü �߽��� ����(float Type)
	m_vPivot = Vector3(_x, _y, _z);
}

void CCollider::AddPrevCollider(CCollider* _pDest)
{
	// ȣ���� �浹ü�� �浹�� �浹ü(_pDest)�� �߰���Ų��.
	m_prevCollider.push_back(_pDest);
}

void CCollider::ErasePrevCollider(CCollider* _pDest)
{
	// ȣ���� �浹ü�� ���� ������ �浹ü(_pDest)�� ������Ų��.
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_prevCollider.end();
	for (iter = m_prevCollider.begin(); iter != iterEnd; ++iter)
	{
		if (*iter == _pDest)
		{
			m_prevCollider.erase(iter);
			return;
		}
	}
}

bool CCollider::CheckPrevCollider(CCollider* _pDest)
{
	// ȣ���� �浹ü�� �Ű����� �浹ü(_pDest)�� �浹 ���θ� �Ǵ��Ѵ�.
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_prevCollider.end();
	for (iter = m_prevCollider.begin(); iter != iterEnd; ++iter)
	{
		if (*iter == _pDest)
		{
			return true;
		}
	}
	return false;
}

void CCollider::CheckPrevColliderInSection(float _fTime)
{
	// ȣ���� �浹ü (���Ǿȿ� �����ϴ� �浹ü ���)
	// ���� �浹 �׷��� �ƴϰų�, ���� ������ �ƴϸ� ��Ͽ��� �����.  
	// Object To Object �浹�� �˻��� �� Enter, Stay, Leave ���¸� �ٽ� �˻��Ѵ�.
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_prevCollider.end();
	for (iter = m_prevCollider.begin(); iter != iterEnd;)
	{
		// �浹 �׷� �˻�
		if (m_strCollisionGroup != (*iter)->m_strCollisionGroup)
		{
			// �浹 ��
			OnCollisionLeave((*iter), _fTime);
			(*iter)->OnCollisionLeave(this, _fTime);

			// ���� �浹 ��ü���� ����
			(*iter)->ErasePrevCollider(this);
			iter	= m_prevCollider.erase(iter);
			iterEnd = m_prevCollider.end();
			continue;
		}

		// �ڽ��� �浹 ���� ����Ʈ
		list<int>::iterator	Iter;
		list<int>::iterator	IterEnd = m_collisionSectionList.end();

		// �ڽ��� �浹ü�� �浹�� �浹ü�� ���� ����Ʈ
		const list<int>* pSectionList = (*iter)->GetColisionSection();
		list<int>::const_iterator destIter;
		list<int>::const_iterator destIterEnd = pSectionList->end();

		// ���� �˻�
		bool isPair = false;
		for (destIter = pSectionList->begin(); destIter != destIterEnd; ++destIter)
		{
			for (Iter = m_collisionSectionList.begin(); Iter != IterEnd; ++Iter)
			{
				// ���� ��ġ
				if (*destIter == *Iter)
				{
					isPair = true;
					break;
				}
			}
			if (isPair == true)
			{
				break;
			}
		}
		// ���� ����ġ
		if (isPair == false)
		{
			// �浹 ��
			OnCollisionLeave((*iter), _fTime);
			(*iter)->OnCollisionLeave(this, _fTime);

			// ���� �浹 ��ü���� ����
			(*iter)->ErasePrevCollider(this);
			iter	= m_prevCollider.erase(iter);
			iterEnd = m_prevCollider.end();
			continue;
		}
		++iter;
	}
}

// ========== �浹 Ÿ�� ���� ==========

bool CCollider::CollisionRectToRect(const BoxInfo& tSrc, const BoxInfo& tDest)
{
	// Rect To Rect Collision (AABB)
	// BoxInfo
	// > Vector3	vMin; �ּ� ũ��
	// > Vector3	vMax; �ִ� ũ��
	// > Vector3 vLength; ����
	if (tSrc.vMin.x > tDest.vMax.x)			{ return false; }
	else if (tSrc.vMax.x < tDest.vMin.x)	{ return false; }
	else if (tSrc.vMin.y > tDest.vMax.y)	{ return false; }
	else if (tSrc.vMax.y < tDest.vMin.y)	{ return false; }

	return true;
}

bool CCollider::CollisionRectToPoint(const BoxInfo& _tSrc, const Vector3& _vDest)
{
	// Rect To Point Collision
	// ���콺�� ��ǥ(Point)�� �簢��(Rect) �������� �������� �浹
	if (_vDest.x < _tSrc.vMin.x)		{ return false; }
	else if (_vDest.x > _tSrc.vMax.x)	{ return false; }
	else if (_vDest.y < _tSrc.vMin.y)	{ return false; }
	else if (_vDest.y > _tSrc.vMax.y)	{ return false; }

	return true;
}

bool CCollider::CollisionCircleToCircle(const CircleInfo& _tSrc, const CircleInfo& _tDest)
{
	//�� �� ������ �Ÿ�
	float distance = _tSrc.vCenter.Distance(_tDest.vCenter);
	if (distance <= _tSrc.fRadius + _tDest.fRadius) 
	{
		return true;
	}

	return false;
}

bool CCollider::CollisionRectToCircle(const BoxInfo& _tSrc, const CircleInfo& _tDest)
{
	float rectL		 = _tSrc.vMin.x;
	float rectR		 = _tSrc.vMax.x;
	float rectT		 = _tSrc.vMax.y;
	float rectB		 = _tSrc.vMin.y;
	float circlePosX = _tDest.vCenter.x;
	float circlePosY = _tDest.vCenter.y;
	float radius	 = _tDest.fRadius;

	if ((rectL <= circlePosX && circlePosX <= rectR) || 
		(rectB <= circlePosY && circlePosY <= rectT))
	{
		// ������ ��ŭ Ȯ���� �簢���� ���Ѵ�.
		RECT rcEx = {
			(LONG)(rectL - radius), // Left
			(LONG)(rectT + radius), // Top
			(LONG)(rectR + radius), // Right
			(LONG)(rectB - radius), // Bottom
		};

		// Ȯ���� �簢���ȿ� ���� �߽��� ����ִ��� �ľ�
		if ((rcEx.left < circlePosX && circlePosX < rcEx.right) &&
			(rcEx.top > circlePosY && circlePosY > rcEx.bottom))
		{
			return true;
		}
	}
	else
	{
		// Min 
		if (CollisionCircleToPoint(_tDest, _tSrc.vMin) == true)
		{
			return true;
		}
		// Max
		if (CollisionCircleToPoint(_tDest, _tSrc.vMax) == true)
		{
			return true;
		}
		// Left, Top
		Vector3 vLeftTop = {_tSrc.vMin.x, _tSrc.vMax.y, 1.0f};
		if (CollisionCircleToPoint(_tDest, vLeftTop) == true)
		{
			return true;
		}
		// Right, Bottom
		Vector3 vRightBottom = { _tSrc.vMax.x, _tSrc.vMin.y, 1.0f };
		if (CollisionCircleToPoint(_tDest, vRightBottom) == true)
		{
			return true;
		}
	}

	return false;
}

bool CCollider::CollisionCircleToPoint(const CircleInfo& _tSrc, const Vector3& _vDest)
{
	// ���� �߽����� ���� ��ǥ�� ���̸� ���� ���� ������ ���� ���� ���̰� �۾����� �浹 ����
	// > ��Ÿ����� ������ �̿��� ����̴�.
	float deltaX = _tSrc.vCenter.x - _vDest.x;
	float deltaY = _tSrc.vCenter.y - _vDest.y;
	float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
	if (distance <= _tSrc.fRadius)
	{
		return true;
	}

	return false;
}

bool CCollider::CollisionOBB2DRectToOBB2DRect(const OBB2DInfo& _tSrc, const OBB2DInfo& _tDest)
{
	// OBB To OBB
	// > ������ ������, ������ �̿��Ͽ� '�и� ��'�� �̿��Ͽ� �浹 �˻�
	// > 2D OBB �浹�� Src, Dest ������ �浹ü�� x, y ���� ����Ѵ�.

	// �� �浹ü�� x, y �࿡ ���� ���� ���� �̸� ����Ѵ�.
	// Src x  -> Dest x, y
	// Src y  -> Dest x, y
	// Dest x ->  Src x, y
	// Dest y ->  Src x, y
	float fDot[2][4];
	fDot[0][0] = abs(_tSrc.vAxis[AXIS_X].Dot(_tDest.vAxis[AXIS_X]));
	fDot[0][1] = abs(_tSrc.vAxis[AXIS_X].Dot(_tDest.vAxis[AXIS_Y]));
	fDot[0][2] = abs(_tSrc.vAxis[AXIS_Y].Dot(_tDest.vAxis[AXIS_X]));
	fDot[0][3] = abs(_tSrc.vAxis[AXIS_Y].Dot(_tDest.vAxis[AXIS_Y]));
	fDot[1][0] = abs(_tDest.vAxis[AXIS_X].Dot(_tSrc.vAxis[AXIS_X]));
	fDot[1][1] = abs(_tDest.vAxis[AXIS_X].Dot(_tSrc.vAxis[AXIS_Y]));
	fDot[1][2] = abs(_tDest.vAxis[AXIS_Y].Dot(_tSrc.vAxis[AXIS_X]));
	fDot[1][3] = abs(_tDest.vAxis[AXIS_Y].Dot(_tSrc.vAxis[AXIS_Y]));

	// Src Center���� Dest Center�� ���ϴ� ����
	Vector3	vLength = _tDest.vCenter - _tSrc.vCenter;

	// 1. Src AxisX�� ��� 
	// > �������Ϳ� ���� �����ϸ� AxisX ������ vLength ���Ͱ� ������ ����(��Į��)�� ���´�.
	// > fR1 = Src�� x ����
	// > fR2 = ȸ�� ���� �����ϱ� ���� �и��Ͽ� ���� ���� ���ϰ� ������ ��Į�� ���� ���� ��
	float fLength = abs(_tSrc.vAxis[AXIS_X].Dot(vLength));
	float fR1 = _tSrc.fLength[0];
	float fR2 = (fDot[0][0] * _tDest.fLength[0]) + (fDot[0][1] * _tDest.fLength[1]);
	if (fLength > fR1 + fR2)
	{
		return false;
	}

	// 2. Src AxisY�� ���
	fLength = abs(_tSrc.vAxis[AXIS_Y].Dot(vLength));
	fR1 = _tSrc.fLength[1];
	fR2 = (fDot[0][2] * _tDest.fLength[0]) + (fDot[0][3] * _tDest.fLength[1]);
	if (fLength > fR1 + fR2)
	{
		return false;
	}

	// 3. Dest AxisX�� ���
	fLength = abs(_tDest.vAxis[AXIS_X].Dot(vLength));
	fR1 = _tDest.fLength[0];
	fR2 = (fDot[1][0] * _tSrc.fLength[0]) + (fDot[1][1] * _tSrc.fLength[1]);
	if (fLength > fR1 + fR2)
	{
		return false;
	}

	// 4. Dest AxisY�� ���
	fLength = abs(_tDest.vAxis[AXIS_Y].Dot(vLength));
	fR1 = _tDest.fLength[1];
	fR2 = (fDot[1][2] * _tSrc.fLength[0]) + (fDot[1][3] * _tSrc.fLength[1]);
	if (fLength > fR1 + fR2)
	{
		return false;
	}

	return true;
}

bool CCollider::CollisionOBB2DRectToRect(const OBB2DInfo& _tSrc, const BoxInfo& _tDest)
{
	// _tDest �浹ü�� Rect Ÿ���� �浹ü���
	// Rect -> OBB �浹 Ÿ������ �����Ͽ� OBB �浹�� �˻��Ѵ�.
	OBB2DInfo tInfo;
	tInfo.vCenter		 = (_tDest.vMin + _tDest.vMax) / 2.f;
	tInfo.vAxis[AXIS_X]	 = Vector3::Axis[AXIS_X];
	tInfo.vAxis[AXIS_Y]	 = Vector3::Axis[AXIS_Y];
	tInfo.fLength[0]	 = _tDest.vLength.x / 2.f; 
	tInfo.fLength[1]	 = _tDest.vLength.y / 2.f;

	return CollisionOBB2DRectToOBB2DRect(_tSrc, tInfo);
}

bool CCollider::CollisionOBB2DRectToCircle(const OBB2DInfo& _tSrc, const CircleInfo& _tDest)
{
	float fAngle = _tSrc.vAxis[AXIS_X].Angle(Vector3(1.f, 0.f, 0.f));
	if (_tSrc.vAxis[AXIS_X].y < 0.f)
	{
		fAngle = 360.f - fAngle;
	}

	Matrix matRot;
	matRot.RotationZ(-fAngle);
	memcpy(&matRot._41, &_tSrc.vCenter, sizeof(Vector3));

	Vector3	vPos = _tDest.vCenter - _tSrc.vCenter;
	vPos = vPos.TransformCoord(matRot);

	BoxInfo	tInfo;
	tInfo.vMin = _tSrc.vCenter - Vector3(_tSrc.fLength[0], _tSrc.fLength[1], 0.f);
	tInfo.vMax = _tSrc.vCenter + Vector3(_tSrc.fLength[0], _tSrc.fLength[1], 0.f);

	CircleInfo tCircleInfo;
	tCircleInfo.vCenter = vPos;
	tCircleInfo.fRadius = _tDest.fRadius;

	return CollisionRectToCircle(tInfo, tCircleInfo);
}

bool CCollider::CollisionOBB2DRectToPoint(const OBB2DInfo& _tSrc, const Vector3& _vDest)
{
	// OBB ������ Z������ ȸ�� ����ŭ Point�� Z������ ȸ�� ��Ų��.
	// �̸� ���Ͽ� Rect To Point �浹ó���� ������ �����ϴ�.

	// OBB�� Z�� ȸ�� ������ ���Ѵ�.
	float fAngle = _tSrc.vAxis[AXIS_X].Angle(Vector3(1.f, 0.f, 0.f));
	if (_tSrc.vAxis[AXIS_X].y < 0.f)
	{
		fAngle = 360.f - fAngle;
	}

	Matrix	matRot;
	matRot.RotationZ(-fAngle);
	memcpy(&matRot._41, &_tSrc.vCenter, sizeof(Vector3));

	Vector3	vPos = _vDest - _tSrc.vCenter;

	vPos = vPos.TransformCoord(matRot);

	BoxInfo	tInfo;
	tInfo.vMin = _tSrc.vCenter - Vector3(_tSrc.fLength[0], _tSrc.fLength[1], 0.f);
	tInfo.vMax = _tSrc.vCenter + Vector3(_tSrc.fLength[0], _tSrc.fLength[1], 0.f);

	return CollisionRectToPoint(tInfo, vPos);
}

bool CCollider::CollisionRectToPixel(BoxInfo _tSrc, const PixelInfo& _tDest)
{
	if (CollisionRectToRect(_tSrc, _tDest.tBox) == false)
	{
		return false;
	}

	// Box�� �ȼ��������� ��ȯ�Ѵ�.
	_tSrc.vMin -= _tDest.tBox.vMin;
	_tSrc.vMax -= _tDest.tBox.vMin;

	_tSrc.vMin.x = _tSrc.vMin.x < 0.f ? 0.f : _tSrc.vMin.x;
	_tSrc.vMin.y = _tSrc.vMin.y < 0.f ? 0.f : _tSrc.vMin.y;

	_tSrc.vMax.x = _tSrc.vMax.x > _tDest.iWidth ? _tDest.iWidth : _tSrc.vMax.x;
	_tSrc.vMax.y = _tSrc.vMax.y > _tDest.iHeight ? _tDest.iHeight : _tSrc.vMax.y;

	for (int i = (int)_tSrc.vMin.y; i < (int)_tSrc.vMax.y; ++i)
	{
		for (int j = (int)_tSrc.vMin.x; j < (int)_tSrc.vMax.x; ++j)
		{
			int	iIndex = i * _tDest.iWidth + j;

			Pixel24& tPixel = _tDest.pPixel[iIndex];

			if (tPixel.r != 0 || tPixel.g != 0 || tPixel.b != 0)
			{
				return true;
			}
		}
	}

	return false;
}

// ========== �浹 ���� ==========

void CCollider::OnCollisionEnter(CCollider* pDest, float fTime)
{
	// �浹 ����
	list<function<void(CCollider*, CCollider*, float)>>::iterator iter;
	list<function<void(CCollider*, CCollider*, float)>>::iterator iterEnd = m_collisionFunc[CCT_ENTER].end();
	for (iter = m_collisionFunc[CCT_ENTER].begin(); iter != iterEnd; ++iter)
	{
		(*iter)(this, pDest, fTime);
	}
}

void CCollider::OnCollision(CCollider* pDest, float fTime)
{
	// �浹 ��
	list<function<void(CCollider*, CCollider*, float)>>::iterator iter;
	list<function<void(CCollider*, CCollider*, float)>>::iterator iterEnd = m_collisionFunc[CCT_STAY].end();
	for (iter = m_collisionFunc[CCT_STAY].begin(); iter != iterEnd; ++iter)
	{
		(*iter)(this, pDest, fTime);
	}
}

void CCollider::OnCollisionLeave(CCollider* pDest, float fTime)
{
	// �浹 ��
	list<function<void(CCollider*, CCollider*, float)>>::iterator iter;
	list<function<void(CCollider*, CCollider*, float)>>::iterator iterEnd = m_collisionFunc[CCT_LEAVE].end();
	for (iter = m_collisionFunc[CCT_LEAVE].begin(); iter != iterEnd; ++iter)
	{
		(*iter)(this, pDest, fTime);
	}
}

// ========== �浹 �ݹ� �Լ� ��� ==========

void CCollider::SetCollisionCallback(COLLISION_CALLBACK_TYPE _eType,
									 void(*_pFunc)(CCollider*, CCollider*, float))
{
	// �浹 �� �ݹ� �Լ� �߰�
	function<void(CCollider*, CCollider*, float)> func;
	func = bind(_pFunc, placeholders::_1, placeholders::_2, placeholders::_3);
	m_collisionFunc[_eType].push_back(func);
}