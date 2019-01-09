#include "PCHEngine.h"
#include "Device.h"
#include "Stage2D.h"
#include "Tile2D.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"
#include "Component/Renderer.h"
#include "Component/Material.h"
#include "Component/Camera.h"
#include "Scene/Scene.h"

ENGINE_USING

CStage2D::CStage2D()
{
	SetTag("Stage2D");
	m_eComType  = CT_STAGE2D;
	m_eTileType = S2D_NONE;
	m_bTileLineOn = true;
}

CStage2D::CStage2D(const CStage2D& _stage2D) :
	CComponent(_stage2D)
{
	*this = _stage2D;
}

CStage2D::~CStage2D()
{
	Safe_Release_VecList(m_vecTileTr);
	Safe_Release_VecList(m_vecTile);
	Safe_Release_VecList(m_vecTileObj);
}

bool CStage2D::Init()
{
	return true;
}

int CStage2D::Input(float _fTime)
{
	return 0;
}

int CStage2D::Update(float _fTime)
{
	CTransform*	pMainCameraTr = m_pScene->GetMainCameraTransform();

	Vector3	vCameraPos = pMainCameraTr->GetWorldPos();

	SAFE_RELEASE(pMainCameraTr);

	vCameraPos -= m_vStartPos;

	Vector3	vEndPos = vCameraPos;
	vEndPos.x += _RESOLUTION.iWidth;
	vEndPos.y += _RESOLUTION.iHeight;

	vCameraPos /= m_vTileScale;
	vEndPos /= m_vTileScale;

	m_iStartX = (int)vCameraPos.x;
	if (m_iStartX < 0)
		m_iStartX = 0;

	else if (m_iStartX >= (int)m_iNumX)
		m_iStartX = m_iNumX - 1;

	m_iStartY = (int)vCameraPos.y;
	if (m_iStartY < 0)
		m_iStartY = 0;

	else if (m_iStartY >= (int)m_iNumY)
		m_iStartY = m_iNumY - 1;

	m_iEndX = (int)vEndPos.x + 1;
	if (m_iEndX < 0)
		m_iEndX = 0;

	else if (m_iEndX >= (int)m_iNumX)
		m_iEndX = m_iNumX - 1;

	m_iEndY = (int)vEndPos.y * 2 + 1;
	if (m_iEndY < 0)
		m_iEndY = 0;

	else if (m_iEndY >= (int)m_iNumY)
		m_iEndY = m_iNumY - 1;

	for (int i = m_iStartY; i < m_iEndY; ++i)
	{
		for (int j = m_iStartX; j < m_iEndX; ++j)
		{
			m_vecTileObj[i * m_iNumX + j]->Update(_fTime);
		}
	}

	return 0;
}

int CStage2D::LateUpdate(float _fTime)
{
	for (int i = m_iStartY; i < m_iEndY; ++i)
	{
		for (int j = m_iStartX; j < m_iEndX; ++j)
		{
			m_vecTileObj[i * m_iNumX + j]->LateUpdate(_fTime);
		}
	}

	return 0;
}

void CStage2D::Collision(float _fTime)
{
	for (int i = m_iStartY; i < m_iEndY; ++i)
	{
		for (int j = m_iStartX; j < m_iEndX; ++j)
		{
			m_vecTileObj[i * m_iNumX + j]->Collision(_fTime);
		}
	}
}

void CStage2D::Render(float _fTime)
{
	for (int i = m_iStartY; i < m_iEndY; ++i)
	{
		for (int j = m_iStartX; j < m_iEndX; ++j)
		{
			m_vecTileObj[i * m_iNumX + j]->Render(_fTime);
		}
	}
}

void CStage2D::AfterClone()
{
}

CStage2D* CStage2D::Clone()
{
	return new CStage2D(*this);
}

void CStage2D::CreateTile(UINT iNumX, UINT iNumY, const Vector3 & vStart, 
						  const Vector3& vTileScale, STAGE2D_TILE_TYPE eType,
						  const string& strKey, const TCHAR* pFileName,
						  const string& strPathKey)
{
	Safe_Release_VecList(m_vecTileTr);
	Safe_Release_VecList(m_vecTile);
	Safe_Release_VecList(m_vecTileObj);

	m_vecTileObj.resize(iNumX * iNumY);
	m_vecTile.resize(iNumX * iNumY);
	m_vecTileTr.resize(iNumX * iNumY);

	Vector3	vScale;
	vScale.x = vTileScale.x * iNumX;
	vScale.y = vScale.y = vTileScale.y * 2 + (iNumY - 2) * (vTileScale.y / 2.f);
	vScale.z = 1.f;

	m_vTileScale = vTileScale;
	m_vStartPos = vStart;
	m_pTransform->SetWorldScale(vScale);
	m_pTransform->SetWorldPos(vStart);

	m_eTileType = eType;
	m_iNumX = iNumX;
	m_iNumY = iNumY;

	switch (eType)
	{
		case S2D_RECT:
		{
			CreateRect(vStart, vTileScale, strKey, pFileName, strPathKey);
			break;
		}
		case S2D_ISOMETRIC:
		{
			CreateIsometric(vStart, vTileScale, strKey, pFileName, strPathKey);
			break;
		}
	}
}

