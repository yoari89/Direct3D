#include "PCHEngine.h"
#include "UI.h"

ENGINE_USING

CUI::CUI()
{
	SetTag("UI");
	m_eComType = CT_UI;
}

CUI::CUI(const CUI& _ui) :
	CComponent(_ui)
{
	m_eUIType = _ui.m_eUIType;
}

CUI::~CUI()
{
}

/* 컴포넌트 클래스 가상함수 재정의 */

bool CUI::Init()
{
	return true;
}

int CUI::Input(float _fTime)
{
	return 0;
}

int CUI::Update(float _fTime)
{
	return 0;
}

int CUI::LateUpdate(float _fTime)
{
	return 0;
}

void CUI::Collision(float _fTime)
{
}

void CUI::Render(float _fTime)
{
}

void CUI::AfterClone()
{
}

/* UI */

UI_TYPE CUI::GetUIType() const
{
	return m_eUIType;
}