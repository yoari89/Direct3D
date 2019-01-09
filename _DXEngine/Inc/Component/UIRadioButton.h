#pragma once
#include <map>
#include "UIButton.h"

ENGINE_BEGIN
class CCollider;
class ENGINE_DLL CUIRadioButton : public CUIButton
{
private:
	friend class CGameObject;

public:
	CUIRadioButton();
	CUIRadioButton(const CUIRadioButton& _radioBtn);
	~CUIRadioButton();

private:
	// Group
	map<string, list<CGameObject*>> m_mapGroupList;

public:
	bool Init()						override;
	int  Input(float _fTime)		override;
	int  Update(float _fTime)		override;
	int  LateUpdate(float _fTime)	override;
	void Collision(float _fTime)	override;
	void Render(float _fTime)		override;
	void AfterClone()				override;
	CUIRadioButton* Clone()			override;

public:
	list<CGameObject*>* FindGroup(string _groupName);
	void AddObject(string _groupName, CGameObject* _pObj);

public:
	void CheckBoxStateInit(string _groupName);
};
ENGINE_END