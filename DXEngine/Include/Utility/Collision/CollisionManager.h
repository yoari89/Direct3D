#pragma once
#include "Component/Collider.h"

ENGINE_BEGIN
class ENGINE_DLL CCollisionManager
{
DECLARE_SINGLE(CCollisionManager)

private:
	// Section
	// > �ɰ��� �ϳ��� ����(����)
	// > ���� �浹ü(Collider) ����� �ʿ��ϴ�.
	typedef struct ENGINE_DLL _tagCollisionSection
	{
		int iCapacity;
		int iColliderCount;			
		CCollider** pColliderList;	// �浹ü�� �ּҵ��� �޾Ƽ� �����ؾ� �Ѵ�.
		
		_tagCollisionSection() 
		{
			iColliderCount = 0;
			iCapacity	   = 5;
			pColliderList  = new CCollider*[iCapacity];
		}
		~_tagCollisionSection()
		{
			SAFE_DELETE_ARRAY(pColliderList);
		}
	}CollisionSection, *PCollisionSection;

	// �浹ü �׷�
	// > ���� �浹ü�� ������ �׷�
	typedef struct ENGINE_DLL _tagCollisionGroup
	{
		int						iCountX;		// X�� ���� ����
		int						iCountY;		// Y�� ���� ����
		int						iCountZ;		// Z�� ���� ����
		int						iCount;			//  �� ���� ����
		COLLISION_GROUP_TYPE	eType;
		Vector3					vMin;			// �浹 ������ Min, Max
		Vector3					vMax;			
		Vector3					vLength;		// ��� ������ ���� 
		Vector3					vCellLength;	// �浹 ����(��)�� ����
		PCollisionSection		pSectionList;	// �浹 ���� ����Ʈ

		_tagCollisionGroup()
		{
		}
		~_tagCollisionGroup()
		{
			SAFE_DELETE_ARRAY(pSectionList);
		}
	}CollisionGroup, *PCollisionGroup;

private:
	unordered_map<string, PCollisionGroup> m_mapGroup;

/* �浹 ���� */
// > CollisionManager.cpp
public:
	bool Init();
	bool CreateGroup(const string& _strKey, const Vector3& _vMin, const Vector3& _vMax, 
					 int _iCountX, int _iCountY, int _iCountZ, COLLISION_GROUP_TYPE _eGroupType = CGT_3D);
	void AddCollision(class CGameObject* _pObj);
	void ChangeGroupType(const string& _strGroup, COLLISION_GROUP_TYPE _eType);
	void ClearCollisionGroup();
	
private:
	PCollisionGroup FindGroup(const string& _strGroup);

/* Ÿ�� �浹 �˻� */
// > CheckCollisionType.cpp
public:
	void Collision(float _fTime);
	bool CollisionMouseWindow(float _fTime);
	void CollisionMouseWorld(float _fTime);
	void CollisionObject(float _fTime);
};
ENGINE_END