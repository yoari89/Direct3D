#include "PCHClient3D.h"
#include "Core.h"
//#include "Macro.h"
#include "resource.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include <ctime>

ENGINE_USING
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR    lpCmdLine,
					  _In_ int       nCmdShow)
{

	srand((unsigned int)time(NULL));
	if (CCore::GetInst()->Init(hInstance, SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("MapleStory"), TEXT("MapleStory"),
							   IDI_ICON1, IDI_ICON1, true) == false)
	{
		CCore::DestroyInst();
		return 0;
	}

	// LogoScene ȣ��
	//GET_SINGLE(CSceneManager)->AddSceneComponent<CLogoScene>("LogoScene", true);
	{
		// �׽�Ʈ ��
		//GET_SINGLE(CSceneManager)->AddSceneComponent<CTitleScene>("TitleScene", true);
		//GET_SINGLE(CSceneManager)->AddSceneComponent<CIntroScene>("IntroScene", true);
		//GET_SINGLE(CSceneManager)->AddSceneComponent<CGameScene>("GameScene", true);
	}

	// ���� ���� ����
	int	iResult = CCore::GetInst()->Run();

	// Ŭ���̾�Ʈ�� ������ �Ŵ��� �Ҹ��� ȣ��
	//DESTROY_SINGLE(CDataManager);

	// ��ü �ھ� ����
	CCore::DestroyInst();

	return iResult;
}