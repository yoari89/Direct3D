#include "PCHEngine.h"
#include "Core.h"
#include "Utility/Types.h"

ENGINE_USING
HWND CCore::GetWindowHandle() const
{
	return m_hWnd;
}

HINSTANCE CCore::_GetWindowInstance() const
{
	return m_hInst;
}

bool CCore::Init(HINSTANCE _hInst, unsigned int _iWidth, unsigned int _iHeight,
				 const wchar_t* _pTitle, const wchar_t* _pClass, int _iIconID, 
				 int _iSmallIConID, bool _isWindowMode)
{
	m_hInst = _hInst;
	m_tRs	= Resolution(_iWidth, _iHeight);
	Register(_pClass, _iIconID, _iSmallIConID);
	CreateWnd(_pTitle, _pClass);
	
	return Init(m_hInst, m_hWnd, _iWidth, _iHeight, _isWindowMode);
}

bool CCore::Init(HINSTANCE _hInst, HWND _hWnd, unsigned int _iWidth, unsigned int _iHeight, 
				 bool _isWindowMode)
{
	m_hInst		  = _hInst;
	m_hWnd		  = _hWnd;
	m_tRs.iWidth  = _iWidth;
	m_tRs.iHeight = _iHeight;

	// DirectX 초기화
	DXInit(_hWnd, _iWidth, _iHeight, _isWindowMode);

#ifdef _DEBUG
	AllocConsole();
	SetConsoleOutputCP(CP_UTF8);
#endif

	// 게임로직에 필요한 데이터 초기화
	GameInit();

	return true;
}

void CCore::Register(const wchar_t* _pClass, unsigned int _iIconID, unsigned int _iSmallIconID)
{
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CCore::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInst;
	wcex.hIcon			= LoadIcon(m_hInst, MAKEINTRESOURCE(_iIconID));
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= _pClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(_iSmallIconID));
	RegisterClassEx(&wcex);
}

void CCore::CreateWnd(const wchar_t* _pTitle, const wchar_t* _pClass)
{
	m_hWnd = CreateWindow(_pClass, _pTitle, WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, 0, m_tRs.iWidth, m_tRs.iHeight,
						  nullptr, nullptr, m_hInst, nullptr);
	
	if (!m_hWnd)
	{
		return;
	}

	RECT rect = { 0, 0, (LONG)m_tRs.iWidth, (LONG)m_tRs.iHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	SetWindowPos(m_hWnd, HWND_TOPMOST, 100, 100,
				 rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
}

LRESULT CCore::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			m_isLoop = false;
			PostQuitMessage(0);
			break;
		}
		default:
		{
			if (wParam == SC_KEYMENU && (lParam >> 16) <= 0) return 0;
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return 0;
}