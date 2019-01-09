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
	// �浹������ �ּ� 2���� �־�� �Ѵ�.
	// > UI
	//   - Resolution Size�� ������ X(4), Y(4), Z(1) ��ŭ �����Ѵ�. 
	CreateGroup("UI", Vector3(0.f, 0.f, 0.f), Vector3((float)_RESOLUTION.iWidth, 
				(float)_RESOLUTION.iHeight, 0.f), 4, 4, 1, CGT_2D);

	CreateGroup("BackGround", Vector3(0.f, 0.f, 0.f), Vector3(5000.f, 5000.f, 0.f),
				10, 10, 1, CGT_2D);

	// Default
	// > Size(������ ���� 5000, 5000, 0) ȭ�鿡 �Ⱥ��̴� ������ �浹 ó���� �ϰ� �־���Ѵ�.
	//   - Size�� ������ X(10), Y(10), Z(10) ��ŭ �����Ѵ�.
	CreateGroup("Default", Vector3(0.f, 0.f, 0.f), 
				Vector3(5000.f, 5000.f, 0.f), 10, 10, 1, CGT_2D);

	return true;
}

bool CCollisionManager::CreateGroup(const string& _strKey, const Vector3& _vMin, const Vector3& _vMax, 
									int _iCountX, int _iCountY, int _iCountZ, COLLISION_GROUP_TYPE _eGroupType)
{
	// �浹 �׷� ����
	PCollisionGroup pGroup = FindGroup(_strKey);
	if (pGroup != nullptr)
	{
		return false;
	}

	// ����
	pGroup				 = new CollisionGroup();
	pGroup->vMin		 = _vMin;
	pGroup->vMax		 = _vMax;
	pGroup->iCountX		 = _iCountX;						// X�� ���� ����
	pGroup->iCountY		 = _iCountY;						// Y�� ���� ����
	pGroup->iCountZ		 = _iCountZ;						// Z�� ���� ����
	pGroup->iCount		 = _iCountX * _iCountY * _iCountZ;	//  �� ���� ����
	pGroup->eType		 = _eGroupType;
	pGroup->vLength		 = _vMax - _vMin;					// ����(Vector - Vector3)
	// �浹 ������ �� ����(Section) ũ�⸸ŭ ������.
	pGroup->vCellLength  = pGroup->vLength / Vector3(_iCountX, _iCountY, _iCountZ);
	pGroup->pSectionList = new CollisionSection[pGroup->iCount];

	// �߰�
	m_mapGroup.insert(make_pair(_strKey, pGroup));

	return true;
}

