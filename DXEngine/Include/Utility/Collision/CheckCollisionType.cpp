#include "PCHEngine.h"
#include "CollisionManager.h"
#include "Utility/Input/Input.h"
#include "Object/GameObject.h"
#include "Component/ColliderPoint.h"

ENGINE_USING

void CCollisionManager::Collision(float _fTime)
{
	// 실제 충돌 상태를 계산하는 함수
	// 마우스(Window, UI)
	// > 게임진행 중 UI 오브젝트가 가장 먼저 충돌처리가 되어야 한다.
	bool isUICollision = CollisionMouseWindow(_fTime);

	// 마우스(World, Object)
	// > UI 충돌이 되었다면 오브젝트 충돌 처리는 하지않는다.
	if (isUICollision == false)
	{
		// Mouse(World)
		CollisionMouseWorld(_fTime);
	}

	// 마우스와 상관없이 오브젝트 충돌은 마지막이 처리한다.
	CollisionObject(_fTime);
}

bool CCollisionManager::CollisionMouseWindow(float _fTime)
{
	// 오브젝트 충돌보다 마우스 충돌이 우선시 되어야 한다.
	// > UI 오브젝트와 충돌
	CGameObject* pObjMouse		 = GET_SINGLE(CInput)->GetMouseObj();
	CColliderPoint*	pWindowPoint = pObjMouse->FindComponentFromTag<CColliderPoint>("MouseWindow");

	// 섹션 정보를 비워준다.
	// > 자신의 충돌체를 가지고 있는 섹션 리스트를 초기화 시킨다.
	//   - 오브젝트의 위치는 변할 수 있고, 그렇기에 섹션이 바뀔 수 있기 때문이다.
	pWindowPoint->ClearCollisionSection();

	// 마우스 위치가 존재하는 UI그룹 섹션을 구한다.
	// > UI Group Size (1280, 720)
	PCollisionGroup	pGroup = FindGroup("UI");
	Vector3	vMousePoint = pWindowPoint->GetInfo();
	int	iMouseSectionX = (int)vMousePoint.x / (int)pGroup->vCellLength.x;
	int	iMouseSectionY = (int)vMousePoint.y / (int)pGroup->vCellLength.y;

	bool isUICollision = false;
	if (iMouseSectionX >= 0 && iMouseSectionX < pGroup->iCountX &&
		iMouseSectionY >= 0 && iMouseSectionY < pGroup->iCountY)
	{
		// UIGroup의 섹션 리스트[iCount]에서 pWindowPoint 충돌체를 가지고 있는 섹션을 가지고 온다.
		int	iPointSection = iMouseSectionY * pGroup->iCountX + iMouseSectionX;
		PCollisionSection pSection = &pGroup->pSectionList[iPointSection];

		// pWindowPoint 충돌체를 가지고 있는 섹션에 섹션 인덱스를 추가한다.
		pWindowPoint->AddCollisionSection(iPointSection);

		// 섹션은 충돌체의 갯수(iSize)와, 충돌체 주소를 접근할 수 있는 CCollider** 리스트를 가지고 있다.
		// 해당 섹션에 충돌체가 있다면?
		if (pSection->iColliderCount > 0)
		{
			// UI 정렬
			// > (미구현)

			// 섹션에 담겨있는 오브젝트 갯수
			for (int i = 0; i < pSection->iColliderCount; ++i)
			{
				CCollider* pCollSrc = pSection->pColliderList[i];	// 충돌체 목록
				CCollider* pCollDest = pWindowPoint;				// 마우스 충돌체(Collider Point)

				// 충돌 여부 판단
				// 충돌체를 호출자로 마우스(Window)를 Dest로 충돌 검사.
				if (pCollSrc->Collision(pCollDest, _fTime) == true)
				{
					// Collision Enter(충돌체와 처음 충돌된 경우)
					if (pCollSrc->CheckPrevCollider(pCollDest) == false)
					{
						// 이전 충돌 목록에 추가
						pCollSrc->AddPrevCollider(pCollDest);
						pCollDest->AddPrevCollider(pCollSrc);

						// OnCollisionEnter() 호출
						pCollSrc->OnCollisionEnter(pCollDest, _fTime);
						pCollDest->OnCollisionEnter(pCollSrc, _fTime);

						isUICollision = true;
					}
					// OnCollision(이전에 충돌한 충돌체와 계속 충돌중인 상태)
					else
					{
						// OnCollision() 호출
						pCollSrc->OnCollision(pCollDest, _fTime);
						pCollDest->OnCollision(pCollSrc, _fTime);

						isUICollision = true;
					}
				}
				else
				{
					// OnCollisionLeave(이전에 충돌중이던 충돌체와 떨어진 경우)
					if (pCollSrc->CheckPrevCollider(pCollDest))
					{
						// 이전 충돌 목록에서 삭제
						pCollSrc->ErasePrevCollider(pCollDest);
						pCollDest->ErasePrevCollider(pCollSrc);

						// OnCollisionLeave() 호출
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
	// 마우스의 Window ColliderPoint 컴포넌트
	// > 오브젝트와 충돌
	CGameObject* pObjMouse = GET_SINGLE(CInput)->GetMouseObj();
	CColliderPoint*	pWorldPoint = pObjMouse->FindComponentFromTag<CColliderPoint>("MouseWorld");

	// 섹션 정보를 비워준다.
	// > 자신의 충돌체를 가지고 있는 섹션 리스트를 초기화 시킨다.
	//   - 오브젝트의 위치는 변할 수 있고, 그렇기에 섹션이 바뀔 수 있기 때문이다.
	pWorldPoint->ClearCollisionSection();

	// 충돌 검사
	bool isMouseCollision = false;
	unordered_map<string, PCollisionGroup>::iterator iterColGroup;
	unordered_map<string, PCollisionGroup>::iterator iterColGroupEnd = m_mapGroup.end();
	for (iterColGroup = m_mapGroup.begin(); iterColGroup != iterColGroupEnd; ++iterColGroup)
	{
		// 충돌 그룹을 가져와서 UI 그룹이라면 continue한다.
		if (iterColGroup->first == "UI") { continue; }

		// 마우스 섹션을 구한다.
		Vector3	vMousePoint = pWorldPoint->GetInfo();
		int iMouseSectionX = (int)vMousePoint.x / (int)iterColGroup->second->vCellLength.x;
		int iMouseSectionY = (int)vMousePoint.y / (int)iterColGroup->second->vCellLength.y;

		if (iMouseSectionX >= 0 && iMouseSectionX < iterColGroup->second->iCountX &&
			iMouseSectionY >= 0 && iMouseSectionY < iterColGroup->second->iCountY)
		{
			int iPointSection = iMouseSectionY * iterColGroup->second->iCountX + iMouseSectionX;
			PCollisionSection pSection = &iterColGroup->second->pSectionList[iPointSection];
			pWorldPoint->AddCollisionSection(iPointSection);

			// 섹션에 존재하는 충돌체 갯수만큼 반복한다.
			for (int j = 0; j < pSection->iColliderCount; ++j)
			{
				// 충돌처리를 한다.
				// > 아래 코드는 위 CollisionMouseWindow() 함수 주석 참조
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
					// 처음 충돌
					if (pCollSrc->CheckPrevCollider(pCollDest) == false)
					{
						pCollSrc->AddPrevCollider(pCollDest);
						pCollDest->AddPrevCollider(pCollSrc);

						pCollSrc->OnCollisionEnter(pCollDest, _fTime);
						pCollDest->OnCollisionEnter(pCollSrc, _fTime);
						
						isMouseCollision = true;
					}
					// 충돌 중
					else
					{
						pCollSrc->OnCollision(pCollDest, _fTime);
						pCollDest->OnCollision(pCollSrc, _fTime);
						
						isMouseCollision = true;
					}
				}
				else
				{
					// 충돌 끝
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
		// 충돌 그룹의 섹션 갯수 만큼 반복한다.
		for (int i = 0; i < iterColGroup->second->iCount; ++i)
		{
			PCollisionSection pSection = &iterColGroup->second->pSectionList[i];

			// 섹션에 충돌체가 하나인 경우는 충돌 검사가 필요가 없다.
			// > 해당 충돌체가 이전에 충돌하고 있던 충돌체를 리스트에서 삭제 시킨다.
			if (pSection->iColliderCount < 2)
			{
				for (int j = 0; j < pSection->iColliderCount; ++j)
				{
					pSection->pColliderList[j]->CheckPrevColliderInSection(_fTime);
				}
				// 섹션이 들고 있는 충돌체 카운트 0으로 설정
				// > Object의 Collision() 함수를 호출 시 매번 AddCollision을 하기 때문에 0으로 초기화한다.
				pSection->iColliderCount = 0;
				continue;
			}

			// 이전에 충돌된 오브젝트에 대한 처리 
			for (int j = 0; j < pSection->iColliderCount; ++j)
			{
				pSection->pColliderList[j]->CheckPrevColliderInSection(_fTime);
			}

			// 각 영역별 충돌체 수만큼 반복한다.
			for (int j = 0; j < pSection->iColliderCount - 1; ++j)
			{
				for (int k = j + 1; k < pSection->iColliderCount; ++k)
				{
					CGameObject* pSrc = pSection->pColliderList[j]->GetGameObject();
					CGameObject* pDest = pSection->pColliderList[k]->GetGameObject();

					// 같은 오브젝트의 경우는 검사하지 않는다.
					if (pSrc == pDest)
					{
						SAFE_RELEASE(pSrc);
						SAFE_RELEASE(pDest);
						continue;
					}

					// 충돌 처리
					CCollider* pCollSrc = pSection->pColliderList[j];
					CCollider* pCollDest = pSection->pColliderList[k];
					if (pCollSrc->Collision(pCollDest, _fTime))
					{
						// 처음 충돌
						if (!pCollSrc->CheckPrevCollider(pCollDest))
						{
							pCollSrc->AddPrevCollider(pCollDest);
							pCollDest->AddPrevCollider(pCollSrc);

							pCollSrc->OnCollisionEnter(pCollDest, _fTime);
							pCollDest->OnCollisionEnter(pCollSrc, _fTime);
						}
						// 충돌 중
						else
						{
							pCollSrc->OnCollision(pCollDest, _fTime);
							pCollDest->OnCollision(pCollSrc, _fTime);
						}
					}
					else
					{
						// 충돌 끝
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
			// 섹션이 들고 있는 충돌체 카운트 0으로 설정
			// > Object의 Collision() 함수를 호출 시 매번 AddCollision을 하기 때문에 0으로 초기화한다.
			pSection->iColliderCount = 0;
		}
	}
}