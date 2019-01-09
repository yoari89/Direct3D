#pragma once
#include "Component/Component.h"

ENGINE_BEGIN
class CGameObject;
class ENGINE_DLL ENGINE_ALIGN CTransform : public CComponent
{
private:
	friend CGameObject;

private:
	CTransform();
	CTransform(const CTransform& _tr);
	virtual ~CTransform();

/* align(16) new, delete ������ */ 
public:	
	void* operator new (size_t _iSize);
	void operator delete (void* _p);

/* ��� ���� ������ �Լ� */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	CTransform* Clone()				override;

/* ��� ��� Flag */
private:
	bool m_isUpdate;
	bool m_isStatic; // ���� ��ü �Ǻ�

public:
	void Static(bool _isStatic);

/* �θ�, �ڽ� Transform */
private:
	int					m_iParentFlag;
	CTransform*			m_pParentTransform;
	list<CTransform*>	m_childTransformList;

public:
	void SetParentFlag(int iFlag);
	void AddParentFlag(TRANSFORM_PARENT_FLAG eFlag);
	void DeleteParentFlag(TRANSFORM_PARENT_FLAG eFlag);
	void DeleteParentFlag();

/* ���� ���� */
private:
	// > Vector
	Vector3 m_vecLocalScale;
	Vector3 m_vecLocalRot;
	Vector3 m_vecLocalPos;
	Vector3 m_vecLocalAxis[AXIS_END];	// ���� ����

	// > Matrix
	Matrix m_matLocalScale;
	Matrix m_matLocalRotX;
	Matrix m_matLocalRotY;
	Matrix m_matLocalRotZ;
	Matrix m_matLocalRot;
	Matrix m_matLocalPos;
	Matrix m_matLocal;

// Local.cpp ���Ͽ� �Լ� ����
public:
	// > ũ��
	void SetLocalScale(const Vector3& _vecScale);
	void SetLocalScale(float _x, float _y, float _z);
	Vector3 GetLocalScale()	const;

	// > ȸ��
	void SetLocalRot(const Vector3& _vecRot);
	void SetLocalRot(float  _x, float _y, float _z);
	void SetLocalRotX(float _x);
	void SetLocalRotY(float _y);
	void SetLocalRotZ(float _z);
	Vector3 GetLocalRot() const;

	// > ��ġ
	void SetLocalPos(const Vector3& _vecPos);
	void SetLocalPos(float _x, float _y, float _z);
	Vector3 GetLocalPos() const;

	// > ���� ��
	void ComputeLocalAxis();
	const Vector3* GetLocalAxis()		const;
	Vector3 GetLocalAxis(AXIS _eAxis)	const;

	// > ���
	Matrix  GetLocalRotMatrix()			const;
	Matrix  GetLocalMatrix()			const;

/* ���� ���� */
private:
	// > Vector3
	Vector3 m_vecWorldScale;
	Vector3 m_vecWorldRot;
	Vector3 m_vecWorldPos;
	Vector3	m_vecPivot;
	Vector3	m_vWorldRelativeScale;
	Vector3	m_vWorldRelativeRot;
	Vector3	m_vWorldRelativePos;
	Vector3 m_vecWorldAxis[AXIS_END];	// ���� ����
	Vector3 m_vecWorldMove;

	// > Matrix
	Matrix m_matWorldScale;
	Matrix m_matWorldRotX;
	Matrix m_matWorldRotY;
	Matrix m_matWorldRotZ;
	Matrix m_matWorldRot;
	Matrix m_matWorldPos;
	Matrix m_matParent;				
	Matrix m_matWorld;

// World.cpp ���Ͽ� �Լ� ����
public:
	// > ũ��
	void SetWorldScale(const Vector3& _vecScale);
	void SetWorldScale(float _x, float _y, float _z);
	Vector3 GetWorldScale()	const;

	// > ȸ��
	void SetWorldRot(const Vector3& _vecRot);
	void SetWorldRot(float  _x, float _y, float _z);
	void SetWorldRotX(float _x);
	void SetWorldRotY(float _y);
	void SetWorldRotZ(float _z);
	Vector3 GetWorldRot() const;

	// > ��ġ
	void SetWorldPos(const Vector3& _vecPos);
	void SetWorldPos(float _x, float _y, float _z);
	void SetWorldRelativePos(const Vector3& _vPos);
	void SetWorldRelativePos(float _x, float _y, float _z);
	Vector3 GetWorldPos() const;
	Vector3 GetWorldMove() const;

	// > ����
	void SetWorldPivot(const Vector3& _vecPivot);
	void SetWorldPivot(float _x, float _y, float _z);

	// > �� ���
	void ComputeWorldAxis();
	Vector3 GetWorldAxis(AXIS _eAxis)	const;
	const Vector3* GetWorldAxis()		const;

	// > �Ǻ�
	Vector3 GetPivot()					const;

	// > ���
	Matrix GetWorldMatrix()				const;
	Matrix GetWorldRotMatrix()			const;
	

/* ��� ��� */
// MatrixCalc.cpp ���Ͽ� �Լ� ����
public:
	Matrix GetParentMatrix()			const;
	Matrix GetParentMatrixFromNoScale()	const;
	Matrix GetParentRotMatrix()			const;
	Matrix GetParentPosMatrix()			const;

	void Move(AXIS _eAxis, float _fSpeed);
	void Move(AXIS _eAxis, float _fSpeed, float _fTime);
	void Move(const Vector3& _vecDir, float _fSpeed);
	void Move(const Vector3& _vecDir, float _fSpeed, float _fTime);
	void Move(const Vector3& _vecMove);
	void RotationX(float _x);
	void RotationX(float _x, float _fTime);
	void RotationY(float _y);
	void RotationY(float _y, float _fTime);
	void RotationZ(float _z);
	void RotationZ(float _z, float _fTime);
	void Rotation(const Vector3& _vecRot, float _fTime);
	void Rotation(const Vector3& _vecRot);
	void LookAt(CGameObject* _pObj,   AXIS _eAxis = AXIS_Z);
	void LookAt(CComponent* _pCom,    AXIS _eAxis = AXIS_Z);
	void LookAt(const Vector3& _vPos, AXIS _eAxis = AXIS_Z);
	void UpdateTransform();

private:
	void CheckScaleParent();
	void Check_Rot_Pos_Parent();
};
ENGINE_END