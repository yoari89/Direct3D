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
	// �޸� ����
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

	// MeshContainer ����
	PMeshContainer pContainer = new MeshContainer;
	pContainer->ePrimitive	  = _ePrimitive;
	m_vecMeshContainer.push_back(pContainer);

	// ���ؽ� ���� ����
	if (CreateVertexBuffer(_pVtx, _iVtxCount, _iVtxSize, _eVtxUsage) == false)
	{
		tassert(true);
		return false;
	}

	// �ε��� ���� ����
	// > Collider Mesh ��� �ε��� ���� �����Ͱ� ����.
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

	// ���ؽ� ���� ������ ����
	// > memcpy(���� ���� �޸𸮰���, ������ �޸𸮰���, �޸� ���� ũ��)
	pContainer->tVB.pData = new char[_iCount * _iSize];
	memcpy(pContainer->tVB.pData, _pData, _iCount * _iSize);
	pContainer->tVB.iCount = _iCount;
	pContainer->tVB.iSize  = _iSize;
	pContainer->tVB.eUsage = _eUsage;

	// ���۸� ���� ����
	// > ������ ������ �Է��� �� �ִ� D3D11_BUFFER_DESC ����ü �����Ͱ� �־���Ѵ�.
	D3D11_BUFFER_DESC tDesc = {};
	tDesc.ByteWidth = _iCount * _iSize;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���۸� ���ؽ� ���ۿ� ���� ��Ų��.
	tDesc.Usage		= _eUsage;
	if (_eUsage == D3D11_USAGE_DYNAMIC)
	{
		// D3D11_USAGE_DYNAMIC
		// > GPU(�б� ����)�� CPU(���� ����)�� ��� �׼��� �� ���ִ� ���ҽ�.
		// > CPUAccessFlags : CPU ���� ���
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	// D3D11_SUBRESOURCE_DATA : ���۷� ����� ����������
	// > pSysMem : �ʱ�ȭ �������� ������. (VertexBuffer ������ �ʱ�ȭ)
	D3D11_SUBRESOURCE_DATA tResource = {};
	tResource.pSysMem = pContainer->tVB.pData;

	// ���� ����
	if (FAILED(DEVICE->CreateBuffer(&tDesc, &tResource, &pContainer->tVB.pBuffer)))
	{
		return false;
	}

	// ���� Rect ����
	char*	pVertices = (char*)_pData;
	Vector3	vPos;
	memcpy(&vPos, pVertices, sizeof(Vector3));

	m_vMin = vPos;
	m_vMax = vPos;
	for (int i = 1; i < _iCount; ++i)
	{
		// ���� ������ ����
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

	// IndexBuffer�� ����
	// > pContainer ��� vector<IndexBuffer>�� �߰� 
	IndexBuffer tIB;
	tIB.pData	= new char[_iCount * _iSize];
	memcpy(tIB.pData, _pData, _iCount * _iSize);
	tIB.iCount	= _iCount;
	tIB.iSize	= _iSize;
	tIB.eUsage	= _eUsage;

	// ���� ����ü ����
	D3D11_BUFFER_DESC tDesc = {};
	tDesc.ByteWidth = _iCount * _iSize;
	tDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tDesc.Usage		= _eUsage;
	if (_eUsage == D3D11_USAGE_DYNAMIC)
	{
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	// �ε��� ���� ������
	D3D11_SUBRESOURCE_DATA tResource = {};
	tResource.pSysMem = tIB.pData;

	// ���� ����
	if (FAILED(DEVICE->CreateBuffer(&tDesc, &tResource, &tIB.pBuffer)))
	{
		return false;
	}

	// �߰�
	pContainer->vecIB.push_back(tIB);

	return true;
}

void CMesh::UpdateVertexBuffer(void* _pData, int _iContainer)
{
	if (_iContainer < 0 || _iContainer >= (int)m_vecMeshContainer.size())
	{
		tassert(true);
	}
	// ���� ���� ����
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
	// ��� �޽� ������
	for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
		// ���� �����Ͱ� ȭ�鿡 ������ �Ǵ� ����� �����Ѵ�.
		CONTEXT->IASetPrimitiveTopology(m_vecMeshContainer[i]->ePrimitive);

		// ���ؽ� ���۸� IA(�Է� ������)�ܰ迡 ���´�.
		UINT iOffset = 0;
		UINT iStride = m_vecMeshContainer[i]->tVB.iSize;
		CONTEXT->IASetVertexBuffers(0, 1, &m_vecMeshContainer[i]->tVB.pBuffer, &iStride, &iOffset);

		// �ε��� ���۸� �̿��� ������ 
		if (m_vecMeshContainer[i]->vecIB.empty() == false)
		{
			for (size_t j = 0; j < m_vecMeshContainer[i]->vecIB.size(); ++j)
			{
				// �ε��� ���۸� IA(�Է� ������) �ܰ迡 ����
				CONTEXT->IASetIndexBuffer(m_vecMeshContainer[i]->vecIB[j].pBuffer,
										  m_vecMeshContainer[i]->vecIB[j].eFmt, 0);

				// ������ ���������� ����
				CONTEXT->DrawIndexed(m_vecMeshContainer[i]->vecIB[j].iCount, 0, 0);
			}
		}
		// ���� ���۸��� �̿��� ������
		else
		{
			//  ������ ���������� ����
			CONTEXT->Draw(m_vecMeshContainer[i]->tVB.iCount, 0);
		}
	}
}

void CMesh::Render(int _iContainer, int _iSubset)
{
	// Ư�� �޽ø� ������
	// > �ڵ� ������ Render() �Լ� ����
	CONTEXT->IASetPrimitiveTopology(m_vecMeshContainer[_iContainer]->ePrimitive);
	UINT iOffset = 0;
	UINT iStride = m_vecMeshContainer[_iContainer]->tVB.iSize;
	CONTEXT->IASetVertexBuffers(0, 1, &m_vecMeshContainer[_iContainer]->tVB.pBuffer, &iStride, &iOffset);

	// �ε��� ���۸� �̿��� ������ 
	if (m_vecMeshContainer[_iContainer]->vecIB.empty() == false)
	{
		CONTEXT->IASetIndexBuffer(m_vecMeshContainer[_iContainer]->vecIB[_iSubset].pBuffer,
								  m_vecMeshContainer[_iContainer]->vecIB[_iSubset].eFmt, 0);
		CONTEXT->DrawIndexed(m_vecMeshContainer[_iContainer]->vecIB[_iSubset].iCount, 0, 0);
	}
	// ���� ���۸��� �̿��� ������
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