void CStage2D::CreateRect(const Vector3& vStart, const Vector3& vTileScale,
						  const string& strKey, const TCHAR* pFileName,
						  const string& strPathKey)
{
	for (UINT i = 0; i < m_iNumY; ++i)
	{
		for (UINT j = 0; j < m_iNumX; ++j)
		{
			UINT iIndex = i * m_iNumX + j;
			CGameObject* pTileObj = CGameObject::CreateObject("TileObj");
			pTileObj->SetScene(m_pScene);
			pTileObj->SetLayer(m_pLayer);

			CTile2D* pTile = pTileObj->AddComponent<CTile2D>("Tile");
			pTile->SetTileType(S2D_RECT);
			pTile->OnTile(m_bTileLineOn);

			CTransform*	pTransform = pTileObj->GetTransform();
			pTransform->SetWorldScale(vTileScale);

			Vector3	vPos = vStart + vTileScale * Vector3((float)j, (float)i, 1.f);
			pTransform->SetWorldPos(vPos);

			if (pFileName)
			{
				CRenderer*	pRenderer = pTileObj->AddComponent<CRenderer>("Renderer");
				pRenderer->SetMesh("TexRect");
				pRenderer->SetRenderState(ALPHA_BLEND);
				SAFE_RELEASE(pRenderer);

				CMaterial*	pMaterial = pTileObj->AddComponent<CMaterial>("Material");
				pMaterial->SetDiffuseTex(0, strKey, pFileName, strPathKey);
				SAFE_RELEASE(pMaterial);
			}

			m_vecTileObj[iIndex] = pTileObj;
			m_vecTileTr[iIndex] = pTransform;
			m_vecTile[iIndex] = pTile;
		}
	}
}

void CStage2D::CreateIsometric(const Vector3 & vStart, const Vector3 & vTileScale,
							   const string& strKey, const TCHAR* pFileName,
							   const string& strPathKey)
{
	for (UINT i = 0; i < m_iNumY; ++i)
	{
		for (UINT j = 0; j < m_iNumX; ++j)
		{
			UINT	iIndex = i * m_iNumX + j;

			CGameObject*	pTileObj = CGameObject::CreateObject("TileObj");

			pTileObj->SetScene(m_pScene);
			pTileObj->SetLayer(m_pLayer);

			CTile2D*	pTile = pTileObj->AddComponent<CTile2D>("Tile");

			pTile->SetTileType(S2D_ISOMETRIC);

			CTransform*	pTransform = pTileObj->GetTransform();

			pTransform->SetWorldScale(vTileScale);

			Vector3	vPos;

			float	fOffset = 0.f;

			if (i % 2 == 1)
				fOffset = vTileScale.x / 2.f;

			vPos.x = vStart.x + fOffset + j * vTileScale.x;
			vPos.y = vStart.y + i * vTileScale.y / 2.f;

			pTransform->SetWorldPos(vPos);

			if (pFileName)
			{
				CRenderer*	pRenderer = pTileObj->AddComponent<CRenderer>("Renderer");

				pRenderer->SetMesh("TexRect");
				pRenderer->SetRenderState(ALPHA_BLEND);

				SAFE_RELEASE(pRenderer);

				CMaterial*	pMaterial = pTileObj->AddComponent<CMaterial>("Material");

				pMaterial->SetDiffuseTex(0, strKey, pFileName, strPathKey);

				SAFE_RELEASE(pMaterial);
			}

			m_vecTileObj[iIndex] = pTileObj;
			m_vecTileTr[iIndex] = pTransform;
			m_vecTile[iIndex] = pTile;
		}
	}
}

void CStage2D::SetTileType(STAGE2D_TILE_TYPE _eType)
{
	m_eTileType = _eType;
}

void CStage2D::SetTileOption(const Vector3& _vPos, TILE2D_OPTION _eOption)
{
	int	iIndex = GetTileIndex(_vPos);

	if (iIndex == -1)
		return;

	m_vecTile[iIndex]->SetTileOption(_eOption);
}

void CStage2D::OnTile(bool bTile)
{
	m_bTileLineOn = bTile;

	for (size_t i = 0; i < m_vecTile.size(); ++i)
	{
		m_vecTile[i]->OnTile(bTile);
	}
}

STAGE2D_TILE_TYPE CStage2D::GetTileType() const
{
	return m_eTileType;
}

int CStage2D::GetTileIndex(const Vector3 & vPos) const
{
	switch (m_eTileType)
	{
	case S2D_RECT:
		return GetRectTileIndex(vPos);
	case S2D_ISOMETRIC:
		return GetIsometricTileIndex(vPos);
	}

	return -1;
}

int CStage2D::GetRectTileIndex(const Vector3& vPos) const
{
	int	x = (int)(vPos.x - m_vStartPos.x) / (int)m_vTileScale.x;
	if (x < 0 || x >= (int)m_iNumX)
	{
		return -1;
	}

	int	y = (int)(vPos.y - m_vStartPos.y) / (int)m_vTileScale.y;
	if (y < 0 || y >= (int)m_iNumY)
	{
		return -1;
	}

	return y * m_iNumX + x;
}