void CCollisionManager::AddCollision(CGameObject* _pObj)
{
	// ADdCollision() ȣ�� ����
	// > GameObject->Collision(this)
	// > Collision() ȣ��ο��� ������Ʈ�� �ڽ��� ����Ѵ�.

	// ������Ʈ�� ������ �ִ� ������Ʈ �� CT_COLLIDER Ÿ���� ���ٸ� false
	if (_pObj->CheckComponentFromType(CT_COLLIDER) == false)
	{
		return;
	}

	// ������Ʈ �� CT_COLLIDER Ÿ���� ������Ʈ�� �ִٸ� ���� �����´�.
	const list<CComponent*>* pCollList = _pObj->FindComponentsFromType(CT_COLLIDER);
	list<CComponent*>::const_iterator iter;
	list<CComponent*>::const_iterator iterEnd = pCollList->end();
	for (iter = pCollList->begin(); iter != iterEnd; ++iter)
	{
		// UpCasting...
		CCollider* pCollider = (CCollider*)(*iter);

		// �ڽ��� ���� �浹 �׷�
		// > �ڽ��� ���� �׷��� ���ٸ�? ���α׷� ����
		string groupName = pCollider->GetCollisionGroup();
		PCollisionGroup	pGroup = FindGroup(groupName);
		if (pGroup == nullptr)
		{
			tassert(true);
			return;
		}

		// ������Ʈ�� �� ������ ���� �� �� �����Ƿ�, �ڽ��� �浹ü�� ������ ������ �ʱ�ȭ�Ѵ�.
		pCollider->ClearCollisionSection();

		// ColliderSize(Min, Max), �浹ü ũ��
		Vector3	vColliderSizeMin = pCollider->GetColliderSizeMin();
		Vector3	vColliderSizeMax = pCollider->GetColliderSizeMax();

		// �浹�׷��� ���� �ε��� ����
		int	xMin = 0, xMax = 0, yMin = 0, yMax = 0, zMin = 0, zMax = 0;
		xMin = (int)(vColliderSizeMin.x - pGroup->vMin.x) / (int)pGroup->vCellLength.x;
		xMax = (int)(vColliderSizeMax.x - pGroup->vMin.x) / (int)pGroup->vCellLength.x;
		yMin = (int)(vColliderSizeMin.y - pGroup->vMin.y) / (int)pGroup->vCellLength.y;
		yMax = (int)(vColliderSizeMax.y - pGroup->vMin.y) / (int)pGroup->vCellLength.y;
		// Z(3D)
		// > ����� 2D ��Ʈ������ ���� ���̶� �ʿ� ����.
		if (pGroup->iCountZ > 1)
		{
			zMin = (int)(vColliderSizeMin.z - pGroup->vMin.z) / (int)pGroup->vCellLength.z;
			zMax = (int)(vColliderSizeMax.z - pGroup->vMin.z) / (int)pGroup->vCellLength.z;
		}

		// �ּ� �ѹ��� ����
		// > x, y ���� ���� �Ǵ� �� ���� ũ�⺸�� ũ�� continue ...
		// > min(x, y, z) ~ max(x, y, z) �浹 ������ ������.
		for (int z = zMin; z <= zMax; ++z)
		{
			if (z < 0 || z >= pGroup->iCountZ) { continue; }
			for (int y = yMin; y <= yMax; ++y)
			{
				if (y < 0 || y >= pGroup->iCountY) { continue; }
				for (int x = xMin; x <= xMax; ++x)
				{
					if (x < 0 || x >= pGroup->iCountX) { continue; }

					// �ڽ��� �浹ü ������ ���� �ε����� �߰�
					int	index = z * (pGroup->iCountX * pGroup->iCountY) + y * pGroup->iCountX + x;
					pCollider->AddCollisionSection(index);

					/*
						PCollisionSection 
						> iCapacity
						> iColliderCount   = �浹ü ����
						> pColliderList**  = Collider* ���� ������ ���� ������
					*/

					// > �׷쿡 �����ϴ� �ش� �ε����� ������ ������ �´�.
					PCollisionSection pSection = &pGroup->pSectionList[index];

					// Capacity ���� iColliderCount�� ���� ��� Capacity���� �ø���.
					// > �޸� ����
					if (pSection->iCapacity == pSection->iColliderCount)
					{
						pSection->iCapacity = (int)(pSection->iCapacity * 1.5f);
						CCollider**	pArray  = new CCollider*[(pSection->iCapacity)];
						memcpy(pArray, pSection->pColliderList, sizeof(CCollider*) * pSection->iColliderCount);
						SAFE_DELETE_ARRAY(pSection->pColliderList);
						pSection->pColliderList = pArray;
					}

					// ���ǿ� �浹ü �߰�, �浹ü ī��Ʈ ����
					// > CollisionObject() �Լ����� iColliderCount�� 0���� ������ �ʴ´ٸ�? ��� ���̰� �ȴ�.
					pSection->pColliderList[pSection->iColliderCount] = pCollider;
					++pSection->iColliderCount;
				}
			}
		}
	}
}

void CCollisionManager::ChangeGroupType(const string& _strGroup, COLLISION_GROUP_TYPE _eType)
{
	// �浹 �׷� Ÿ���� �����Ѵ�.
	PCollisionGroup	pGroup = FindGroup(_strGroup);
	if (pGroup == nullptr)
	{
		return;
	}
	pGroup->eType = _eType;
}

void CCollisionManager::ClearCollisionGroup()
{
	// �浹 �׷� ��ü ����
	Safe_Delete_Map<unordered_map<string, PCollisionGroup>>(m_mapGroup);
}

CCollisionManager::PCollisionGroup CCollisionManager::FindGroup(const string& _strGroup)
{
	// �浹 �׷� ã��
	unordered_map<string, PCollisionGroup>::iterator iter = m_mapGroup.find(_strGroup);
	if (iter == m_mapGroup.end())
	{
		return nullptr;
	}
	return iter->second;
}