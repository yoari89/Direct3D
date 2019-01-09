#pragma once

ENGINE_BEGIN
class ENGINE_DLL CCore
{
private:
	static bool m_isLoop;
	HINSTANCE	m_hInst;
	HWND		m_hWnd;
	Resolution	m_tRs;
	float		m_arrFClearColor[4];
	GAME_MODE	m_eGameMode;

/* ������ */
public:
	HWND GetWindowHandle()			const;
	HINSTANCE _GetWindowInstance()	const;

	// ����
	bool Init(HINSTANCE _hInst, unsigned int _iWidth, unsigned int _iHeight,
			  const wchar_t* _pTitle, const wchar_t* _pClass, int _iIconID, int _iSmallIConID,
			  bool _isWindowMode = false);

	// ���
	bool Init(HINSTANCE _hInst, HWND _hWnd, unsigned int _iWidth, unsigned int _iHeight, 
			  bool _isWindowMode = false);

private:
	void Register(const wchar_t* _pClass, unsigned int _iIconID, unsigned int _iSmallIconID);
	void CreateWnd(const wchar_t* _pTitle, const wchar_t* _pClass);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/* ���� Ÿ�� �� */
public:
	void SetClearColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a);

/* Direct3D */
	bool DXInit(HWND _hWnd, unsigned int _iWidth, unsigned int _iHeight,
				bool _isWindowMode);

/* GameMode */
public:
	void SetGameMode(GAME_MODE _eMode);
	void SetMasterVolume(float _fVolume);
	void SetVolume(float _fVolume, bool _isBGM = false);

/* ���� ���� */
public:
	bool GameInit();
	int  Run();
	void Logic();

private:
	int  Input(float _fTime);
	int  Update(float _fTime);
	int  LateUpdate(float _fTime);
	int  Collision(float _fTime);
	void Render(float _fTime);

DECLARE_SINGLE(CCore)
};
ENGINE_END