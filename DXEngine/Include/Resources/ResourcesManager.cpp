#include "PCHEngine.h"
#include "ResourcesManager.h"
#include "Mesh.h"
#include "Texture.h"
#include "Sampler.h"
#include "Utility/Types.h"

ENGINE_USING
DEFINITION_SINGLE(CResourcesManager)

CResourcesManager::CResourcesManager()
{
}

CResourcesManager::~CResourcesManager()
{
	// �޸� ����
	Safe_Release_Map<unordered_map<string, CMesh*>>(m_mapMesh);
	Safe_Release_Map<unordered_map<string, CTexture*>>(m_mapTexture);
	Safe_Release_Map(m_mapSampler);
}

bool CResourcesManager::Init()
{
	// �⺻ �޽� ����
	CreateDefaultTriMesh();
	CreateDefaultRectMesh();
	CreateDefaultTexMesh();

#ifdef _DEBUG
	// ����׿뵵 �޽� ����
	CreateColliderMesh();
#endif // _DEBUG

	// �⺻ ���÷�(Linear, �������͸�) ����
	CreateSampler(SAMPLER_LINEAR);
	
	return true;
}

bool CResourcesManager::CreateDefaultTriMesh()
{
	// ���ؽ�
	VertexColor	tColorTriangle[3] =
	{
		VertexColor(Vector3( 0.f,   0.5f, 0.f),	Vector4::Red),
		VertexColor(Vector3( 0.5f, -0.5f, 0.f),	Vector4::Green),
		VertexColor(Vector3(-0.5f, -0.5f, 0.f), Vector4::Blue)
	};

	// �ε���
	// > �ð� ����   : ȭ�鿡 ������ �� �޽�
	// > �ݽð� ���� : Back FaceCulling(�ĸ� �߸���) ȭ�鿵���� �׷����� �ʴ� �޽�
	unsigned short sIdx[3] = { 0, 1, 2 };

	// �⺻ �ﰢ�� �޽� ����
	return CreateMesh("ColorTri", STANDARD_COLOR_SHADER, POS_COLOR_LAYOUT, tColorTriangle, 3, sizeof(VertexColor),
					   D3D11_USAGE_DEFAULT, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, sIdx, 3, sizeof(unsigned short));
}

bool CResourcesManager::CreateDefaultRectMesh()
{
	// ���ؽ�
	VertexColor	tColorRect[4] =
	{
		VertexColor(Vector3(-0.5f,  0.5f, 0.f),	Vector4::White),
		VertexColor(Vector3(0.5f,  0.5f, 0.f),	Vector4::White),
		VertexColor(Vector3(-0.5f, -0.5f, 0.f), Vector4::White),
		VertexColor(Vector3(0.5f, -0.5f, 0.f),  Vector4::White)
	};

	// �ε���
	unsigned short sRectIdx[6] = { 0, 1, 2, 1, 3, 2 };

	// �⺻ �簢�� �޽� ����
	return CreateMesh("ColorRect", STANDARD_COLOR_SHADER, POS_COLOR_LAYOUT, tColorRect, 4, sizeof(VertexColor),
					   D3D11_USAGE_DEFAULT, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, sRectIdx, 6, sizeof(unsigned short));
}

bool CResourcesManager::CreateDefaultTexMesh()
{
	// ���ؽ�
	VertexTex tTexRect[4] =
	{
		VertexTex(Vector3(0.f, 1.f, 0.f), Vector2(0.f, 0.f)),
		VertexTex(Vector3(1.f, 1.f, 0.f), Vector2(1.f, 0.f)),
		VertexTex(Vector3(0.f, 0.f, 0.f), Vector2(0.f, 1.f)),
		VertexTex(Vector3(1.f, 0.f, 0.f), Vector2(1.f, 1.f))
	};

	// �ε���
	unsigned short sRectIdx[6] = { 0, 1, 3, 0, 3, 2 };
	
	// �⺻ �ؽ�ó �޽� ����
	return CreateMesh("TexRect", STANDARD_TEX_SHADER, POS_UV_LAYOUT, tTexRect, 4, sizeof(VertexTex), 
				 	   D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, sRectIdx, 6, sizeof(unsigned short));
}

