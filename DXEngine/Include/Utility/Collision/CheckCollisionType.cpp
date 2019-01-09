#include "PCHEngine.h"
#include "CollisionManager.h"
#include "Utility/Input/Input.h"
#include "Object/GameObject.h"
#include "Component/ColliderPoint.h"

ENGINE_USING

void CCollisionManager::Collision(float _fTime)
{
	// ���� �浹 ���¸� ����ϴ� �Լ�
	// ���콺(Window, UI)
	// > �������� �� UI ������Ʈ�� ���� ���� �浹ó���� �Ǿ�� �Ѵ�.
	bool isUICollision = CollisionMouseWindow(_fTime);

	// ���콺(World, Object)
	// > UI �浹�� �Ǿ��ٸ� ������Ʈ �浹 ó���� �����ʴ´�.
	if (isUICollision == false)
	{
		// Mouse(World)
		CollisionMouseWorld(_fTime);
	}

	// ���콺�� ������� ������Ʈ �浹�� �������� ó���Ѵ�.
	CollisionObject(_fTime);
}

bool CCollisionManager::CollisionMouseWindow(float _fTime)
{
	// ������Ʈ �浹���� ���콺 �浹�� �켱�� �Ǿ�� �Ѵ�.
	// > UI ������Ʈ�� �浹
	CGameObject* pObjMouse		 = GET_SINGLE(CInput)->GetMouseObj();
	CColliderPoint*	pWindowPoint = pObjMouse->FindComponentFromTag<CColliderPoint>("MouseWindow");

	// ���� ������ ����ش�.
	// > �ڽ��� �浹ü�� ������ �ִ� ���� ����Ʈ�� �ʱ�ȭ ��Ų��.
	//   - ������Ʈ�� ��ġ�� ���� �� �ְ�, �׷��⿡ ������ �ٲ� �� �ֱ� �����̴�.
	pWindowPoint->ClearCollisionSection();

	// ���콺 ��ġ�� �����ϴ� UI�׷� ������ ���Ѵ�.
	// > UI Group Size (1280, 720)
	PCollisionGroup	pGroup = FindGroup("UI");
	Vector3	vMousePoint = pWindowPoint->GetInfo();
	int	iMouseSectionX = (int)vMousePoint.x / (int)pGroup->vCellLength.x;
	int	iMouseSectionY = (int)vMousePoint.y / (int)pGroup->vCellLength.y;

	bool isUICollision = false;
	if (iMouseSectionX >= 0 && iMouseSectionX < pGroup->iCountX &&
		iMouseSectionY >= 0 && iMouseSectionY < pGroup->iCountY)
	{
		// UIGroup�� ���� ����Ʈ[iCount]���� pWindowPoint �浹ü�� ������ �ִ� ������ ������ �´�.
		int	iPointSection = iMouseSectionY * pGroup->iCountX + iMouseSectionX;
		PCollisionSection pSection = &pGroup->pSectionList[iPointSection];

		// pWindowPoint �浹ü�� ������ �ִ� ���ǿ� ���� �ε����� �߰��Ѵ�.
		pWindowPoint->AddCollisionSection(iPointSection);

		// ������ �浹ü�� ����(iSize)��, �浹ü �ּҸ� ������ �� �ִ� CCollider** ����Ʈ�� ������ �ִ�.
		// �ش� ���ǿ� �浹ü�� �ִٸ�?
		if (pSection->iColliderCount > 0)
		{
			// UI ����
			// > (�̱���)

			// ���ǿ� ����ִ� ������Ʈ ����
			for (int i = 0; i < pSection->iColliderCount; ++i)
			{
				CCollider* pCollSrc = pSection->pColliderList[i];	// �浹ü ���
				CCollider* pCollDest = pWindowPoint;				// ���콺 �浹ü(Collider Point)

				// �浹 ���� �Ǵ�
				// �浹ü�� ȣ���ڷ� ���콺(Window)�� Dest�� �浹 �˻�.
				if (pCollSrc->Collision(pCollDest, _fTime) == true)
				{
					// Collision Enter(�浹ü�� ó�� �浹�� ���)
					if (pCollSrc->CheckPrevCollider(pCollDest) == false)
					{
						// ���� �浹 ��Ͽ� �߰�
						pCollSrc->AddPrevCollider(pCollDest);
						pCollDest->AddPrevCollider(pCollSrc);

						// OnCollisionEnter() ȣ��
						pCollSrc->OnCollisionEnter(pCollDest, _fTime);
						pCollDest->OnCollisionEnter(pCollSrc, _fTime);

						isUICollision = true;
					}
					// OnCollision(������ �浹�� �浹ü�� ��� �浹���� ����)
					else
					{
						// OnCollision() ȣ��
						pCollSrc->OnCollision(pCollDest, _fTime);
						pCollDest->OnCollision(pCollSrc, _fTime);

						isUICollision = true;
					}
				}
				else
				{
					// OnCollisionLeave(������ �浹���̴� �浹ü�� ������ ���)
					if (pCollSrc->CheckPrevCollider(pCollDest))
					{
						// ���� �浹 ��Ͽ��� ����
						pCollSrc->ErasePrevCollider(pCollDest);
						pCollDest->ErasePrevCollider(pCollSrc);

						// OnCollisionLeave() ȣ��
						pCollSrc->OnCollisionLeave(pCollDest, _fTime);
						pCollDest->OnCollisionLeave(pCollSrc, _fTime);
					}
				}
			}
		}
	}
	SAFE_RELEASE(pWindowPoint);

	return isUICollision;
}

