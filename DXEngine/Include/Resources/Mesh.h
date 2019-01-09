#pragma once
#include "Vector2.h"
#include "../ReferenceCnt.h"

ENGINE_BEGIN

/* ���ؽ� ���� */
typedef struct ENGINE_DLL _tagVertexBuffer
{
public :
	ID3D11Buffer*	pBuffer; // ID3D11Buffer : ���� ������ �ʿ��� �������̽�
	void*			pData;	 // ��� �޸��� �ּҵ� ���� �� �ִ�.(�� ������ �Ұ���)
	int				iCount;	 // ���� ����
	int				iSize;	 // ������ ũ��
	D3D11_USAGE		eUsage;	 // ���� ��� ����

	_tagVertexBuffer() :
		pBuffer(nullptr),
		pData(nullptr),
		iCount(0),
		iSize(0),
		eUsage(D3D11_USAGE_DEFAULT)
	{
	}
}VertexBuffer, *PVertexBuffer;

/* �ε��� ���� */
typedef struct ENGINE_DLL _tagIndexBuffer
{
public:
	ID3D11Buffer*	pBuffer;	
	void*			pData;		
	int				iCount;		
	int				iSize;		
	D3D11_USAGE		eUsage;	
	DXGI_FORMAT		eFmt;	 // ���ҽ� ������ ����(�ȼ� ����)

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

/* �޽� �����̳� */
typedef struct ENGINE_DLL _tagMeshContainer
{
public:
	VertexBuffer			 tVB;		// Container 
	vector<IndexBuffer>		 vecIB;		// SubSet
	D3D11_PRIMITIVE_TOPOLOGY ePrimitive;
	// > D3D11_PRIMITIVE_TOPOLOGY : ���� ���� ���� 
	//                              (���� �����Ͱ� ȭ�鿡 ������ �Ǵ� ���)

}MeshContainer, *PMeshContainer;

// ============================================================================================= //

class ENGINE_DLL CMesh : public CReferenceCnt
{
private:
	friend class CResourcesManager;

private:
	CMesh();
	~CMesh();

/* �޽� */
private:
	vector<PMeshContainer> m_vecMeshContainer;

public:
	bool CreateMesh(const string& _strName, const string& _strShaderKey, const string& _strInputLayoutKey,
					void* _pVtx, int _iVtxCount, int _iVtxSize, D3D11_USAGE _eVtxUsage,
					D3D11_PRIMITIVE_TOPOLOGY _ePrimitive, void* _pIdx = nullptr, int _iIdxCount = 0,
					int _iIdxSize = 0, D3D11_USAGE _eInxUsage = D3D11_USAGE_DEFAULT,
					DXGI_FORMAT eFmt = DXGI_FORMAT_R16_UINT);

/* ���� */
private:
	bool CreateVertexBuffer(void* _pData, int _iCount, int _iSize, D3D11_USAGE _eUsage);
	bool CreateIndexBuffer(void* _pData, int _iCount, int _iSize, D3D11_USAGE _eUsage, DXGI_FORMAT _eFmt);

public:
	void UpdateVertexBuffer(void* _pData, int _iContainer = 0);

/* ������ */
// > ������ ���������� ������ Input Assembler
public:
	void Render();
	void Render(int _iContainer, int _iSubset);

/* ���̴�, �Է� ���̾ƿ� Ű */
// > �޽ø� �����ϱ� ���� ���� ���̴�, �Է� ���̾ƿ� Ű ��
// > ��, �޽ø� ������ �� ���� ������ �´� ���̴�, �Է� ���̾ƿ��� ������ �� ����Ѵ�.
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