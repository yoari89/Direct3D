#include "PCHEngine.h"

#include "ColliderPixel.h"
#include "ColliderPoint.h"
#include "ColliderRect.h"
#include "ColliderOBB.h"
#include "Component/Camera.h"
#include "Component/Transform.h"
#include "Utility/Path/PathManager.h"
#include "Scene/Scene.h"
#include "Resources/Mesh.h"
#include "Resources/ResourcesManager.h"
#include "Rendering/RenderManager.h"
#include "Rendering/ShaderManager.h"

ENGINE_USING

CColliderPixel::CColliderPixel()
{
	m_eCollType = CT_PIXEL;
#ifdef _DEBUG
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh("ColliderRect");
	m_pDepthDisable = GET_SINGLE(CRenderManager)->FindRenderState(DEPTH_DISABLE);
#endif // _DEBUG
}

CColliderPixel::CColliderPixel(const CColliderPixel& _com)
{
	m_tRelativeInfo = _com.m_tRelativeInfo;

	m_tRelativeInfo.pPixel = new Pixel24[m_tRelativeInfo.iWidth * m_tRelativeInfo.iHeight];
	memcpy(m_tRelativeInfo.pPixel, _com.m_tRelativeInfo.pPixel,
		   sizeof(Pixel24) * m_tRelativeInfo.iWidth * m_tRelativeInfo.iHeight);
}

CColliderPixel::~CColliderPixel()
{
	SAFE_DELETE_ARRAY(m_tRelativeInfo.pPixel);
}

void CColliderPixel::SetInfo(const Vector3 _vMin, const Pixel24& _tPixel, 
							 const char* _pFileName, const string& _strPathKey)
{
	// 이미지를 불러온다.
	const char*	pPath = GET_SINGLE(CPathManager)->FindPathFromMultibyte(_strPathKey);

	string strPath;
	if (pPath)
	{
		strPath = pPath;
	}
	strPath += _pFileName;

	// 파일 입출력
	FILE* pFile = nullptr;
	fopen_s(&pFile, strPath.c_str(), "rb");
	if (pFile == nullptr)
	{
		return;
	}

	// BITMAPFILEHEADER = 비트맵 파일 자체에 관한 정보를 가지는 구조체
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	fread(&fh, sizeof(fh), 1, pFile);
	fread(&ih, sizeof(ih), 1, pFile);

	int	iWidth  = ih.biWidth;
	int	iHeight = ih.biHeight;
	m_tRelativeInfo.tYasuoPixel = _tPixel;
	m_tRelativeInfo.pPixel = new Pixel24[iWidth * iHeight];
	fread(m_tRelativeInfo.pPixel, sizeof(Pixel24), iWidth * iHeight, pFile);
	
	// 뒤집어진 이미지를 다시 원래방향으로 돌려준다.
	// > 기존 윈도우 좌표계가 아닌 좌하단을 기준이기 때문에 필요 없는 코드다.
	/*
	Pixel24* pLine = new Pixel24[iWidth];
	for (int i = 0; i < iHeight / 2; ++i)
	{
		memcpy(pLine, &m_tRelativeInfo.pPixel[i * iWidth], sizeof(Pixel24) * iWidth);
		memcpy(&m_tRelativeInfo.pPixel[i * iWidth], &m_tRelativeInfo.pPixel[(iHeight - i - 1) * iWidth], sizeof(Pixel24) * iWidth);
		memcpy(&m_tRelativeInfo.pPixel[(iHeight - i - 1) * iWidth], pLine, sizeof(Pixel24) * iWidth);
	}
	delete[]	pLine;
	*/

	fclose(pFile);

	// 충돌 박스 값 설정
	m_tRelativeInfo.tBox.vMin		= _vMin;
	m_tRelativeInfo.tBox.vMax		= _vMin + Vector3(iWidth, iHeight, 0);
	m_tRelativeInfo.tBox.vLength	= m_tRelativeInfo.tBox.vMax - _vMin;
	m_tRelativeInfo.tBox.vMin.z		= 0.f;
	m_tRelativeInfo.tBox.vMax.z		= 0.f;
	m_tRelativeInfo.tBox.vLength.z	= 1.f;
	m_tRelativeInfo.iWidth			= iWidth;
	m_tRelativeInfo.iHeight			= iHeight;

	// 실제 충돌 값 
	m_tInfo = m_tRelativeInfo;
}

PixelInfo CColliderPixel::GetInfo() const
{
	return m_tInfo;
}

bool CColliderPixel::Collision(CCollider* _pDest, float _fTime)
{
	switch (_pDest->GetColliderType())
	{
		// Pixel To Rect Collision
		case CT_RECT:
		{
			CColliderRect* pDest = (CColliderRect*)_pDest;
			return CollisionRectToPixel(pDest->GetInfo(), m_tInfo);
		}
	}

	return false;
}

bool CColliderPixel::Init()
{
	return true;
}

int CColliderPixel::Input(float fTime)
{
	return 0;
}

int CColliderPixel::Update(float fTime)
{
	return 0;
}

int CColliderPixel::LateUpdate(float fTime)
{
	Vector3	vPos			= m_pTransform->GetWorldPos() - m_pTransform->GetPivot() * m_pTransform->GetWorldScale();
	m_tInfo.tBox.vMin		= vPos + m_tRelativeInfo.tBox.vMin + m_vPivot * m_tRelativeInfo.tBox.vLength;
	m_tInfo.tBox.vMax		= vPos + m_tRelativeInfo.tBox.vMax + m_vPivot * m_tRelativeInfo.tBox.vLength;
	m_tInfo.tBox.vLength	= m_tRelativeInfo.tBox.vLength;

	// 실제 충돌 영역 값 
	m_vColliderSizeMin = m_tInfo.tBox.vMin;
	m_vColliderSizeMax = m_tInfo.tBox.vMax;

	return 0;
}

void CColliderPixel::Collision(float fTime)
{
}

void CColliderPixel::Render(float fTime)
{
#ifdef _DEBUG
	Matrix matScale, matPos, matView;
	matScale.Scaling(m_tInfo.tBox.vLength);	// 크기 행렬
	matPos.Translation(m_tInfo.tBox.vMin);	// 이동 행렬

	// 상수 버퍼
	TransformCBuffer tCBuffer = {};
	CCamera* pMainCamera = m_pScene->GetMainCamera();

	// 충돌 그룹이 UI가 아니라면 카메라 영향을 받는다.
	if (m_strCollisionGroup != "UI")
	{
		matView = pMainCamera->GetViewMatrix();
	}
	tCBuffer.matWorld	= matScale * matPos; 
	tCBuffer.matView	= matView;
	tCBuffer.matProj	= pMainCamera->GetProjMatrix();
	tCBuffer.matWV		= tCBuffer.matWorld * tCBuffer.matView;
	tCBuffer.matWVP		= tCBuffer.matWV * tCBuffer.matProj;
	tCBuffer.vPivot		= m_vPivot;
	tCBuffer.vLength	= m_pMesh->GetLength();

	// 역행렬
	tCBuffer.matWorld.Transpose();
	tCBuffer.matView.Transpose();
	tCBuffer.matProj.Transpose();
	tCBuffer.matWV.Transpose();
	tCBuffer.matWVP.Transpose();

	// "Transform" 상수 버퍼 업데이트
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Transform", &tCBuffer);

	// 메모리 해제
	SAFE_RELEASE(pMainCamera);
	
	// 그리기
	CCollider::Render(fTime);
#endif // _DEBUG
}

CColliderPixel * CColliderPixel::Clone()
{
	return nullptr;
}
