#pragma once
#include "Component/Component.h"
#include "Utility/EnumFlag.h"
#include "Utility/Types.h"

ENGINE_BEGIN
class CMesh;
class CShader;
class CMaterial;
class CGameObject;
class CRenderState;
class ENGINE_DLL CRenderer : public CComponent
{
private:
	friend CGameObject;

private:
	CRenderer();
	CRenderer(const CRenderer& _renderer);
	~CRenderer();

/* 렌더링에 필요한 자원 */
private:
	CMesh*				m_pMesh;
	CShader*			m_pShader;
	CMaterial*			m_pMaterial;
	CRenderState*		m_pRenderState[RS_END];
	ID3D11InputLayout*	m_pLayout;
	ComponentCBuffer	m_tComponentCBuffer;
	unordered_map<string, PRendererCBuffer> m_mapCBuffer;

/* Mesh */
public:
	void SetMesh(CMesh* _pMesh);
	void SetMesh(const string& _strKey);

/* Shader */
public:
	void SetShader(CShader* _pshader);
	void SetShader(const string& _strKey);

/* Render*/
public:
	void SetRenderState(const string& _strKey);

/* Render State */
public:
	bool CreateRendererCBuffer(const string& _strName, int _iSize);
	void UpdateRendererCBuffer(const string& _strName, void* _pData, int _iSize);

private:
	PRendererCBuffer FindRendererCBuffer(const string& _strName);

/* Input Layout */
public:
	void SetInputLayout(const string& _strKey);

/* Component ConstantBuffer */
public:
	void CheckComponent();
	void DeleteComponentCBuffer(CComponent* pComponent);
	void UpdateAnimationType(int iType);

/* 공간 변환 */
private:
	void UpdateTransform();

/* 상속 받은 재정의 함수 */
public:
	bool Init()						override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CRenderer* Clone()				override;
};
ENGINE_END