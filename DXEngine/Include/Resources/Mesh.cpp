#include "PCHEngine.h"
#include "Mesh.h"
#include "../Device.h"
#include "Utility/Thread/ThreadManager.h"

ENGINE_USING
CMesh::CMesh()
{
}

CMesh::~CMesh()
{
	// 메모리 해제
	for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
		// vector<IndexBuffer>
		for (size_t j = 0; j < m_vecMeshContainer[i]->vecIB.size(); ++j)
		{
			SAFE_RELEASE(m_vecMeshContainer[i]->vecIB[j].pBuffer);
			SAFE_DELETE_ARRAY(m_vecMeshContainer[i]->vecIB[j].pData);
		}
		
		// VertexBuffer
		SAFE_RELEASE(m_vecMeshContainer[i]->tVB.pBuffer);			
		SAFE_DELETE_ARRAY(m_vecMeshContainer[i]->tVB.pData);
		
		// MeshContainer
		SAFE_DELETE(m_vecMeshContainer[i]);								
	}
	m_vecMeshContainer.clear();
}

bool CMesh::CreateMesh(const string& _strName, const string& _strShaderKey, const string& _strInputLayoutKey,
					   void* _pVtx, int _iVtxCount, int _iVtxSize, D3D11_USAGE _eVtxUsage,
					   D3D11_PRIMITIVE_TOPOLOGY _ePrimitive, void* _pIdx, int _iIdxCount, int _iIdxSize,
					   D3D11_USAGE _eInxUsage, DXGI_FORMAT _eFmt)
{
	SetTag(_strName);
	m_strShaderKey	 = _strShaderKey;
	m_inputLayoutKey = _strInputLayoutKey;

	// MeshContainer 생성
	PMeshContainer pContainer = new MeshContainer;
	pContainer->ePrimitive	  = _ePrimitive;
	m_vecMeshContainer.push_back(pContainer);

	// 버텍스 버퍼 생성
	if (CreateVertexBuffer(_pVtx, _iVtxCount, _iVtxSize, _eVtxUsage) == false)
	{
		tassert(true);
		return false;
	}

	// 인덱스 버퍼 생성
	// > Collider Mesh 경우 인덱스 버퍼 데이터가 없다.
	if (_pIdx != nullptr)
	{
		// Debug Collider Mesh
		if (CreateIndexBuffer(_pIdx, _iIdxCount, _iIdxSize, _eInxUsage, _eFmt) == false)
		{
			tassert(true);
			return false;
		}
	}

	return true;
}

