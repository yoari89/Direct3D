#pragma once
#include "Component/Component.h"

ENGINE_BEGIN

enum TILE2D_OPTION
{
	T2D_NORMAL,
	T2D_NOMOVE
};

class CMesh;
class CShader;
class ENGINE_DLL CTile2D : public CComponent
{
public:
	CTile2D();
	CTile2D(const CTile2D& _tile2D);
	~CTile2D();

/* 컴포넌트 클래스 가상함수 재정의 */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CTile2D* Clone()				override;

private:
	bool				m_bTileLine;
	TILE2D_OPTION		m_eTileOption;
	STAGE2D_TILE_TYPE	m_eTileType;
	CShader*			m_pShader;
	CMesh*				m_pMesh;
	ID3D11InputLayout*	m_pLayout;

public:
	TILE2D_OPTION GetTile2DOption()	const;
	STAGE2D_TILE_TYPE GetTileType()	const;

public:
	void SetTileOption(TILE2D_OPTION eOption);
	void SetTileType(STAGE2D_TILE_TYPE eType);
	void OnTile(bool bTile);
};
ENGINE_END