void CCollisionManager::CollisionMouseWorld(float _fTime)
{
	// ���콺�� Window ColliderPoint ������Ʈ
	// > ������Ʈ�� �浹
	CGameObject* pObjMouse = GET_SINGLE(CInput)->GetMouseObj();
	CColliderPoint*	pWorldPoint = pObjMouse->FindComponentFromTag<CColliderPoint>("MouseWorld");

	// ���� ������ ����ش�.
	// > �ڽ��� �浹ü�� ������ �ִ� ���� ����Ʈ�� �ʱ�ȭ ��Ų��.
	//   - ������Ʈ�� ��ġ�� ���� �� �ְ�, �׷��⿡ ������ �ٲ� �� �ֱ� �����̴�.
	pWorldPoint->ClearCollisionSection();

	// �浹 �˻�
	bool isMouseCollision = false;
	unordered_map<string, PCollisionGroup>::iterator iterColGroup;
	unordered_map<string, PCollisionGroup>::iterator iterColGroupEnd = m_mapGroup.end();
	for (iterColGroup = m_mapGroup.begin(); iterColGroup != iterColGroupEnd; ++iterColGroup)
	{
		// �浹 �׷��� �����ͼ� UI �׷��̶�� continue�Ѵ�.
		if (iterColGroup->first == "UI") { continue; }

		// ���콺 ������ ���Ѵ�.
		Vector3	vMousePoint = pWorldPoint->GetInfo();
		int iMouseSectionX = (int)vMousePoint.x / (int)iterColGroup->second->vCellLength.x;
		int iMouseSectionY = (int)vMousePoint.y / (int)iterColGroup->second->vCellLength.y;

		if (iMouseSectionX >= 0 && iMouseSectionX < iterColGroup->second->iCountX &&
			iMouseSectionY >= 0 && iMouseSectionY < iterColGroup->second->iCountY)
		{
			int iPointSection = iMouseSectionY * iterColGroup->second->iCountX + iMouseSectionX;
			PCollisionSection pSection = &iterColGroup->second->pSectionList[iPointSection];
			pWorldPoint->AddCollisionSection(iPointSection);

			// ���ǿ� �����ϴ� �浹ü ������ŭ �ݺ��Ѵ�.
			for (int j = 0; j < pSection->iColliderCount; ++j)
			{
				// �浹ó���� �Ѵ�.
				// > �Ʒ� �ڵ�� �� CollisionMouseWindow() �Լ� �ּ� ����
				CCollider* pCollSrc = pSection->pColliderList[j];
				CCollider* pCollDest = pWorldPoint;

				if (isMouseCollision == true)
				{
					if (pCollSrc->CheckPrevCollider(pCollDest))
					{
						pCollSrc->ErasePrevCollider(pCollDest);
						pCollDest->ErasePrevCollider(pCollSrc);

						pCollSrc->OnCollisionLeave(pCollDest, _fTime);
						pCollDest->OnCollisionLeave(pCollSrc, _fTime);
					}
					continue;
				}
				if (pCollSrc->Collision(pCollDest, _fTime) == true)
				{
					// ó�� �浹
					if (pCollSrc->CheckPrevCollider(pCollDest) == false)
					{
						pCollSrc->AddPrevCollider(pCollDest);
						pCollDest->AddPrevCollider(pCollSrc);

						pCollSrc->OnCollisionEnter(pCollDest, _fTime);
						pCollDest->OnCollisionEnter(pCollSrc, _fTime);
						
						isMouseCollision = true;
					}
					// �浹 ��
					else
					{
						pCollSrc->OnCollision(pCollDest, _fTime);
						pCollDest->OnCollision(pCollSrc, _fTime);
						
						isMouseCollision = true;
					}
				}
				else
				{
					// �浹 ��
					if (pCollSrc->CheckPrevCollider(pCollDest) == true)
					{
						pCollSrc->ErasePrevCollider(pCollDest);
						pCollDest->ErasePrevCollider(pCollSrc);

						pCollSrc->OnCollisionLeave(pCollDest, _fTime);
						pCollDest->OnCollisionLeave(pCollSrc, _fTime);
					}
				}
			}
		}
	}
	SAFE_RELEASE(pWorldPoint);
}

