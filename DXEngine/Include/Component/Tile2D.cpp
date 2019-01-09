#include "PCHEngine.h"
#include "Tile2D.h"
#include "Resources/ResourcesManager.h"
#include "Resources/Mesh.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/Shader.h"
#include "Component/Camera.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"
#include "Device.h"

ENGINE_USING

CTile2D::CTile2D()
{
	SetTag("Tile2D");
	m_eTileOption = T2D_NORMAL;
	m_eTileType = S2D_RECT;
	m_eComType  = CT_TILE2D;

	m_bTileLine = true;
	m_pMesh = nullptr;
	m_pShader = nullptr;
}

CTile2D::CTile2D(const CTile2D& _tile2D) :
	CComponent(_tile2D)
{
	*this = _tile2D;
}

CTile2D::~CTile2D()
{
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pShader);
}

bool CTile2D::Init()
{
	return false;
}

int CTile2D::Input(float _fTime)
{
	return 0;
}

int CTile2D::Update(float _fTime)
{
	return 0;
}

int CTile2D::LateUpdate(float _fTime)
{
	return 0;
}

void CTile2D::Collision(float _fTime)
{
}

void CTile2D::Render(float _fTime)
{
	if (m_bTileLine)
	{
		TransformCBuffer	tCBuffer = {};

		CCamera*	pMainCamera = m_pScene->GetMainCamera();

		tCBuffer.matWorld = m_pTransform->GetWorldMatrix();
		tCBuffer.matView = pMainCamera->GetViewMatrix();
		tCBuffer.matProj = pMainCamera->GetProjMatrix();
		tCBuffer.matWV = tCBuffer.matWorld * tCBuffer.matView;
		tCBuffer.matWVP = tCBuffer.matWV * tCBuffer.matProj;
		tCBuffer.vPivot = Vector3::Zero;
		tCBuffer.vLength = m_pMesh->GetLength();

		tCBuffer.matWorld.Transpose();
		tCBuffer.matView.Transpose();
		tCBuffer.matProj.Transpose();
		tCBuffer.matWV.Transpose();
		tCBuffer.matWVP.Transpose();

		GET_SINGLE(CShaderManager)->UpdateCBuffer("Transform",
			&tCBuffer);

		SAFE_RELEASE(pMainCamera);

		Vector4	vColor;
		switch (m_eTileOption)
		{
		case T2D_NORMAL:
			vColor = Vector4::Green;
			break;
		case T2D_NOMOVE:
			vColor = Vector4::Red;
			break;
		}

		m_pShader->SetShader();
		CONTEXT->IASetInputLayout(m_pLayout);

		GET_SINGLE(CShaderManager)->UpdateCBuffer("Collider", &vColor);

		m_pMesh->Render();
	}
}

void CTile2D::AfterClone()
{
}

CTile2D* CTile2D::Clone()
{
	return new CTile2D(*this);
}

TILE2D_OPTION CTile2D::GetTile2DOption() const
{
	return m_eTileOption;
}

STAGE2D_TILE_TYPE CTile2D::GetTileType() const
{
	return m_eTileType;
}

void CTile2D::SetTileOption(TILE2D_OPTION eOption)
{
	m_eTileOption = eOption;
}

void CTile2D::SetTileType(STAGE2D_TILE_TYPE eType)
{
	m_eTileType = eType;

	switch (eType)
	{
	case S2D_RECT:
		m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh("ColliderRect");
		break;
	case S2D_ISOMETRIC:
		m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh("TileIso");
		break;
	}

	m_pShader = GET_SINGLE(CShaderManager)->FindShader(COLLIDER_SHADER);
	m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_LAYOUT);
}

void CTile2D::OnTile(bool bTile)
{
	m_bTileLine = bTile;
}
