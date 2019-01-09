#include "PCHEngine.h"
#include "Input.h"
#include "Core.h"
#include "Device.h"
#include "Object/GameObject.h"
#include "Utility/Collision/CollisionManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Resources/Texture.h"
#include "Resources/ResourcesManager.h"
#include "Component/Transform.h"
#include "Component/Renderer.h"
#include "Component/Material.h"
#include "Component/Animation2D.h"
#include "Component/ColliderPoint.h"

ENGINE_USING
DEFINITION_SINGLE(CInput)

CInput::CInput() :
	m_bShowCursor(false),
	m_pCreateKey(nullptr),
	m_pMouseTr(nullptr),
	m_pMouse(nullptr)
{
}

CInput::~CInput()
{
	// �޸� ����
	SAFE_RELEASE(m_pMouse);
	SAFE_RELEASE(m_pMouseTr);
	SAFE_RELEASE(m_pWorldPoint);
	Safe_Delete_Map<unordered_map<string, PKeyInfo>>(m_mapKey);
}

PKeyInfo CInput::FindKey(const string& strKey)
{
	unordered_map<string, PKeyInfo>::iterator iter = m_mapKey.find(strKey);
	if (iter == m_mapKey.end())
	{
		return nullptr;
	}
	return iter->second;
}

void CInput::ChangeMouseScene(CScene* pScene)
{
	m_pMouse->SetScene(pScene);
}

void CInput::AddMouseWorldCollision()
{
	// �� ���� ī�޶� ���� ��ġ�� ����� ��ǥ�� ����
	// > UI �浹 �׷�� ī�޶� �ƴϴ�.
	CScene* pScene = GET_SINGLE(CSceneManager)->GetScene();
	CTransform* pCameraTr = pScene->GetMainCameraTransform();
	m_pWorldPoint->SetInfo(pCameraTr->GetWorldPos());
	SAFE_RELEASE(pScene);
	SAFE_RELEASE(pCameraTr);

	m_pMouse->LateUpdate(0.f);
}

Vector2 CInput::GetMouseGap() const
{
	return m_vMouseGap;
}

Vector2 CInput::GetMouseClient() const
{
	return m_vMouseClient;
}

Vector2 CInput::GetMouseWorld() const
{
	return m_vMouseWorld;
}

CGameObject* CInput::GetMouseObj() const
{
	return m_pMouse;
}

unordered_map<string, PKeyInfo>* CInput::GetKeyList()
{
	return &m_mapKey;
}

bool CInput::Init()
{
	// Ű ���
	// ex) ��� ��
	// AddKey("MoveUp", 'W')	: "MoveUp" �̸����� 'w' ���� ����Ѵ�.
	// AddKey(VK_SPACE, "Fire") : "Fire" �̸����� VK_SPACE ���� ����Ѵ�.
	// AddKey(VK_CONTROL, '1', "Skill1") : "Skill1"  �̸����� Control + 1 ���� Ű�� ����Ѵ�."

	// ����Ű
	AddKey(VK_UP,    "Up");
	AddKey(VK_DOWN,  "Down");
	AddKey(VK_LEFT,  "Left");
	AddKey(VK_RIGHT, "Right");

	// ����Ű, ����Ű
	AddKey(VK_CONTROL, "Ctrl");
	AddKey(VK_MENU,    "Alt");

	// �� ���� ���
	AddKey('Q',		 "Q");
	AddKey('W',		 "W");
	AddKey('E',		 "E");
	AddKey('R',		 "R");
	AddKey('1',		 "1");
	AddKey('2',		 "2");
	AddKey('3',		 "3");
	AddKey('4',		 "4");
	AddKey('I',		 "I");
	AddKey('A',		 "A");
	AddKey('S',		 "S");
	AddKey('D',		 "D");
	AddKey('F',		 "F");
	AddKey(VK_F1,	 "F1");
	AddKey(VK_F2,	 "F2");
	AddKey(VK_F3,	 "F3");
	AddKey(VK_F4,	 "F4");
	AddKey(VK_SHIFT, "Shift");

	// NPC ��ȭ
	AddKey(VK_SPACE, "KeyClick");

	// ���콺 ��ư Ŭ��
	AddKey(VK_LBUTTON, "LButton");
	AddKey(VK_RBUTTON, "RButton");

	// ���콺 ������Ʈ ����
	m_pMouse   = CGameObject::CreateObject("Mouse");
	m_pMouseTr = m_pMouse->GetTransform();

	// Render ������Ʈ ���
	// > "CompRenderer"
	CRenderer* pRenderer = m_pMouse->AddComponent<CRenderer>("CompRenderer");
	pRenderer->SetMesh("TexRect");
	pRenderer->SetRenderState(ALPHA_BLEND);
	SAFE_RELEASE(pRenderer);

	// Material ������Ʈ ã��
	// > �ؽ�ó ���
	CMaterial* pMaterial = m_pMouse->FindComponentFromType<CMaterial>(CT_MATERIAL);
	pMaterial->SetDiffuseTex(0, "Mouse", TEXT("UI/Cursor.png"));
	SAFE_RELEASE(pMaterial);

	// �浹ü �߰�
	// > Window �浹ü(UI), World �浹ü(������Ʈ) �߰�
	// > Default CollisionGroup = "Default"
	CColliderPoint* pPoint = m_pMouse->AddComponent<CColliderPoint>("MouseWindow");
	pPoint->SetCollisionGroup("UI");
	SAFE_RELEASE(pPoint);
	m_pWorldPoint = m_pMouse->AddComponent<CColliderPoint>("MouseWorld");

	// Transform ����
	CTexture* pTexture = GET_SINGLE(CResourcesManager)->FindTexture("Mouse");
	int texWidth = pTexture->GetWidth();
	int texHeight = pTexture->GetHeight();
	SAFE_RELEASE(pTexture);
	m_pMouseTr->SetWorldScale(Vector3(texWidth, texHeight, 1));
	m_pMouseTr->SetWorldPivot(Vector3(0.f, 1.f, 0.f)); // Left Top

	// ���콺 Ŀ���� ��� ���� �ʴ´�.
	ShowCursor(FALSE);

	return true;
}

