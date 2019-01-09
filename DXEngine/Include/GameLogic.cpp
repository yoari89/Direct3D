#include "PCHEngine.h"
#include "Core.h"
#include "Device.h"
#include "Resources/Mesh.h"
#include "Rendering/Shader.h"
#include "Utility/EnumFlag.h"
#include "Utility/Timer/Timer.h"
#include "Utility/Input/Input.h"

// Manager Header
#include "Scene\SceneManager.h"
#include "Object/ObjectManager.h"
#include "Rendering\ShaderManager.h"
#include "Rendering\RenderManager.h"
#include "Resources\ResourcesManager.h"
#include "Utility/Font/FontManager.h"
#include "Utility/Path/PathManager.h"
#include "Utility/Timer/TimerManager.h"
#include "Utility/Sound/SoundManager.h"
#include "Utility/Thread/ThreadManager.h"
#include "Utility/Collision/CollisionManager.h"

ENGINE_USING
DEFINITION_SINGLE(CCore)

bool CCore::m_isLoop = true;

CCore::CCore()
{
	// 메모리 릭 체크
	//_CrtSetBreakAlloc(1477077);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}

CCore::~CCore()
{
	// 싱글톤 객체 삭제
	DESTROY_SINGLE(CSoundManager);
	DESTROY_SINGLE(CObjectManager);
	DESTROY_SINGLE(CSceneManager);
	DESTROY_SINGLE(CInput);
	DESTROY_SINGLE(CCollisionManager);
	DESTROY_SINGLE(CRenderManager);
	DESTROY_SINGLE(CShaderManager);
	DESTROY_SINGLE(CResourcesManager);
	DESTROY_SINGLE(CTimerManager);
	DESTROY_SINGLE(CPathManager);
	DESTROY_SINGLE(CFontManager);
	DESTROY_SINGLE(CThreadManager);
	DESTROY_SINGLE(CDevice);
}

bool CCore::DXInit(HWND _hWnd, unsigned int _iWidth, unsigned int _iHeight,
				   bool _isWindowMode)
{
	// 디바이스 초기화
	if (GET_SINGLE(CDevice)->Init(_hWnd, _iWidth, _iHeight, _isWindowMode) == false)
	{
		tassert(true);
		return false;
	}

	// 렌더 타겟 뷰 색상
	SetClearColor(0xcc, 0x66, 0xff, 0x00);

	return true;
}

void CCore::SetGameMode(GAME_MODE _eMode)
{
	m_eGameMode = _eMode;
	GET_SINGLE(CRenderManager)->SetGameMode(_eMode);
}

void CCore::SetMasterVolume(float _fVolume)
{
	GET_SINGLE(CSoundManager)->SetMasterVolume(_fVolume);
}

void CCore::SetVolume(float _fVolume, bool _isBGM)
{
	GET_SINGLE(CSoundManager)->SetVolume(_fVolume, _isBGM);
}

void CCore::SetClearColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
{
	m_arrFClearColor[0] = _r / 255.0f;
	m_arrFClearColor[1] = _g / 255.0f;
	m_arrFClearColor[2] = _b / 255.0f;
	m_arrFClearColor[3] = _a / 255.0f;
}

bool CCore::GameInit()
{
	if (GET_SINGLE(CThreadManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// 경로 초기화
	// > 최상위 위치 설정
	// > 실제 사용할 경로 추가
	if (GET_SINGLE(CPathManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// 타이머 초기화
	// > Timer 객체 생성
	if (GET_SINGLE(CTimerManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// 사운드 관리자 초기화
	if (GET_SINGLE(CSoundManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// 오브젝트 관리자 초기화
	if (GET_SINGLE(CObjectManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// 폰트 관리자 초기화
	if (GET_SINGLE(CFontManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// 충돌 초기화
	// > 충돌 그룹 생성
	if (GET_SINGLE(CCollisionManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// 리소스 초기화
	// > 기본 메시, 디버그 충돌용 메시 생성
	// > 기본 샘플러(선형 필터링, Linear) 생성
	if (GET_SINGLE(CResourcesManager)->Init() == false)
	{
		tassert(true);
		return false; 
	}

	// 렌더링 초기화
	// > 셰이더 초기화
	//   - 셰이더 파일 로드 및 입력 레이아웃 설정
	// > 상수 버퍼 생성
	//   - Transform(b0), Material(b1), Component(b2), Animation2D(b8), Collider(b8)
	// > 렌더링 상태 생성
	//   - AlphaBlend(생성, 사용)
	//   - DepthStencil(생성, 2D에선 사용하지 않는다.)
	//   - Raterize(미구현)
	if (GET_SINGLE(CRenderManager)->Init() == false)
	{
		tassert(true);
		return false; 
	}

	// 입력 초기화
	// > 키 등록
	// > 마우스 오브젝트 생성 (게임이 끝날때까지 지워지면 안되는 오브젝트)
	//   - 마우스 오브젝트에 필요한 컴포넌트 추가
	//   - 추가 컴포넌트를 하기 위해 리소스와, 렌더링 작업들이 우선적으로 필요하다.
	if (GET_SINGLE(CInput)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// 씬 초기화
	// > 씬 생성
	// > 처음 씬에 마우스 오브젝트 등록
	if (GET_SINGLE(CSceneManager)->Init() == false)	
	{ 
		tassert(true);
		return false; 
	}

	return true;
}

int CCore::Run()
{
	MSG msg;
	while (m_isLoop)
	{
		// PeekMessage
		// 메세지가 없을때는 FALSE를 반환하면서 바로 빠져나온다.
		// 메세지가 있을 경우 TRUE를 반환하게 된다.
		// 이 메세지를 이용하면 윈도우의 데드타임을 이용해서 게임을 제작할 수 있다.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 게임 로직
			Logic();
		}
	}

	return (int)msg.wParam;
}

void CCore::Logic()
{
	// DeltaTime
	CTimer* pTiemer = GET_SINGLE(CTimerManager)->FindTimer("MainTimer");
	pTiemer->Update();
	float deltaTime = pTiemer->GetTime();

	// 게임 로직 호출 순서
	if (Input(deltaTime) == 1)		{ return; }
	if (Update(deltaTime) == 1)		{ return; }
	if (LateUpdate(deltaTime) == 1) { return; }
	if (Collision(deltaTime) == 1)	{ return; }
	Render(deltaTime);
}

int CCore::Input(float _fTime)
{
	// (키보드, 마우스) 입력 
	GET_SINGLE(CInput)->Input(_fTime);

	// 씬 매니저 입력
	// > SceneComponent -> Layer -> GameObject -> UserComponent 순서로 호출된다.
	int iRet = GET_SINGLE(CSceneManager)->Input(_fTime);

	return iRet;
}

int CCore::Update(float _fTime)
{
	GET_SINGLE(CSoundManager)->Update();
	
	// 씬 매니저 업데이트
	int iRet = GET_SINGLE(CSceneManager)->Update(_fTime);
	return iRet;
}

int CCore::LateUpdate(float _fTime)
{
	// 씬 매니저 (후)업데이트
	int iRet = GET_SINGLE(CSceneManager)->LateUpdate(_fTime);
	return iRet;
}

int CCore::Collision(float _fTime)
{
	// 씬 매니저 Collision() 호출
	// > 함수 내부에 CollisionManager(충돌 매니저)를 통하여 실제 충돌 관련 일을 한다.
	return GET_SINGLE(CSceneManager)->Collision(_fTime);
}

void CCore::Render(float _fTime)
{
	// 렌더 타겟, 깊이 버퍼 뷰 초기화
	GET_SINGLE(CDevice)->Clear(m_arrFClearColor);

	// 실제 렌더링
	// > 마우스를 가장 마지막에 렌더링 한다.
	{
		GET_SINGLE(CSceneManager)->Render(_fTime);
		GET_SINGLE(CInput)->RenderMouse(_fTime);
	}

	// 시연
	GET_SINGLE(CDevice)->Present();
}