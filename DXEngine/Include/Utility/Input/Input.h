#pragma once

ENGINE_BEGIN

typedef struct ENGINE_DLL _tagKeyInfo
{
	bool			bPress;	// 키 입력 시점
	bool			bPush;	// 키를 누르고 있는 상태
	bool			bUp;	// 키 입력 종료 시점
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
	Vector2			m_vMouseClient;	// 윈도우 좌표
	Vector2			m_vMouseWorld;	// 게임 공간(월드) 좌표
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

/* 템플릿 가변인자 */
public:
	// > 템플릿 가변인자의 종료조건 함수를 만들어준다.
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
			// int, char 타입이 있는 경우는 PKeyInfo 멤버의 vecKey에 값을 추가시킨다.
			m_pCreateKey->vecKey.push_back((DWORD)value);
		}
		else
		{
			// 문자열이 들어오는 경우는 해당 문자열을 키로 PKeyInfo 정보를 만든다.
			m_pCreateKey->strName = value;
			m_mapKey.insert(make_pair(m_pCreateKey->strName, m_pCreateKey));
		}

		// 재귀
		AddKey(Args...);

		// 키를 새로 만들기 위해 nullptr 초기화
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