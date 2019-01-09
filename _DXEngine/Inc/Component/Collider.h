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

	/* ��� ���� ������ �Լ� */
public:
	virtual bool Init();
	virtual int  Input(float _fTime);
	virtual int  Update(float _fTime);
	virtual int  LateUpdate(float _fTime);
	virtual void Collision(float _fTime);
	virtual void Render(float _fTime);
	virtual CCollider* Clone() = 0;

	// > ���� �浹ü ó�� (���� ���� �Լ�)
	virtual bool Collision(CCollider* _pDest, float _fTime) = 0;

private:
	eUserColType m_userColType;

public:
	void SetUserColType(eUserColType _type);
	eUserColType GetUserColType();

#ifdef _DEBUG
	Vector4				m_vColor;					// �浹ü ���� ����
	CShader*			m_pShader;					// ���̴� ó�� ��ü
	CMesh*				m_pMesh;					// �浹ü ���� �޽�
	CRenderState*		m_pDepthDisable;			// �浹ü ������ ����
	ID3D11InputLayout*	m_pLayout;					// �浹ü �Է� ���̾ƿ� 
#endif // _DEBUG

protected:
	bool			 m_bUpdateCollision;
	string			 m_strCollisionGroup;			// �浹 �׷� �̸�
	COLLIDER_TYPE	 m_eCollType;					// �浹 Ÿ��
	Vector3			 m_vPivot;						// �浹 �߽���
	Vector3			 m_vColliderSizeMin;			// �浹ü �ּ� ũ��
	Vector3			 m_vColliderSizeMax;			// �浹ü �ִ� ũ��
	list<CCollider*> m_prevCollider;				// ������ �浹�� �浹ü ����Ʈ
	list<int>		 m_collisionSectionList;		// �浹ü(ColliderType)�� �����ϰ� �ִ� ���� ����Ʈ
													// > ���ҵ� Index ������ ����

	void AAAA()
	{
	}
	// �浹 ���¿� ���� �Լ�
	// > std::function Parameter Type (Collider* Collider* float) 
	list<function<void(CCollider*, CCollider*, float)>>	m_collisionFunc[CCT_END]; 

/* �浹ü ���� �������� */
public:
	bool			 GetUpdateCollision()	const;	
	string			 GetCollisionGroup()	const;	// �浹 �׷� �̸�
	COLLIDER_TYPE	 GetColliderType()		const;	// �浹ü Ÿ��
	Vector3			 GetColliderSizeMin()	const;	// �浹ü ����(�ּ�), ���� �浹 ����	
	Vector3			 GetColliderSizeMax()	const;	// �浹ü ����(�ִ�), ���� �浹 ����
	const list<int>* GetColisionSection()	const;	// ���� ���� ����(�ε���) ����Ʈ ��������

/* �浹 �׷�, �浹ü ���� */
public:
	// > �浹 �׷�
	void ClearCollisionSection();
	void AddCollisionSection(int _iSection);
	void SetCollisionGroup(const string& _strCollisionGroup);

	// > �浹ü �߽���
	void SetPivot(const Vector3& _vPivot);
	void SetPivot(float _x, float _y, float _z);

	// > �浹ü �߰�, ����, ���� �浹ü �˻�, ����
	void AddPrevCollider(CCollider* _pDest);
	void ErasePrevCollider(CCollider* _pDest);
	bool CheckPrevCollider(CCollider* _pDest);
	void CheckPrevColliderInSection(float _fTime);

/* �浹 Ÿ�Կ��� */
public:
	// 01. �簢���� �� �浹				: CollisionRectToPoint			(O)
	// 02. �簢���� �� �浹				: CollisionRectToCircle			(O)
	// 03. �簢���� �簢�� �浹			: CollisionRectToRect			(O)
	// 04. �簢���� �ȼ� �浹				: CollisionRectToPixel			(O)
	// 05. ���� �� �浹					: CollisionCircleToPoint		(O)
	// 06. ���� �� �浹					: CollisionCircleToCircle		(O)
	// 07. ���� �ȼ� �浹					: (�̱���)
	// 08. OBB �簢���� �� �浹			: CollisionOBB2DToPoint			(O)
	// 09. OBB �簢���� �� �浹			: CollisionOBB2DToCircle		(O)
	// 10. OBB �簢���� �簢�� �浹		: CollisionOBB2DToRect			(O)
	// 11. OBB �簢���� OBB �簢�� �浹	: CollisionOBB2DRectToOBB2DRect	(O)
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

/* �浹 ���� */
public:
	void OnCollisionEnter(CCollider* _pDest, float _fTime);
	void OnCollision(CCollider* _pDest, float _fTime);
	void OnCollisionLeave(CCollider* _pDest, float _fTime);

/* �浹 �ݹ� �Լ� ��� */
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