#include "PCHEngine.h"
#include "CollisionManager.h"
#include "Device.h"
#include "Object/GameObject.h"
#include "Component/Transform.h"

ENGINE_USING
DEFINITION_SINGLE(CCollisionManager)

CCollisionManager::CCollisionManager()
{
}

CCollisionManager::~CCollisionManager()
{
	Safe_Delete_Map<unordered_map<string, PCollisionGroup>>(m_mapGroup);
}

bool CCollisionManager::Init()
{
	// 충돌공간은 최소 2개는 있어야 한다.
	// > UI
	//   - Resolution Size의 공간을 X(4), Y(4), Z(1) 만큼 분할한다. 
	CreateGroup("UI", Vector3(0.f, 0.f, 0.f), Vector3((float)_RESOLUTION.iWidth, 
				(float)_RESOLUTION.iHeight, 0.f), 4, 4, 1, CGT_2D);

	CreateGroup("BackGround", Vector3(0.f, 0.f, 0.f), Vector3(5000.f, 5000.f, 0.f),
				10, 10, 1, CGT_2D);

	// Default
	// > Size(임의의 공간 5000, 5000, 0) 화면에 안보이는 공간도 충돌 처리를 하고 있어야한다.
	//   - Size의 공간을 X(10), Y(10), Z(10) 만큼 분할한다.
	CreateGroup("Default", Vector3(0.f, 0.f, 0.f), 
				Vector3(5000.f, 5000.f, 0.f), 10, 10, 1, CGT_2D);

	return true;
}

bool CCollisionManager::CreateGroup(const string& _strKey, const Vector3& _vMin, const Vector3& _vMax, 
									int _iCountX, int _iCountY, int _iCountZ, COLLISION_GROUP_TYPE _eGroupType)
{
	// 충돌 그룹 생성
	PCollisionGroup pGroup = FindGroup(_strKey);
	if (pGroup != nullptr)
	{
		return false;
	}

	// 생성
	pGroup				 = new CollisionGroup();
	pGroup->vMin		 = _vMin;
	pGroup->vMax		 = _vMax;
	pGroup->iCountX		 = _iCountX;						// X축 분할 갯수
	pGroup->iCountY		 = _iCountY;						// Y축 분할 갯수
	pGroup->iCountZ		 = _iCountZ;						// Z축 분할 갯수
	pGroup->iCount		 = _iCountX * _iCountY * _iCountZ;	//  총 분할 갯수
	pGroup->eType		 = _eGroupType;
	pGroup->vLength		 = _vMax - _vMin;					// 길이(Vector - Vector3)
	// 충돌 공간을 각 분할(Section) 크기만큼 나눈다.
	pGroup->vCellLength  = pGroup->vLength / Vector3(_iCountX, _iCountY, _iCountZ);
	pGroup->pSectionList = new CollisionSection[pGroup->iCount];

	// 추가
	m_mapGroup.insert(make_pair(_strKey, pGroup));

	return true;
}

