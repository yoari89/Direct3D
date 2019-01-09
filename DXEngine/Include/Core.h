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

/* 윈도우 */
public:
	HWND GetWindowHandle()			const;
	HINSTANCE _GetWindowInstance()	const;

	// 생성
	bool Init(HINSTANCE _hInst, unsigned int _iWidth, unsigned int _iHeight,
			  const wchar_t* _pTitle, const wchar_t* _pClass, int _iIconID, int _iSmallIConID,
			  bool _isWindowMode = false);

	// 사용
	bool Init(HINSTANCE _hInst, HWND _hWnd, unsigned int _iWidth, unsigned int _iHeight, 
			  bool _isWindowMode = false);

private:
	void Register(const wchar_t* _pClass, unsigned int _iIconID, unsigned int _iSmallIconID);
	void CreateWnd(const wchar_t* _pTitle, const wchar_t* _pClass);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/* 렌더 타겟 뷰 */
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

/* 게임 로직 */
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