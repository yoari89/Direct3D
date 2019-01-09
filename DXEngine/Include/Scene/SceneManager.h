#pragma once

ENGINE_BEGIN
class CScene;
class CLayer;
class CGameObject;
class ENGINE_DLL CSceneManager
{
DECLARE_SINGLE(CSceneManager)

/* �� */
private:
	bool	m_isChange;
	CScene* m_pScene;
	CScene* m_pNextScene;

private:
	int ChangeScene();

public:
	CScene* GetScene()		const;
	CScene* GetNextScene()	const;
	void CreateNextScene(bool _isChange = true);
	void ChangeEnable();

/* �� ������Ʈ */
public:
	template<typename T>
	bool AddSceneComponent(const string& _strTag, bool _isCurrent = true)
	{
		// > ���� ��
		if (_isCurrent == true)
		{
			return m_pScene->AddSceneComponent<T>(_strTag);
		}
		// > ���� ��
		return m_pNextScene->AddSceneComponent<T>(_strTag);
	}

/* ���̾� */
public:
	void	AddLayer(const string& _strTag, int _iZOrder, bool _isCurrent = true);
	void	ChangeLayerZOrder(const string& _strTag, int _iZOrder, bool _isCurrent = true);
	CLayer* FindLayer(const string& _strTag, bool _isCurrent = true);
	CGameObject* FindObject(const string& _strTag);

/* ���� */
public:
	bool Init();					// �ʱ�ȭ
	int  Input(float _fTime);		// �Է�
	int  Update(float _fTime);		// (��) ������Ʈ
	int  LateUpdate(float _fTime);	// (��) ������Ʈ
	int  Collision(float _fTime);	// �浹
	void Render(float _fTime);		// �׸���
};
ENGINE_END