void CCollisionManager::AddCollision(CGameObject* _pObj)
{
	// ADdCollision() 호출 시점
	// > GameObject->Collision(this)
	// > Collision() 호출부에서 오브젝트는 자신을 등록한다.

	// 오브젝트가 가지고 있는 컴포넌트 중 CT_COLLIDER 타입이 없다면 false
	if (_pObj->CheckComponentFromType(CT_COLLIDER) == false)
	{
		return;
	}

	// 컴포넌트 중 CT_COLLIDER 타입의 컴포넌트가 있다면 전부 가져온다.
	const list<CComponent*>* pCollList = _pObj->FindComponentsFromType(CT_COLLIDER);
	list<CComponent*>::const_iterator iter;
	list<CComponent*>::const_iterator iterEnd = pCollList->end();
	for (iter = pCollList->begin(); iter != iterEnd; ++iter)
	{
		// UpCasting...
		CCollider* pCollider = (CCollider*)(*iter);

		// 자신이 속한 충돌 그룹
		// > 자신이 속한 그룹이 없다면? 프로그램 종료
		string groupName = pCollider->GetCollisionGroup();
		PCollisionGroup	pGroup = FindGroup(groupName);
		if (pGroup == nullptr)
		{
			tassert(true);
			return;
		}

		// 오브젝트는 매 프레임 움직 일 수 있으므로, 자신의 충돌체를 가지는 섹션을 초기화한다.
		pCollider->ClearCollisionSection();

		// ColliderSize(Min, Max), 충돌체 크기
		Vector3	vColliderSizeMin = pCollider->GetColliderSizeMin();
		Vector3	vColliderSizeMax = pCollider->GetColliderSizeMax();

		// 충돌그룹의 섹션 인덱스 범위
		int	xMin = 0, xMax = 0, yMin = 0, yMax = 0, zMin = 0, zMax = 0;
		xMin = (int)(vColliderSizeMin.x - pGroup->vMin.x) / (int)pGroup->vCellLength.x;
		xMax = (int)(vColliderSizeMax.x - pGroup->vMin.x) / (int)pGroup->vCellLength.x;
		yMin = (int)(vColliderSizeMin.y - pGroup->vMin.y) / (int)pGroup->vCellLength.y;
		yMax = (int)(vColliderSizeMax.y - pGroup->vMin.y) / (int)pGroup->vCellLength.y;
		// Z(3D)
		// > 현재는 2D 포트폴리오 진행 중이라 필요 없다.
		if (pGroup->iCountZ > 1)
		{
			zMin = (int)(vColliderSizeMin.z - pGroup->vMin.z) / (int)pGroup->vCellLength.z;
			zMax = (int)(vColliderSizeMax.z - pGroup->vMin.z) / (int)pGroup->vCellLength.z;
		}

		// 최소 한번은 실행
		// > x, y 값이 음수 또는 축 분할 크기보다 크면 continue ...
		// > min(x, y, z) ~ max(x, y, z) 충돌 영역의 범위다.
		for (int z = zMin; z <= zMax; ++z)
		{
			if (z < 0 || z >= pGroup->iCountZ) { continue; }
			for (int y = yMin; y <= yMax; ++y)
			{
				if (y < 0 || y >= pGroup->iCountY) { continue; }
				for (int x = xMin; x <= xMax; ++x)
				{
					if (x < 0 || x >= pGroup->iCountX) { continue; }

					// 자신의 충돌체 가지는 섹션 인덱스를 추가
					int	index = z * (pGroup->iCountX * pGroup->iCountY) + y * pGroup->iCountX + x;
					pCollider->AddCollisionSection(index);

					/*
						PCollisionSection 
						> iCapacity
						> iColliderCount   = 충돌체 갯수
						> pColliderList**  = Collider* 값을 가지는 이중 포인터
					*/

					// > 그룹에 존재하는 해당 인덱스의 섹션을 가지고 온다.
					PCollisionSection pSection = &pGroup->pSectionList[index];

					// Capacity 값과 iColliderCount가 같은 경우 Capacity값을 늘린다.
					// > 메모리 복사
					if (pSection->iCapacity == pSection->iColliderCount)
					{
						pSection->iCapacity = (int)(pSection->iCapacity * 1.5f);
						CCollider**	pArray  = new CCollider*[(pSection->iCapacity)];
						memcpy(pArray, pSection->pColliderList, sizeof(CCollider*) * pSection->iColliderCount);
						SAFE_DELETE_ARRAY(pSection->pColliderList);
						pSection->pColliderList = pArray;
					}

					// 섹션에 충돌체 추가, 충돌체 카운트 증가
					// > CollisionObject() 함수에서 iColliderCount를 0으로 해주지 않는다면? 계속 쌓이게 된다.
					pSection->pColliderList[pSection->iColliderCount] = pCollider;
					++pSection->iColliderCount;
				}
			}
		}
	}
}

void CCollisionManager::ChangeGroupType(const string& _strGroup, COLLISION_GROUP_TYPE _eType)
{
	// 충돌 그룹 타입을 변경한다.
	PCollisionGroup	pGroup = FindGroup(_strGroup);
	if (pGroup == nullptr)
	{
		return;
	}
	pGroup->eType = _eType;
}

void CCollisionManager::ClearCollisionGroup()
{
	// 충돌 그룹 전체 삭제
	Safe_Delete_Map<unordered_map<string, PCollisionGroup>>(m_mapGroup);
}

CCollisionManager::PCollisionGroup CCollisionManager::FindGroup(const string& _strGroup)
{
	// 충돌 그룹 찾기
	unordered_map<string, PCollisionGroup>::iterator iter = m_mapGroup.find(_strGroup);
	if (iter == m_mapGroup.end())
	{
		return nullptr;
	}
	return iter->second;
}