int CInput::Input(float _fTime)
{
	KeyboardInput(_fTime);
	MouseInput(_fTime);
	return 0;
}

void CInput::KeyboardInput(float _fTime)
{
	// Ű����
	unordered_map<string, PKeyInfo>::iterator iter;
	unordered_map<string, PKeyInfo>::iterator iterEnd = m_mapKey.end();
	for (iter = m_mapKey.begin(); iter != iterEnd; ++iter)
	{
		size_t iCount = 0;
		for (size_t i = 0; i < iter->second->vecKey.size(); ++i)
		{
			if (GetAsyncKeyState(iter->second->vecKey[i]) & 0x8000)
			{
				++iCount;
			}
		}

		// Ű �Է��� ���� ������ ���
		if (iCount == iter->second->vecKey.size())
		{
			// Ű�� �Է� �Ǿ��� ���°� �ƴϰ�, ������ ���µ� �ƴ϶��?
			// > �Է� ���� ����, ������ �ִ� ���� true
			if ((iter->second->bPress == false) && (iter->second->bPush == false))
			{
				iter->second->bPress = true;
				iter->second->bPush  = true;

			}
			// > Ű �Է� ���� ����
			else if (iter->second->bPress == true)
			{
				iter->second->bPress = false;
			}
		}

		// Ű�� ������ �ִ� ���
		// > Ű �Է� ���� ������ true�� �����س��´�.
		else if (iter->second->bPush == true)
		{
			iter->second->bUp = true;
			iter->second->bPress = false;
			iter->second->bPush = false;
		}

		// Ű �Է��� ��ģ����(Up)
		else if (iter->second->bUp == true)
		{
			iter->second->bUp = false;
		}
	}
}

void CInput::MouseInput(float _fTime)
{
	// ���콺
	POINT tMousePos;
	GetCursorPos(&tMousePos);
	ScreenToClient(WINDOWHANDLE, &tMousePos);
	Vector2 vMousePos(tMousePos.x, tMousePos.y);

	// ȭ�� ����
	// > ������ �̿��Ͽ� ����̽� �ػ� �������� ��ǥ�� ���Ѵ�.
	RECT rc = {};
	GetClientRect(WINDOWHANDLE, &rc);
	vMousePos.y = rc.bottom - vMousePos.y;
	vMousePos *= GET_SINGLE(CDevice)->GetWindowToDeviceResolution();

	// ���콺 ��ǥ
	// > Gap, Client
	m_vMouseGap		= vMousePos - m_vMouseClient;
	m_vMouseClient  = vMousePos;
	m_pMouseTr->SetWorldPos(Vector3(m_vMouseClient.x, m_vMouseClient.y, 0.f));

	// ���콺 ������Ʈ ������Ʈ
	// > ���콺 ������Ʈ�� ������ �ִ� ������Ʈ�� Update, LateUpdate ȣ��
	m_pMouse->Update(_fTime);

	// ���콺 Ŀ�� ǥ��
	if (m_bShowCursor == false && (m_vMouseClient.x < 0 || m_vMouseClient.x > _RESOLUTION.iWidth ||
								   m_vMouseClient.y < 0 || m_vMouseClient.y > _RESOLUTION.iHeight))
	{
		m_bShowCursor = true;
		//while (ShowCursor(TRUE) != 0) {}
	}
	// ���콺 Ŀ�� ��Ȱ��ȭ
	else if (m_bShowCursor == true && (0.f <= m_vMouseClient.x && m_vMouseClient.x <= _RESOLUTION.iWidth &&
									   0.f <= m_vMouseClient.y && m_vMouseClient.y <= _RESOLUTION.iHeight))
	{
		m_bShowCursor = false;
		//while (ShowCursor(FALSE) >= 0) {}
	}
}

void CInput::Update(float fTime)
{
}

void CInput::LateUpdate(float _fTime)
{
	m_pWorldPoint->LateUpdate(_fTime);
}

bool CInput::KeyPress(const string& strKey)
{
	PKeyInfo pInfo = FindKey(strKey);
	if (pInfo == false)
	{
		return false;
	}
	return pInfo->bPress;
}

bool CInput::KeyPush(const string& strKey)
{
	PKeyInfo pInfo = FindKey(strKey);
	if (pInfo == false)
	{
		return false;
	}
	return pInfo->bPush;
}

bool CInput::KeyUp(const string& strKey)
{
	PKeyInfo pInfo = FindKey(strKey);
	if (pInfo == false)
	{
		return false;
	}
	return pInfo->bUp;
}

void CInput::RenderMouse(float fTime)
{
	if (m_pMouse->GetEnable() == true)
	{
		m_pMouse->Render(fTime);
	}
}