#pragma once
#include "Utility/Types.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Component/Component.h"

ENGINE_BEGIN
class CMesh;
class CShader;
class CRenderState;
class ENGINE_DLL CCollider : public CComponent
{
private:
	friend class CGameObject;

protected:
	CCollider();
	CCollider(const CCollider& _com);
	virtual ~CCollider();

	/* 상속 받은 재정의 함수 */
public:
	virtual bool Init();
	virtual int  Input(float _fTime);
	virtual int  Update(float _fTime);
	virtual int  LateUpdate(float _fTime);
	virtual void Collision(float _fTime);
	virtual void Render(float _fTime);
	virtual CCollider* Clone() = 0;

	// > 실제 충돌체 처리 (순수 가상 함수)
	virtual bool Collision(CCollider* _pDest, float _fTime) = 0;

private:
	eUserColType m_userColType;

public:
	void SetUserColType(eUserColType _type);
	eUserColType GetUserColType();

#ifdef _DEBUG
	Vector4				m_vColor;					// 충돌체 영역 색상
	CShader*			m_pShader;					// 셰이더 처리 객체
	CMesh*				m_pMesh;					// 충돌체 영역 메시
	CRenderState*		m_pDepthDisable;			// 충돌체 렌더링 상태
	ID3D11InputLayout*	m_pLayout;					// 충돌체 입력 레이아웃 
#endif // _DEBUG

protected:
	bool			 m_bUpdateCollision;
	string			 m_strCollisionGroup;			// 충돌 그룹 이름
	COLLIDER_TYPE	 m_eCollType;					// 충돌 타입
	Vector3			 m_vPivot;						// 충돌 중심점
	Vector3			 m_vColliderSizeMin;			// 충돌체 최소 크기
	Vector3			 m_vColliderSizeMax;			// 충돌체 최대 크기
	list<CCollider*> m_prevCollider;				// 이전에 충돌된 충돌체 리스트
	list<int>		 m_collisionSectionList;		// 충돌체(ColliderType)를 포함하고 있는 섹션 리스트
													// > 분할된 Index 값으로 저장

	void AAAA()
	{
	}
	// 충돌 상태에 따른 함수
	// > std::function Parameter Type (Collider* Collider* float) 
	list<function<void(CCollider*, CCollider*, float)>>	m_collisionFunc[CCT_END]; 

/* 충돌체 정보 가져오기 */
public:
	bool			 GetUpdateCollision()	const;	
	string			 GetCollisionGroup()	const;	// 충돌 그룹 이름
	COLLIDER_TYPE	 GetColliderType()		const;	// 충돌체 타입
	Vector3			 GetColliderSizeMin()	const;	// 충돌체 영역(최소), 실제 충돌 범위	
	Vector3			 GetColliderSizeMax()	const;	// 충돌체 영역(최대), 실제 충돌 범위
	const list<int>* GetColisionSection()	const;	// 분할 공간 섹션(인덱스) 리스트 가져오기

/* 충돌 그룹, 충돌체 설정 */
public:
	// > 충돌 그룹
	void ClearCollisionSection();
	void AddCollisionSection(int _iSection);
	void SetCollisionGroup(const string& _strCollisionGroup);

	// > 충돌체 중심점
	void SetPivot(const Vector3& _vPivot);
	void SetPivot(float _x, float _y, float _z);

	// > 충돌체 추가, 삭제, 이전 충돌체 검사, 삭제
	void AddPrevCollider(CCollider* _pDest);
	void ErasePrevCollider(CCollider* _pDest);
	bool CheckPrevCollider(CCollider* _pDest);
	void CheckPrevColliderInSection(float _fTime);

/* 충돌 타입연산 */
public:
	// 01. 사각형과 점 충돌				: CollisionRectToPoint			(O)
	// 02. 사각형과 원 충돌				: CollisionRectToCircle			(O)
	// 03. 사각형과 사각형 충돌			: CollisionRectToRect			(O)
	// 04. 사각형과 픽셀 충돌				: CollisionRectToPixel			(O)
	// 05. 원과 점 충돌					: CollisionCircleToPoint		(O)
	// 06. 원과 원 충돌					: CollisionCircleToCircle		(O)
	// 07. 원과 픽셀 충돌					: (미구현)
	// 08. OBB 사각형과 점 충돌			: CollisionOBB2DToPoint			(O)
	// 09. OBB 사각형과 원 충돌			: CollisionOBB2DToCircle		(O)
	// 10. OBB 사각형과 사각형 충돌		: CollisionOBB2DToRect			(O)
	// 11. OBB 사각형과 OBB 사각형 충돌	: CollisionOBB2DRectToOBB2DRect	(O)
	bool CollisionRectToPoint	(const BoxInfo&		_tSrc,	const Vector3&		_vDest);
	bool CollisionRectToCircle	(const BoxInfo&		_tSrc,	const CircleInfo&	_tDest);
	bool CollisionRectToRect	(const BoxInfo&		_tSrc,	const BoxInfo&		_tDest);
	bool CollisionCircleToPoint	(const CircleInfo&	_tSrc,	const Vector3&		_vDest);
	bool CollisionCircleToCircle(const CircleInfo&	_tSrc,	const CircleInfo&	_tDest);
	bool CollisionRectToPixel	(	   BoxInfo		_tSrc,  const PixelInfo&	_tDest);

	// > OBB
	bool CollisionOBB2DRectToPoint	(const OBB2DInfo&	_tSrc,	const Vector3&		_vDest);
	bool CollisionOBB2DRectToCircle	(const OBB2DInfo&	_tSrc,	const CircleInfo&	_tDest);
	bool CollisionOBB2DRectToRect	(const OBB2DInfo&	_tSrc,	const BoxInfo&		_tDest);
	bool CollisionOBB2DRectToOBB2DRect(const OBB2DInfo&	_tSrc,  const OBB2DInfo&	_tDest);

/* 충돌 상태 */
public:
	void OnCollisionEnter(CCollider* _pDest, float _fTime);
	void OnCollision(CCollider* _pDest, float _fTime);
	void OnCollisionLeave(CCollider* _pDest, float _fTime);

/* 충돌 콜백 함수 등록 */
public:
	void SetCollisionCallback(COLLISION_CALLBACK_TYPE _eType,
							  void(*_pFunc)(CCollider*, CCollider*, float));

	template <typename T>
	void SetCollisionCallback(COLLISION_CALLBACK_TYPE _eType,
							  T* _pObj, void (T::*_pFunc)(CCollider*, CCollider*, float))
	{
		function<void(CCollider*, CCollider*, float)> func;
		func = bind(_pFunc, _pObj, placeholders::_1, placeholders::_2, placeholders::_3);
		m_collisionFunc[_eType].push_back(func);
	}
};
ENGINE_END