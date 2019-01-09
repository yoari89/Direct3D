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
	// �̹����� �ҷ��´�.
	const char*	pPath = GET_SINGLE(CPathManager)->FindPathFromMultibyte(_strPathKey);

	string strPath;
	if (pPath)
	{
		strPath = pPath;
	}
	strPath += _pFileName;

	// ���� �����
	FILE* pFile = nullptr;
	fopen_s(&pFile, strPath.c_str(), "rb");
	if (pFile == nullptr)
	{
		return;
	}

	// BITMAPFILEHEADER = ��Ʈ�� ���� ��ü�� ���� ������ ������ ����ü
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	fread(&fh, sizeof(fh), 1, pFile);
	fread(&ih, sizeof(ih), 1, pFile);

	int	iWidth  = ih.biWidth;
	int	iHeight = ih.biHeight;
	m_tRelativeInfo.tYasuoPixel = _tPixel;
	m_tRelativeInfo.pPixel = new Pixel24[iWidth * iHeight];
	fread(m_tRelativeInfo.pPixel, sizeof(Pixel24), iWidth * iHeight, pFile);
	
	// �������� �̹����� �ٽ� ������������ �����ش�.
	// > ���� ������ ��ǥ�谡 �ƴ� ���ϴ��� �����̱� ������ �ʿ� ���� �ڵ��.
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

	// �浹 �ڽ� �� ����
	m_tRelativeInfo.tBox.vMin		= _vMin;
	m_tRelativeInfo.tBox.vMax		= _vMin + Vector3(iWidth, iHeight, 0);
	m_tRelativeInfo.tBox.vLength	= m_tRelativeInfo.tBox.vMax - _vMin;
	m_tRelativeInfo.tBox.vMin.z		= 0.f;
	m_tRelativeInfo.tBox.vMax.z		= 0.f;
	m_tRelativeInfo.tBox.vLength.z	= 1.f;
	m_tRelativeInfo.iWidth			= iWidth;
	m_tRelativeInfo.iHeight			= iHeight;

	// ���� �浹 �� 
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

	// ���� �浹 ���� �� 
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
	matScale.Scaling(m_tInfo.tBox.vLength);	// ũ�� ���
	matPos.Translation(m_tInfo.tBox.vMin);	// �̵� ���

	// ��� ����
	TransformCBuffer tCBuffer = {};
	CCamera* pMainCamera = m_pScene->GetMainCamera();

	// �浹 �׷��� UI�� �ƴ϶�� ī�޶� ������ �޴´�.
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

	// �����
	tCBuffer.matWorld.Transpose();
	tCBuffer.matView.Transpose();
	tCBuffer.matProj.Transpose();
	tCBuffer.matWV.Transpose();
	tCBuffer.matWVP.Transpose();

	// "Transform" ��� ���� ������Ʈ
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Transform", &tCBuffer);

	// �޸� ����
	SAFE_RELEASE(pMainCamera);
	
	// �׸���
	CCollider::Render(fTime);
#endif // _DEBUG
}

CColliderPixel * CColliderPixel::Clone()
{
	return nullptr;
}
