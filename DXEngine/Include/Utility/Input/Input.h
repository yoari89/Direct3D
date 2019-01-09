#pragma once

ENGINE_BEGIN

typedef struct ENGINE_DLL _tagKeyInfo
{
	bool			bPress;	// Ű �Է� ����
	bool			bPush;	// Ű�� ������ �ִ� ����
	bool			bUp;	// Ű �Է� ���� ����
	string			strName;
	vector<DWORD>	vecKey;
}KeyInfo, *PKeyInfo;


class CScene;
class CGameObject;
class CTransform;
class CColliderPoint;
class ENGINE_DLL CInput
{
DECLARE_SINGLE(CInput)

private:
	bool			m_bShowCursor;
	PKeyInfo		m_pCreateKey;
	Vector2			m_vMouseClient;	// ������ ��ǥ
	Vector2			m_vMouseWorld;	// ���� ����(����) ��ǥ
	Vector2			m_vMouseGap;
	CGameObject*	m_pMouse;
	CTransform*		m_pMouseTr;
	CColliderPoint* m_pWorldPoint;
	unordered_map<string, PKeyInfo>	m_mapKey;

private:
	PKeyInfo FindKey(const string& strKey);

public:
	void ChangeMouseScene(CScene* pScene);
	void AddMouseWorldCollision();
	Vector2 GetMouseGap()		const;
	Vector2 GetMouseClient()	const;
	Vector2 GetMouseWorld()		const;
	CGameObject* GetMouseObj()	const;
	unordered_map<string, PKeyInfo>* GetKeyList();

/* ���ø� �������� */
public:
	// > ���ø� ���������� �������� �Լ��� ������ش�.
	template <typename T>
	bool AddKey(const T& value)
	{
		const char* pType = typeid(T).name();
		if (strcmp(pType, "int") == 0 || strcmp(pType, "char") == 0)
		{
			m_pCreateKey->vecKey.push_back((DWORD)value);
		}
		else
		{
			m_pCreateKey->strName = value;
			m_mapKey.insert(make_pair(m_pCreateKey->strName, m_pCreateKey));
		}
		return true;
	}

	template <typename T, typename ... Types>
	bool AddKey(const T& value, Types ... Args)
	{
		if (!m_pCreateKey)
		{
			m_pCreateKey		 = new KeyInfo;
			m_pCreateKey->bPress = false;
			m_pCreateKey->bPush  = false;
			m_pCreateKey->bUp	 = false;
		}

		const char* pType = typeid(T).name();
		if (strcmp(pType, "int") == 0 || strcmp(pType, "char") == 0)
		{
			// int, char Ÿ���� �ִ� ���� PKeyInfo ����� vecKey�� ���� �߰���Ų��.
			m_pCreateKey->vecKey.push_back((DWORD)value);
		}
		else
		{
			// ���ڿ��� ������ ���� �ش� ���ڿ��� Ű�� PKeyInfo ������ �����.
			m_pCreateKey->strName = value;
			m_mapKey.insert(make_pair(m_pCreateKey->strName, m_pCreateKey));
		}

		// ���
		AddKey(Args...);

		// Ű�� ���� ����� ���� nullptr �ʱ�ȭ
		if (m_pCreateKey != nullptr)
		{
			m_pCreateKey = nullptr;
		}

		return true;
	}

public:
	bool Init();
	int  Input(float _fTime);
	void KeyboardInput(float _fTime);
	void MouseInput(float _fTime);

	void Update(float fTime);
	void LateUpdate(float _fTime);
	bool KeyPress(const string& strKey);
	bool KeyPush(const string& strKey);
	bool KeyUp(const string& strKey);
	void RenderMouse(float fTime);
};
ENGINE_END