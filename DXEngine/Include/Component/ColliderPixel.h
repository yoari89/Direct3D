#pragma once
#include "Collider.h"

ENGINE_BEGIN
class ENGINE_DLL CColliderPixel : public CCollider
{
private:
	friend class CGameObject;

protected:
	CColliderPixel();
	CColliderPixel(const CColliderPixel& _com);
	~CColliderPixel();

private:
	PixelInfo m_tRelativeInfo;
	PixelInfo m_tInfo;

public:
	void SetInfo(const Vector3 _vMin, const Pixel24& _tPixel, const char* _pFileName,
				 const string& _strPathKey = TEXTURE_PATH);
	PixelInfo GetInfo() const;

public:
	virtual bool Collision(CCollider* _pDest, float _fTime);

public:
	bool Init()						override;
	int  Input(float fTime)			override;
	int  Update(float fTime)		override;
	int  LateUpdate(float fTime)	override;
	void Collision(float fTime)		override;
	void Render(float fTime)		override;
	CColliderPixel* Clone()			override;
};
ENGINE_END