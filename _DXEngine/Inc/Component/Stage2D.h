#pragma once
#include "Component/Component.h"

ENGINE_BEGIN
class CTile2D;
class CTrnasform;
class ENGINE_DLL CStage2D : public CComponent
{
private:
	friend class CGameObject;

public:
	CStage2D();
	CStage2D(const CStage2D& _stage2D);
	~CStage2D();

/* 컴포넌트 클래스 가상함수 재정의 */
public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CStage2D* Clone()				override;

private:
	vector<CGameObject*>	m_vecTileObj;
	vector<CTransform*>		m_vecTileTr;
	vector<CTile2D*>		m_vecTile;
	UINT					m_iNumX;
	UINT					m_iNumY;
	Vector3					m_vTileScale;
	Vector3					m_vStartPos;
	STAGE2D_TILE_TYPE		m_eTileType;
	bool					m_bTileLineOn;
	int						m_iStartX;
	int						m_iStartY;
	int						m_iEndX;
	int						m_iEndY;

/* Create Tile */
public:
	void CreateTile(UINT iNumX, UINT iNumY, const Vector3& vStart,
					const Vector3& vTileScale, STAGE2D_TILE_TYPE eType,
					const string& strKey = "", const TCHAR* pFileName = nullptr,
					const string& strPathKey = TEXTURE_PATH);
	void CreateRect(const Vector3& vStart, const Vector3& vTileScale,
					const string& strKey = "", const TCHAR* pFileName = nullptr,
					const string& strPathKey = TEXTURE_PATH);
	void CreateIsometric(const Vector3& vStart, const Vector3& vTileScale,
						 const string& strKey = "", const TCHAR* pFileName = nullptr,
						 const string& strPathKey = TEXTURE_PATH);

	// > Tile Information
	void SetTileType(STAGE2D_TILE_TYPE _eType);
	void SetTileOption(const Vector3& _vPos, enum TILE2D_OPTION _eOption);
	void OnTile(bool bTile);
	STAGE2D_TILE_TYPE GetTileType()	const;
	int GetTileIndex(const Vector3& vPos)	const;

private:
	int GetRectTileIndex(const Vector3& vPos)	const;
	int GetIsometricTileIndex(const Vector3& vPos)	const;
	int GetIsometricTileIndexX(const Vector3& vPos)	const;
	int GetIsometricTileIndexY(const Vector3& vPos)	const;
};
ENGINE_END