bool CResourcesManager::CreateColliderMesh()
{
	// Rect
	// ���ؽ�
	Vector3	vPos[5] =
	{
		Vector3(0.f, 1.f, 0.f),
		Vector3(1.f, 1.f, 0.f),
		Vector3(1.f, 0.f, 0.f),
		Vector3(0.f, 0.f, 0.f),
		Vector3(0.f, 1.f, 0.f)
	};

	// LineStrip
	// > �ε��� ���� ���� ���ؽ� ������� ������ �׷��ش�.
	CreateMesh("ColliderRect", COLLIDER_SHADER, POS_LAYOUT, vPos, 5, sizeof(Vector3), 
				D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// Circle
	float x, y = 0.f;
	Vector3 vPosCircle[361];
	for (int i = 0; i < 360; ++i)
	{
		x = cosf(DegreeToRadian((float)i));
		y = sinf(DegreeToRadian((float)i));
		vPosCircle[i] = Vector3(x, y, 0.f);
	}
	vPosCircle[360] = Vector3(1.f, 0.f, 0.f);

	CreateMesh("ColliderCircle", COLLIDER_SHADER, POS_LAYOUT, vPosCircle, 361, sizeof(Vector3),
				D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	
	return true;
}

bool CResourcesManager::CreateMesh(const string& _strName, const string& _strShaderKey,
								   const string& _strLayoutKey, void* _pVtx, int _iVtxCount, int _iVtxSize, 
								   D3D11_USAGE _eVtxUsage, D3D11_PRIMITIVE_TOPOLOGY _ePrimitive, 
								   void* _pIdx, int _iIdxCount, int _iIdxSize, D3D11_USAGE _eIdxUsage, DXGI_FORMAT _eFmt)
{
	// ã��
	CMesh* pMesh = FindMesh(_strName);
	if (pMesh != nullptr)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}
	
	// ����
	pMesh = new CMesh();
	if (pMesh->CreateMesh(_strName, _strShaderKey, _strLayoutKey, _pVtx, _iVtxCount, _iVtxSize,
			      		  _eVtxUsage, _ePrimitive, _pIdx, _iIdxCount, _iIdxSize, _eIdxUsage, _eFmt) == false)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}
	
	// �߰�
	m_mapMesh.insert(make_pair(_strName, pMesh));
	return true;
}

CMesh* CResourcesManager::FindMesh(const string& _strName)
{
	unordered_map<string, CMesh*>::iterator	iter = m_mapMesh.find(_strName);
	if (iter == m_mapMesh.end())
	{
		return nullptr;
	}
	iter->second->AddRef();
	return iter->second;
}

bool CResourcesManager::CreateTexture(const string& _strName, const TCHAR* _pFileName, 
								      const string& _strPathKey)
{
	// �̹� �����ϴ� �ؽ�ó 
	CTexture* pTexture = FindTexture(_strName);
	if (pTexture != nullptr)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	// �ε�
	pTexture = new CTexture();
	if (pTexture->LoadTexture(_strName, _pFileName, _strPathKey) == false)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	// �߰�
	m_mapTexture.insert(make_pair(_strName, pTexture));
	return true;
}

bool CResourcesManager::CreateTexture(const char* _strName, const TCHAR * _pFileName, const string & _strPathKey)
{
	int a = 0;

	CTexture* pTexture = FindTexture(_strName);
	if (pTexture != nullptr)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	// �ε�
	pTexture = new CTexture();
	if (pTexture->LoadTexture(_strName, _pFileName, _strPathKey) == false)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	// �߰�
	m_mapTexture.insert(make_pair(_strName, pTexture));

	return true;
}

bool CResourcesManager::CreateTextureFromFullPath(const string& _strName, const TCHAR* _pFullPath)
{
	CTexture* pTexture = FindTexture(_strName);
	if (pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	// �ε�
	pTexture = new CTexture();
	if (pTexture->LoadTextureFromFullPath(_strName, _pFullPath) == false)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	// �߰�
	m_mapTexture.insert(make_pair(_strName, pTexture));
	return true;
}

bool CResourcesManager::CreateTexture(const string & strName, const vector<const TCHAR*>& vecFileName, const string & strPathKey)
{
	CTexture*	pTexture = FindTexture(strName);

	if (pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture = new CTexture;

	if (!pTexture->LoadTexture(strName, vecFileName, strPathKey))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	m_mapTexture.insert(make_pair(strName, pTexture));

	return true;
}

bool CResourcesManager::CreateTextureFromFullPath(const string & strName, const vector<const TCHAR*>& vecFullPath)
{
	CTexture*	pTexture = FindTexture(strName);

	if (pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture = new CTexture;

	if (!pTexture->LoadTextureFromFullPath(strName, vecFullPath))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	m_mapTexture.insert(make_pair(strName, pTexture));

	return true;
}

CTexture* CResourcesManager::FindTexture(const string& _strName)
{
	// ã��
	unordered_map<string, CTexture*>::iterator iter = m_mapTexture.find(_strName);
	if (iter == m_mapTexture.end())
	{
		return nullptr;
	}
	iter->second->AddRef();
	return iter->second;
}

bool CResourcesManager::CreateSampler(const string & _strName, D3D11_FILTER _eFilter, 
									  D3D11_TEXTURE_ADDRESS_MODE _eU, 
									  D3D11_TEXTURE_ADDRESS_MODE _eV, 
									  D3D11_TEXTURE_ADDRESS_MODE _eW)
{
	CSampler* pSampler = FindSampler(_strName);
	if (pSampler)
	{
		SAFE_RELEASE(pSampler);
		return false;
	}

	// ���÷� ����
	pSampler = new CSampler;
	if (pSampler->CreateSampler(_strName, _eFilter, _eU, _eV, _eW) == false)
	{
		SAFE_RELEASE(pSampler);
		return false;
	}

	// �߰�
	m_mapSampler.insert(make_pair(_strName, pSampler));
	return true;
}

CSampler* CResourcesManager::FindSampler(const string& _strName)
{
	unordered_map<string, CSampler*>::iterator iter = m_mapSampler.find(_strName);

	if (iter == m_mapSampler.end())
	{
		return nullptr;
	}
	iter->second->AddRef();
	return iter->second;
}