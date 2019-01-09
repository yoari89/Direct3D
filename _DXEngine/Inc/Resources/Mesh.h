#pragma once
#include "Vector2.h"
#include "../ReferenceCnt.h"

ENGINE_BEGIN

/* 버텍스 버퍼 */
typedef struct ENGINE_DLL _tagVertexBuffer
{
public :
	ID3D11Buffer*	pBuffer; // ID3D11Buffer : 버퍼 생성에 필요한 인터페이스
	void*			pData;	 // 어떠한 메모리의 주소도 받을 수 있다.(역 참조는 불가능)
	int				iCount;	 // 정점 갯수
	int				iSize;	 // 정점당 크기
	D3D11_USAGE		eUsage;	 // 버퍼 사용 목적

	_tagVertexBuffer() :
		pBuffer(nullptr),
		pData(nullptr),
		iCount(0),
		iSize(0),
		eUsage(D3D11_USAGE_DEFAULT)
	{
	}
}VertexBuffer, *PVertexBuffer;

/* 인덱스 버퍼 */
typedef struct ENGINE_DLL _tagIndexBuffer
{
public:
	ID3D11Buffer*	pBuffer;	
	void*			pData;		
	int				iCount;		
	int				iSize;		
	D3D11_USAGE		eUsage;	
	DXGI_FORMAT		eFmt;	 // 리소스 데이터 형식(픽셀 형식)

	_tagIndexBuffer() :
		pBuffer(nullptr),
		pData(nullptr),
		iCount(0),
		iSize(0),
		eUsage(D3D11_USAGE_DEFAULT),
		eFmt(DXGI_FORMAT_R16_UINT)	
	{
	}
}IndexBuffer, *PIndexBuffer;

/* 메시 컨테이너 */
typedef struct ENGINE_DLL _tagMeshContainer
{
public:
	VertexBuffer			 tVB;		// Container 
	vector<IndexBuffer>		 vecIB;		// SubSet
	D3D11_PRIMITIVE_TOPOLOGY ePrimitive;
	// > D3D11_PRIMITIVE_TOPOLOGY : 도형 위상 구조 
	//                              (정점 데이터가 화면에 렌더링 되는 방법)

}MeshContainer, *PMeshContainer;

// ============================================================================================= //

class ENGINE_DLL CMesh : public CReferenceCnt
{
private:
	friend class CResourcesManager;

private:
	CMesh();
	~CMesh();

/* 메시 */
private:
	vector<PMeshContainer> m_vecMeshContainer;

public:
	bool CreateMesh(const string& _strName, const string& _strShaderKey, const string& _strInputLayoutKey,
					void* _pVtx, int _iVtxCount, int _iVtxSize, D3D11_USAGE _eVtxUsage,
					D3D11_PRIMITIVE_TOPOLOGY _ePrimitive, void* _pIdx = nullptr, int _iIdxCount = 0,
					int _iIdxSize = 0, D3D11_USAGE _eInxUsage = D3D11_USAGE_DEFAULT,
					DXGI_FORMAT eFmt = DXGI_FORMAT_R16_UINT);

/* 버퍼 */
private:
	bool CreateVertexBuffer(void* _pData, int _iCount, int _iSize, D3D11_USAGE _eUsage);
	bool CreateIndexBuffer(void* _pData, int _iCount, int _iSize, D3D11_USAGE _eUsage, DXGI_FORMAT _eFmt);

public:
	void UpdateVertexBuffer(void* _pData, int _iContainer = 0);

/* 렌더링 */
// > 렌더링 파이프라인 시작점 Input Assembler
public:
	void Render();
	void Render(int _iContainer, int _iSubset);

/* 셰이더, 입력 레이아웃 키 */
// > 메시를 생성하기 위해 사용될 셰이더, 입력 레이아웃 키 값
// > 후, 메시를 변경할 때 버퍼 정보에 맞는 셰이더, 입력 레이아웃을 변경할 때 사용한다.
private:
	string m_strShaderKey;
	string m_inputLayoutKey;

public:
	string GetShaderKey()	    const;
	string GetInputLayoutKey()  const;

/* Rect */
private:
	Vector3	m_vMin;
	Vector3	m_vMax;
	Vector3	m_vCenter;
	Vector3	m_vLength;
	float	m_fRadius;

public:
	Vector3 GetCenter() const;
	Vector3 GetMin()	const;
	Vector3 GetMax()	const;
	Vector3 GetLength() const;
	float	GetRadius()	const;

/* Container, Subset */
public:
	size_t GetContainerCount()			   const;
	size_t GetSubsetCount(int _iContainer) const;
};
ENGINE_END