bool CMesh::CreateVertexBuffer(void* _pData, int _iCount, int _iSize, D3D11_USAGE _eUsage)
{
	PMeshContainer pContainer = m_vecMeshContainer[m_vecMeshContainer.size() - 1];

	// 버텍스 버퍼 데이터 설정
	// > memcpy(복사 받을 메모리공간, 복사할 메모리공간, 메모리 공간 크기)
	pContainer->tVB.pData = new char[_iCount * _iSize];
	memcpy(pContainer->tVB.pData, _pData, _iCount * _iSize);
	pContainer->tVB.iCount = _iCount;
	pContainer->tVB.iSize  = _iSize;
	pContainer->tVB.eUsage = _eUsage;

	// 버퍼를 생성 조건
	// > 버퍼의 정보를 입력할 수 있는 D3D11_BUFFER_DESC 구조체 데이터가 있어야한다.
	D3D11_BUFFER_DESC tDesc = {};
	tDesc.ByteWidth = _iCount * _iSize;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 버퍼를 버텍스 버퍼에 결합 시킨다.
	tDesc.Usage		= _eUsage;
	if (_eUsage == D3D11_USAGE_DYNAMIC)
	{
		// D3D11_USAGE_DYNAMIC
		// > GPU(읽기 전용)와 CPU(쓰기 전용)가 모두 액세스 할 수있는 리소스.
		// > CPUAccessFlags : CPU 접근 방법
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	// D3D11_SUBRESOURCE_DATA : 버퍼로 복사될 실제데이터
	// > pSysMem : 초기화 데이터의 포인터. (VertexBuffer 데이터 초기화)
	D3D11_SUBRESOURCE_DATA tResource = {};
	tResource.pSysMem = pContainer->tVB.pData;

	// 버퍼 생성
	if (FAILED(DEVICE->CreateBuffer(&tDesc, &tResource, &pContainer->tVB.pBuffer)))
	{
		return false;
	}

	// 정점 Rect 정보
	char*	pVertices = (char*)_pData;
	Vector3	vPos;
	memcpy(&vPos, pVertices, sizeof(Vector3));

	m_vMin = vPos;
	m_vMax = vPos;
	for (int i = 1; i < _iCount; ++i)
	{
		// 정점 포인터 연산
		memcpy(&vPos, pVertices + _iSize * i, sizeof(Vector3));

		if (m_vMin.x > vPos.x) { m_vMin.x = vPos.x; } // MinX
		if (m_vMin.y > vPos.y) { m_vMin.y = vPos.y; } // MinY
		if (m_vMin.z > vPos.z) { m_vMin.z = vPos.z; } // MinZ
		if (m_vMax.x < vPos.x) { m_vMax.x = vPos.x; } // MaxX
		if (m_vMax.y < vPos.y) { m_vMax.y = vPos.y; } // MaxY
		if (m_vMax.z < vPos.z) { m_vMax.z = vPos.z; } // MaxZ
	}
	m_vCenter = (m_vMin + m_vMax) / 2.f;
	m_vLength = m_vMax - m_vMin;
	m_fRadius = m_vLength.Length() / 2.f;

	return true;
}

bool CMesh::CreateIndexBuffer(void* _pData, int _iCount, int _iSize, D3D11_USAGE _eUsage, DXGI_FORMAT _eFmt)
{
	PMeshContainer pContainer = m_vecMeshContainer[m_vecMeshContainer.size() - 1];

	// IndexBuffer를 생성
	// > pContainer 멤버 vector<IndexBuffer>에 추가 
	IndexBuffer tIB;
	tIB.pData	= new char[_iCount * _iSize];
	memcpy(tIB.pData, _pData, _iCount * _iSize);
	tIB.iCount	= _iCount;
	tIB.iSize	= _iSize;
	tIB.eUsage	= _eUsage;

	// 버퍼 구조체 생성
	D3D11_BUFFER_DESC tDesc = {};
	tDesc.ByteWidth = _iCount * _iSize;
	tDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tDesc.Usage		= _eUsage;
	if (_eUsage == D3D11_USAGE_DYNAMIC)
	{
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	// 인덱스 버퍼 데이터
	D3D11_SUBRESOURCE_DATA tResource = {};
	tResource.pSysMem = tIB.pData;

	// 버퍼 생성
	if (FAILED(DEVICE->CreateBuffer(&tDesc, &tResource, &tIB.pBuffer)))
	{
		return false;
	}

	// 추가
	pContainer->vecIB.push_back(tIB);

	return true;
}

void CMesh::UpdateVertexBuffer(void* _pData, int _iContainer)
{
	if (_iContainer < 0 || _iContainer >= (int)m_vecMeshContainer.size())
	{
		tassert(true);
	}
	// 정점 정보 갱신
	PMeshContainer pContainer = m_vecMeshContainer[_iContainer];
	memcpy(pContainer->tVB.pData, _pData, pContainer->tVB.iSize * pContainer->tVB.iCount);
	switch (pContainer->tVB.eUsage)
	{
		case D3D11_USAGE_DEFAULT:
		{
			CONTEXT->UpdateSubresource(pContainer->tVB.pBuffer, 0, nullptr, _pData, 0, 0);
			break;
		}
		case D3D11_USAGE_DYNAMIC:
		{
			CRITICAL_SECTION*	pCrt = GET_SINGLE(CThreadManager)->GetLoadSafe();
			if (pCrt)
				EnterCriticalSection(pCrt);

			D3D11_MAPPED_SUBRESOURCE tMap;
			CONTEXT->Map(pContainer->tVB.pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);
			memcpy(tMap.pData, _pData, pContainer->tVB.iSize * pContainer->tVB.iCount);
			CONTEXT->Unmap(pContainer->tVB.pBuffer, 0);
			
			if (pCrt)
				LeaveCriticalSection(pCrt);

			break;
		}
	}
}

void CMesh::Render()
{
	// 모든 메시 렌더링
	for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
		// 정점 데이터가 화면에 렌더링 되는 방법을 지정한다.
		CONTEXT->IASetPrimitiveTopology(m_vecMeshContainer[i]->ePrimitive);

		// 버텍스 버퍼를 IA(입력 조립기)단계에 묶는다.
		UINT iOffset = 0;
		UINT iStride = m_vecMeshContainer[i]->tVB.iSize;
		CONTEXT->IASetVertexBuffers(0, 1, &m_vecMeshContainer[i]->tVB.pBuffer, &iStride, &iOffset);

		// 인덱스 버퍼를 이용한 렌더링 
		if (m_vecMeshContainer[i]->vecIB.empty() == false)
		{
			for (size_t j = 0; j < m_vecMeshContainer[i]->vecIB.size(); ++j)
			{
				// 인덱스 버퍼를 IA(입력 조립기) 단계에 결합
				CONTEXT->IASetIndexBuffer(m_vecMeshContainer[i]->vecIB[j].pBuffer,
										  m_vecMeshContainer[i]->vecIB[j].eFmt, 0);

				// 렌더링 파이프라인 시작
				CONTEXT->DrawIndexed(m_vecMeshContainer[i]->vecIB[j].iCount, 0, 0);
			}
		}
		// 정점 버퍼만을 이용한 렌더링
		else
		{
			//  렌더링 파이프라인 시작
			CONTEXT->Draw(m_vecMeshContainer[i]->tVB.iCount, 0);
		}
	}
}

void CMesh::Render(int _iContainer, int _iSubset)
{
	// 특정 메시만 렌더링
	// > 코드 설명은 Render() 함수 참조
	CONTEXT->IASetPrimitiveTopology(m_vecMeshContainer[_iContainer]->ePrimitive);
	UINT iOffset = 0;
	UINT iStride = m_vecMeshContainer[_iContainer]->tVB.iSize;
	CONTEXT->IASetVertexBuffers(0, 1, &m_vecMeshContainer[_iContainer]->tVB.pBuffer, &iStride, &iOffset);

	// 인덱스 버퍼를 이용한 렌더링 
	if (m_vecMeshContainer[_iContainer]->vecIB.empty() == false)
	{
		CONTEXT->IASetIndexBuffer(m_vecMeshContainer[_iContainer]->vecIB[_iSubset].pBuffer,
								  m_vecMeshContainer[_iContainer]->vecIB[_iSubset].eFmt, 0);
		CONTEXT->DrawIndexed(m_vecMeshContainer[_iContainer]->vecIB[_iSubset].iCount, 0, 0);
	}
	// 정점 버퍼만을 이용한 렌더링
	else
	{
		CONTEXT->Draw(m_vecMeshContainer[_iContainer]->tVB.iCount, 0);
	}
}

string CMesh::GetShaderKey() const
{
	return m_strShaderKey;
}

string CMesh::GetInputLayoutKey() const
{
	return m_inputLayoutKey;
}

Vector3 CMesh::GetCenter() const
{
	return m_vCenter;
}

Vector3 CMesh::GetMin() const
{
	return m_vMin;
}

Vector3 CMesh::GetMax() const
{
	return m_vMax;
}

Vector3 CMesh::GetLength() const
{
	return m_vLength;
}

float CMesh::GetRadius() const
{
	return m_fRadius;
}

size_t CMesh::GetContainerCount() const
{
	return m_vecMeshContainer.size();
}

size_t CMesh::GetSubsetCount(int _iContainer) const
{
	return m_vecMeshContainer[_iContainer]->vecIB.size();
}