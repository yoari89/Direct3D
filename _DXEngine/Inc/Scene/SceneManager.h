#pragma once

ENGINE_BEGIN
class CScene;
class CLayer;
class CGameObject;
class ENGINE_DLL CSceneManager
{
DECLARE_SINGLE(CSceneManager)

/* 씬 */
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

/* 씬 컴포넌트 */
public:
	template<typename T>
	bool AddSceneComponent(const string& _strTag, bool _isCurrent = true)
	{
		// > 현재 씬
		if (_isCurrent == true)
		{
			return m_pScene->AddSceneComponent<T>(_strTag);
		}
		// > 다음 씬
		return m_pNextScene->AddSceneComponent<T>(_strTag);
	}

/* 레이어 */
public:
	void	AddLayer(const string& _strTag, int _iZOrder, bool _isCurrent = true);
	void	ChangeLayerZOrder(const string& _strTag, int _iZOrder, bool _isCurrent = true);
	CLayer* FindLayer(const string& _strTag, bool _isCurrent = true);
	CGameObject* FindObject(const string& _strTag);

/* 로직 */
public:
	bool Init();					// 초기화
	int  Input(float _fTime);		// 입력
	int  Update(float _fTime);		// (전) 업데이트
	int  LateUpdate(float _fTime);	// (후) 업데이트
	int  Collision(float _fTime);	// 충돌
	void Render(float _fTime);		// 그리기
};
ENGINE_END