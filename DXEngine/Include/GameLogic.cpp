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
	// �޸� �� üũ
	//_CrtSetBreakAlloc(1477077);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}

CCore::~CCore()
{
	// �̱��� ��ü ����
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
	// ����̽� �ʱ�ȭ
	if (GET_SINGLE(CDevice)->Init(_hWnd, _iWidth, _iHeight, _isWindowMode) == false)
	{
		tassert(true);
		return false;
	}

	// ���� Ÿ�� �� ����
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

	// ��� �ʱ�ȭ
	// > �ֻ��� ��ġ ����
	// > ���� ����� ��� �߰�
	if (GET_SINGLE(CPathManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// Ÿ�̸� �ʱ�ȭ
	// > Timer ��ü ����
	if (GET_SINGLE(CTimerManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// ���� ������ �ʱ�ȭ
	if (GET_SINGLE(CSoundManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// ������Ʈ ������ �ʱ�ȭ
	if (GET_SINGLE(CObjectManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// ��Ʈ ������ �ʱ�ȭ
	if (GET_SINGLE(CFontManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// �浹 �ʱ�ȭ
	// > �浹 �׷� ����
	if (GET_SINGLE(CCollisionManager)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// ���ҽ� �ʱ�ȭ
	// > �⺻ �޽�, ����� �浹�� �޽� ����
	// > �⺻ ���÷�(���� ���͸�, Linear) ����
	if (GET_SINGLE(CResourcesManager)->Init() == false)
	{
		tassert(true);
		return false; 
	}

	// ������ �ʱ�ȭ
	// > ���̴� �ʱ�ȭ
	//   - ���̴� ���� �ε� �� �Է� ���̾ƿ� ����
	// > ��� ���� ����
	//   - Transform(b0), Material(b1), Component(b2), Animation2D(b8), Collider(b8)
	// > ������ ���� ����
	//   - AlphaBlend(����, ���)
	//   - DepthStencil(����, 2D���� ������� �ʴ´�.)
	//   - Raterize(�̱���)
	if (GET_SINGLE(CRenderManager)->Init() == false)
	{
		tassert(true);
		return false; 
	}

	// �Է� �ʱ�ȭ
	// > Ű ���
	// > ���콺 ������Ʈ ���� (������ ���������� �������� �ȵǴ� ������Ʈ)
	//   - ���콺 ������Ʈ�� �ʿ��� ������Ʈ �߰�
	//   - �߰� ������Ʈ�� �ϱ� ���� ���ҽ���, ������ �۾����� �켱������ �ʿ��ϴ�.
	if (GET_SINGLE(CInput)->Init() == false)
	{
		tassert(true);
		return false;
	}

	// �� �ʱ�ȭ
	// > �� ����
	// > ó�� ���� ���콺 ������Ʈ ���
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
		// �޼����� �������� FALSE�� ��ȯ�ϸ鼭 �ٷ� �������´�.
		// �޼����� ���� ��� TRUE�� ��ȯ�ϰ� �ȴ�.
		// �� �޼����� �̿��ϸ� �������� ����Ÿ���� �̿��ؼ� ������ ������ �� �ִ�.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// ���� ����
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

	// ���� ���� ȣ�� ����
	if (Input(deltaTime) == 1)		{ return; }
	if (Update(deltaTime) == 1)		{ return; }
	if (LateUpdate(deltaTime) == 1) { return; }
	if (Collision(deltaTime) == 1)	{ return; }
	Render(deltaTime);
}

int CCore::Input(float _fTime)
{
	// (Ű����, ���콺) �Է� 
	GET_SINGLE(CInput)->Input(_fTime);

	// �� �Ŵ��� �Է�
	// > SceneComponent -> Layer -> GameObject -> UserComponent ������ ȣ��ȴ�.
	int iRet = GET_SINGLE(CSceneManager)->Input(_fTime);

	return iRet;
}

int CCore::Update(float _fTime)
{
	GET_SINGLE(CSoundManager)->Update();
	
	// �� �Ŵ��� ������Ʈ
	int iRet = GET_SINGLE(CSceneManager)->Update(_fTime);
	return iRet;
}

int CCore::LateUpdate(float _fTime)
{
	// �� �Ŵ��� (��)������Ʈ
	int iRet = GET_SINGLE(CSceneManager)->LateUpdate(_fTime);
	return iRet;
}

int CCore::Collision(float _fTime)
{
	// �� �Ŵ��� Collision() ȣ��
	// > �Լ� ���ο� CollisionManager(�浹 �Ŵ���)�� ���Ͽ� ���� �浹 ���� ���� �Ѵ�.
	return GET_SINGLE(CSceneManager)->Collision(_fTime);
}

void CCore::Render(float _fTime)
{
	// ���� Ÿ��, ���� ���� �� �ʱ�ȭ
	GET_SINGLE(CDevice)->Clear(m_arrFClearColor);

	// ���� ������
	// > ���콺�� ���� �������� ������ �Ѵ�.
	{
		GET_SINGLE(CSceneManager)->Render(_fTime);
		GET_SINGLE(CInput)->RenderMouse(_fTime);
	}

	// �ÿ�
	GET_SINGLE(CDevice)->Present();
}