int CStage2D::GetIsometricTileIndex(const Vector3 & vPos) const
{
	int	idxX = GetIsometricTileIndexX(vPos);
	if (idxX == -1)
		return -1;

	int	idxY = GetIsometricTileIndexY(vPos);
	if (idxY == -1)
		return -1;

	return idxY * m_iNumX + idxX;
}

int CStage2D::GetIsometricTileIndexX(const Vector3 & vPos) const
{
	if (m_iNumX == 0 || m_iNumY == 0 || m_vTileScale.x == 0.f ||
		m_vTileScale.y == 0.f)
		return -1;

	int	iY = GetIsometricTileIndexY(vPos);

	if (iY < 0 || iY >= (int)m_iNumY)
		return -1;

	float	x = vPos.x - m_vStartPos.x;
	float	y = vPos.y - m_vStartPos.y;
	int	idx = -1;

	if (iY % 2 == 0)
		idx = (int)x / (int)m_vTileScale.x;

	else
		idx = ((int)x - (int)m_vTileScale.x / 2) / (int)m_vTileScale.x;

	if (idx < 0 || idx >= (int)m_iNumX)
		return -1;

	return idx;
}

int CStage2D::GetIsometricTileIndexY(const Vector3 & vPos) const
{
	if (m_iNumX == 0 || m_iNumY == 0 || m_vTileScale.x == 0.f ||
		m_vTileScale.y == 0.f)
		return -1;

	float	x = vPos.x - m_vStartPos.x;
	float	y = vPos.y - m_vStartPos.y;

	// x, y 위치를 타일의 가로, 세로 크기로 나누어서 1, 1 공간으로
	// 변환시켜준다.
	float	fX = x / m_vTileScale.x;
	float	fY = y / m_vTileScale.y;

	// 사각 타일을 기준으로 이 위치가 어느정도쯤에 있는지 비율을
	// 구해준다.
	fX -= (int)fX;
	fY -= (int)fY;

	int	iXIdx = (int)x / (int)m_vTileScale.x;
	int	iYIdx = (int)y / (int)m_vTileScale.y;

	// 현재 Y의 비율이 0.5 보다 작다면 사각형의 아래쪽 부분에
	// 존재한다는 것이다.
	if (fY < 0.5f)
	{
		// 현재 비율을 이용해서 현재 점이 왼쪽 하단 마름모 밖의
		// 삼각형 안일 경우를 구해준다.
		if (fY < 0.5f - fX)
		{
			if (iYIdx == 0)
				return -1;

			else if (iYIdx == 1)
			{
				if (iXIdx == 0)
					return -1;

				else
					return 1;
			}

			else
			{
				if (iXIdx == 0)
					return -1;

				else
					return (iYIdx - 1) * 3 - (iYIdx - 2);
			}
		}

		// 우하단 일경우
		else if (fY < fX - 0.5f)
		{
			if (iYIdx == 0)
				return -1;

			else if (iYIdx == 1)
			{
				if (iXIdx >(int)m_iNumX - 1)
					return -1;

				return 1;
			}

			else
			{
				if (iXIdx > (int)m_iNumX - 1)
					return -1;

				return (iYIdx - 1) * 3 - (iYIdx - 2);
			}
		}

		else
		{
			if (iYIdx == 0)
				return 0;

			else if (iYIdx == 1)
				return 2;

			else
				return (iYIdx - 1) * 3 - (iYIdx - 2) + 1;
		}
	}

	// 위쪽 라인일 경우
	else if (fY > 0.5f)
	{
		// 좌표지점이 좌 상단 지점일 경우
		if (fY - 0.5f > fX)
		{
			if (iYIdx == 0)
			{
				if (iXIdx == 0)
					return -1;

				else
					return 1;
			}

			else if (iYIdx == 1)
			{
				if (iXIdx == 0)
					return -1;

				else
					return 3;
			}

			else
			{
				if (iXIdx == 0)
					return -1;

				return (iYIdx - 1) * 3 - (iYIdx - 2) + 2;
			}
		}

		// 좌표지점이 우 상단 지점일 경우
		else if (fY - 0.5f > 1.f - fX)
		{
			if (iYIdx == 0)
			{
				if (iXIdx > (int)m_iNumX - 1)
					return -1;

				else
					return 1;
			}

			else if (iYIdx == 1)
			{
				if (iXIdx > (int)m_iNumX - 1)
					return -1;

				else
					return 3;
			}

			else
			{
				if (iXIdx > (int)m_iNumX - 1)
					return -1;

				return (iYIdx - 1) * 3 - (iYIdx - 2) + 2;
			}
		}

		else
		{
			if (iYIdx == 0)
				return 0;

			else if (iYIdx == 1)
				return 2;

			else
				return (iYIdx - 1) * 3 - (iYIdx - 2) + 1;
		}
	}

	else
	{
		if (iYIdx == 0)
			return 0;

		else if (iYIdx == 1)
			return 2;

		else
			return (iYIdx - 1) * 3 - (iYIdx - 2) + 1;
	}

	return -1;
}