void CCollisionManager::CollisionObject(float _fTime)
{
	unordered_map<string, PCollisionGroup>::iterator iterColGroup;
	unordered_map<string, PCollisionGroup>::iterator iterColGroupEnd = m_mapGroup.end();
	for (iterColGroup = m_mapGroup.begin(); iterColGroup != iterColGroupEnd; ++iterColGroup)
	{
		// �浹 �׷��� ���� ���� ��ŭ �ݺ��Ѵ�.
		for (int i = 0; i < iterColGroup->second->iCount; ++i)
		{
			PCollisionSection pSection = &iterColGroup->second->pSectionList[i];

			// ���ǿ� �浹ü�� �ϳ��� ���� �浹 �˻簡 �ʿ䰡 ����.
			// > �ش� �浹ü�� ������ �浹�ϰ� �ִ� �浹ü�� ����Ʈ���� ���� ��Ų��.
			if (pSection->iColliderCount < 2)
			{
				for (int j = 0; j < pSection->iColliderCount; ++j)
				{
					pSection->pColliderList[j]->CheckPrevColliderInSection(_fTime);
				}
				// ������ ��� �ִ� �浹ü ī��Ʈ 0���� ����
				// > Object�� Collision() �Լ��� ȣ�� �� �Ź� AddCollision�� �ϱ� ������ 0���� �ʱ�ȭ�Ѵ�.
				pSection->iColliderCount = 0;
				continue;
			}

			// ������ �浹�� ������Ʈ�� ���� ó�� 
			for (int j = 0; j < pSection->iColliderCount; ++j)
			{
				pSection->pColliderList[j]->CheckPrevColliderInSection(_fTime);
			}

			// �� ������ �浹ü ����ŭ �ݺ��Ѵ�.
			for (int j = 0; j < pSection->iColliderCount - 1; ++j)
			{
				for (int k = j + 1; k < pSection->iColliderCount; ++k)
				{
					CGameObject* pSrc = pSection->pColliderList[j]->GetGameObject();
					CGameObject* pDest = pSection->pColliderList[k]->GetGameObject();

					// ���� ������Ʈ�� ���� �˻����� �ʴ´�.
					if (pSrc == pDest)
					{
						SAFE_RELEASE(pSrc);
						SAFE_RELEASE(pDest);
						continue;
					}

					// �浹 ó��
					CCollider* pCollSrc = pSection->pColliderList[j];
					CCollider* pCollDest = pSection->pColliderList[k];
					if (pCollSrc->Collision(pCollDest, _fTime))
					{
						// ó�� �浹
						if (!pCollSrc->CheckPrevCollider(pCollDest))
						{
							pCollSrc->AddPrevCollider(pCollDest);
							pCollDest->AddPrevCollider(pCollSrc);

							pCollSrc->OnCollisionEnter(pCollDest, _fTime);
							pCollDest->OnCollisionEnter(pCollSrc, _fTime);
						}
						// �浹 ��
						else
						{
							pCollSrc->OnCollision(pCollDest, _fTime);
							pCollDest->OnCollision(pCollSrc, _fTime);
						}
					}
					else
					{
						// �浹 ��
						if (pCollSrc->CheckPrevCollider(pCollDest))
						{
							pCollSrc->ErasePrevCollider(pCollDest);
							pCollDest->ErasePrevCollider(pCollSrc);

							pCollSrc->OnCollisionLeave(pCollDest, _fTime);
							pCollDest->OnCollisionLeave(pCollSrc, _fTime);
						}
					}
					SAFE_RELEASE(pSrc);
					SAFE_RELEASE(pDest);
				}
			}
			// ������ ��� �ִ� �浹ü ī��Ʈ 0���� ����
			// > Object�� Collision() �Լ��� ȣ�� �� �Ź� AddCollision�� �ϱ� ������ 0���� �ʱ�ȭ�Ѵ�.
			pSection->iColliderCount = 0;
		}
	}
}