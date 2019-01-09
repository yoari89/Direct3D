#pragma once
#include "Component/Collider.h"

ENGINE_BEGIN
class ENGINE_DLL CCollisionManager
{
DECLARE_SINGLE(CCollisionManager)

private:
	// Section
	// > 쪼개진 하나의 공간(영역)
	// > 실제 충돌체(Collider) 목록이 필요하다.
	typedef struct ENGINE_DLL _tagCollisionSection
	{
		int iCapacity;
		int iColliderCount;			
		CCollider** pColliderList;	// 충돌체의 주소들을 받아서 저장해야 한다.
		
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

	// 충돌체 그룹
	// > 실제 충돌체를 저장할 그룹
	typedef struct ENGINE_DLL _tagCollisionGroup
	{
		int						iCountX;		// X축 분할 갯수
		int						iCountY;		// Y축 분할 갯수
		int						iCountZ;		// Z축 분할 갯수
		int						iCount;			//  총 분할 갯수
		COLLISION_GROUP_TYPE	eType;
		Vector3					vMin;			// 충돌 공간의 Min, Max
		Vector3					vMax;			
		Vector3					vLength;		// 충둘 공간의 길이 
		Vector3					vCellLength;	// 충돌 섹션(셀)의 길이
		PCollisionSection		pSectionList;	// 충돌 섹션 리스트

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

/* 충돌 공간 */
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

/* 타입 충돌 검사 */
// > CheckCollisionType.cpp
public:
	void Collision(float _fTime);
	bool CollisionMouseWindow(float _fTime);
	void CollisionMouseWorld(float _fTime);
	void CollisionObject(float _fTime);
};
ENGINE_END