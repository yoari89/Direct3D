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
	// 메모리 해제
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pDepthDisable);
#endif // _DEBUG

	// 충돌체 소멸시 자신과 충돌중인 오브젝트(m_prevCollider)는 리스트에서 삭제
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_prevCollider.end();
	for (iter = m_prevCollider.begin(); iter != iterEnd; ++iter)
	{
		// 소멸될 충돌체, 이전 충돌체에 대하여 OnCollisionLeave 호출
		OnCollisionLeave((*iter), 0.f);
		(*iter)->OnCollisionLeave(this, 0.f);

		// 이전 충돌체에서 소멸될 충돌체를 삭제시킨다.
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
	// 각 충돌체(Point, Rect, Circle, OBB, Pixel ...)는 하나의 컴포넌트
	// 즉, 게임 로직 구조에 의해서 함수가 호출되며, 호출자는 위에 얘기한 자식들이 호출자가 된다.
	// 각 호출자는 자신만의 메시, 셰이더, 렌더링 상태, 입력 레이아웃을 설정할 수 있다.
	// 이를 통하여 최종적으로 렌더링 준비 상태를 마치며, 메시를 통하여 렌더링을 시작한다.

	if (m_prevCollider.empty() == true) { m_vColor = Vector4::Green; }
	else								{ m_vColor = Vector4::Red; }

	// 충돌체 전용 셰이더 (Collider.fx)
	m_pShader->SetShader();

	// 입력 레이아웃 
	// > POS_LAYOUT : Vector3 정보를 가지는 버텍스 버퍼 구조
	CONTEXT->IASetInputLayout(m_pLayout);

	// 상수 버퍼 업데이트
	// > float4 타입를 가진 상수 버퍼, 색상 정보를 담기 위한 상수 버퍼
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Collider", &m_vColor);

	// 렌더링 상태 설정
	// > AlphaBlend   = 사용 하지 않음
	// > DepthStencil = 2D게임에서는 사용하지 않는다.
	if (m_pDepthDisable != nullptr)
	{
		m_pDepthDisable->SetState();
	}

	// 렌더링 시작
	m_pMesh->Render();

	// 렌더링 상태 기본 값으로 설정
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

// ========== 충돌체 정보 가져오기 ==========

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

// ========== 충돌 그룹, 충돌체 설정 ==========

void CCollider::ClearCollisionSection()
{
	// 섹션 리스트 초기화
	m_collisionSectionList.clear();
}

void CCollider::AddCollisionSection(int _iSection)
{
	// 섹션에 인덱스 추가
	m_collisionSectionList.push_back(_iSection);
}

void CCollider::SetCollisionGroup(const string& _strCollisionGroup)
{
	// 충돌 그룹 변경
	m_strCollisionGroup = _strCollisionGroup;
}

void CCollider::SetPivot(const Vector3& _vPivot)
{
	// 충돌체 중심점 설정(Vector3 Type)
	m_vPivot = _vPivot;
}

void CCollider::SetPivot(float _x, float _y, float _z)
{
	// 충돌체 중심점 설정(float Type)
	m_vPivot = Vector3(_x, _y, _z);
}

void CCollider::AddPrevCollider(CCollider* _pDest)
{
	// 호출자 충돌체에 충돌된 충돌체(_pDest)를 추가시킨다.
	m_prevCollider.push_back(_pDest);
}

void CCollider::ErasePrevCollider(CCollider* _pDest)
{
	// 호출자 충돌체와 끝난 시점의 충돌체(_pDest)를 삭제시킨다.
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
	// 호출자 충돌체와 매개변수 충돌체(_pDest)와 충돌 여부를 판단한다.
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
	// 호출자 충돌체 (섹션안에 존재하는 충돌체 목록)
	// 같은 충돌 그룹이 아니거나, 같은 섹션이 아니면 목록에서 지운다.  
	// Object To Object 충돌을 검사할 때 Enter, Stay, Leave 상태를 다시 검사한다.
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_prevCollider.end();
	for (iter = m_prevCollider.begin(); iter != iterEnd;)
	{
		// 충돌 그룹 검사
		if (m_strCollisionGroup != (*iter)->m_strCollisionGroup)
		{
			// 충돌 끝
			OnCollisionLeave((*iter), _fTime);
			(*iter)->OnCollisionLeave(this, _fTime);

			// 이전 충돌 객체에서 삭제
			(*iter)->ErasePrevCollider(this);
			iter	= m_prevCollider.erase(iter);
			iterEnd = m_prevCollider.end();
			continue;
		}

		// 자신의 충돌 섹션 리스트
		list<int>::iterator	Iter;
		list<int>::iterator	IterEnd = m_collisionSectionList.end();

		// 자신의 충돌체와 충돌된 충돌체의 섹션 리스트
		const list<int>* pSectionList = (*iter)->GetColisionSection();
		list<int>::const_iterator destIter;
		list<int>::const_iterator destIterEnd = pSectionList->end();

		// 섹션 검사
		bool isPair = false;
		for (destIter = pSectionList->begin(); destIter != destIterEnd; ++destIter)
		{
			for (Iter = m_collisionSectionList.begin(); Iter != IterEnd; ++Iter)
			{
				// 섹션 일치
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
		// 섹션 불일치
		if (isPair == false)
		{
			// 충돌 끝
			OnCollisionLeave((*iter), _fTime);
			(*iter)->OnCollisionLeave(this, _fTime);

			// 이전 충돌 객체에서 삭제
			(*iter)->ErasePrevCollider(this);
			iter	= m_prevCollider.erase(iter);
			iterEnd = m_prevCollider.end();
			continue;
		}
		++iter;
	}
}

// ========== 충돌 타입 연산 ==========

bool CCollider::CollisionRectToRect(const BoxInfo& tSrc, const BoxInfo& tDest)
{
	// Rect To Rect Collision (AABB)
	// BoxInfo
	// > Vector3	vMin; 최소 크기
	// > Vector3	vMax; 최대 크기
	// > Vector3 vLength; 길이
	if (tSrc.vMin.x > tDest.vMax.x)			{ return false; }
	else if (tSrc.vMax.x < tDest.vMin.x)	{ return false; }
	else if (tSrc.vMin.y > tDest.vMax.y)	{ return false; }
	else if (tSrc.vMax.y < tDest.vMin.y)	{ return false; }

	return true;
}

bool CCollider::CollisionRectToPoint(const BoxInfo& _tSrc, const Vector3& _vDest)
{
	// Rect To Point Collision
	// 마우스의 좌표(Point)가 사각형(Rect) 범위내에 들어왔으면 충돌
	if (_vDest.x < _tSrc.vMin.x)		{ return false; }
	else if (_vDest.x > _tSrc.vMax.x)	{ return false; }
	else if (_vDest.y < _tSrc.vMin.y)	{ return false; }
	else if (_vDest.y > _tSrc.vMax.y)	{ return false; }

	return true;
}

bool CCollider::CollisionCircleToCircle(const CircleInfo& _tSrc, const CircleInfo& _tDest)
{
	//두 원 사이의 거리
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
		// 반지름 만큼 확장한 사각형을 구한다.
		RECT rcEx = {
			(LONG)(rectL - radius), // Left
			(LONG)(rectT + radius), // Top
			(LONG)(rectR + radius), // Right
			(LONG)(rectB - radius), // Bottom
		};

		// 확장한 사각형안에 원의 중심이 들어있는지 파악
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
	// 원의 중심점과 점의 좌표의 길이를 구한 값이 반지름 보다 구한 길이가 작아지면 충돌 상태
	// > 피타고라스의 정리를 이용한 방법이다.
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
	// > 벡터의 내적과, 투영을 이용하여 '분리 축'을 이용하여 충돌 검사
	// > 2D OBB 충돌은 Src, Dest 각각의 충돌체의 x, y 축을 계산한다.

	// 각 충돌체의 x, y 축에 대한 내적 값을 미리 계산한다.
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

	// Src Center에서 Dest Center를 향하는 벡터
	Vector3	vLength = _tDest.vCenter - _tSrc.vCenter;

	// 1. Src AxisX축 계산 
	// > 단위벡터에 벡터 내적하면 AxisX 축으로 vLength 벡터가 투영된 길이(스칼라)가 나온다.
	// > fR1 = Src의 x 길이
	// > fR2 = 회전 값을 적용하기 위해 분리하여 내적 값을 곱하고 각각의 스칼라 값을 더한 값
	float fLength = abs(_tSrc.vAxis[AXIS_X].Dot(vLength));
	float fR1 = _tSrc.fLength[0];
	float fR2 = (fDot[0][0] * _tDest.fLength[0]) + (fDot[0][1] * _tDest.fLength[1]);
	if (fLength > fR1 + fR2)
	{
		return false;
	}

	// 2. Src AxisY축 계산
	fLength = abs(_tSrc.vAxis[AXIS_Y].Dot(vLength));
	fR1 = _tSrc.fLength[1];
	fR2 = (fDot[0][2] * _tDest.fLength[0]) + (fDot[0][3] * _tDest.fLength[1]);
	if (fLength > fR1 + fR2)
	{
		return false;
	}

	// 3. Dest AxisX축 계산
	fLength = abs(_tDest.vAxis[AXIS_X].Dot(vLength));
	fR1 = _tDest.fLength[0];
	fR2 = (fDot[1][0] * _tSrc.fLength[0]) + (fDot[1][1] * _tSrc.fLength[1]);
	if (fLength > fR1 + fR2)
	{
		return false;
	}

	// 4. Dest AxisY축 계산
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
	// _tDest 충돌체가 Rect 타입의 충돌체라면
	// Rect -> OBB 충돌 타입으로 변경하여 OBB 충돌로 검사한다.
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
	// OBB 상자의 Z축으로 회전 값만큼 Point를 Z축으로 회전 시킨다.
	// 이를 통하여 Rect To Point 충돌처리로 연산이 가능하다.

	// OBB의 Z축 회전 각도를 구한다.
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

	// Box를 픽셀공간으로 변환한다.
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

// ========== 충돌 상태 ==========

void CCollider::OnCollisionEnter(CCollider* pDest, float fTime)
{
	// 충돌 시작
	list<function<void(CCollider*, CCollider*, float)>>::iterator iter;
	list<function<void(CCollider*, CCollider*, float)>>::iterator iterEnd = m_collisionFunc[CCT_ENTER].end();
	for (iter = m_collisionFunc[CCT_ENTER].begin(); iter != iterEnd; ++iter)
	{
		(*iter)(this, pDest, fTime);
	}
}

void CCollider::OnCollision(CCollider* pDest, float fTime)
{
	// 충돌 중
	list<function<void(CCollider*, CCollider*, float)>>::iterator iter;
	list<function<void(CCollider*, CCollider*, float)>>::iterator iterEnd = m_collisionFunc[CCT_STAY].end();
	for (iter = m_collisionFunc[CCT_STAY].begin(); iter != iterEnd; ++iter)
	{
		(*iter)(this, pDest, fTime);
	}
}

void CCollider::OnCollisionLeave(CCollider* pDest, float fTime)
{
	// 충돌 끝
	list<function<void(CCollider*, CCollider*, float)>>::iterator iter;
	list<function<void(CCollider*, CCollider*, float)>>::iterator iterEnd = m_collisionFunc[CCT_LEAVE].end();
	for (iter = m_collisionFunc[CCT_LEAVE].begin(); iter != iterEnd; ++iter)
	{
		(*iter)(this, pDest, fTime);
	}
}

// ========== 충돌 콜백 함수 등록 ==========

void CCollider::SetCollisionCallback(COLLISION_CALLBACK_TYPE _eType,
									 void(*_pFunc)(CCollider*, CCollider*, float))
{
	// 충돌 시 콜백 함수 추가
	function<void(CCollider*, CCollider*, float)> func;
	func = bind(_pFunc, placeholders::_1, placeholders::_2, placeholders::_3);
	m_collisionFunc[_eType].push_back(func);
}