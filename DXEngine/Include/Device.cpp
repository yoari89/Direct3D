#include "PCHEngine.h"
#include "Device.h"
#include "Utility/Types.h"

ENGINE_USING
DEFINITION_SINGLE(CDevice)

CDevice::CDevice() :
	m_pDevice(nullptr),
	m_pContext(nullptr),
	m_pSwapChain(nullptr),
	m_pTargetView(nullptr),
	m_pDepthView(nullptr),
	m_p2DTarget(nullptr),
	m_p2DFactory(nullptr)
{
}

CDevice::~CDevice()
{
	SAFE_RELEASE(m_pDepthView);
	SAFE_RELEASE(m_pTargetView);
	SAFE_RELEASE(m_pSwapChain);
	if (m_pContext != nullptr)
	{
		// 모든 장치 컨텍스트를 기본 설정으로 다시 설정
		m_pContext->ClearState();
	}
	SAFE_RELEASE(m_p2DTarget);
	SAFE_RELEASE(m_p2DFactory);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
}

ID2D1RenderTarget* CDevice::Get2DRenderTarget() const
{
	return m_p2DTarget;
}

bool CDevice::Init(HWND _hWnd, unsigned int _iWidth, unsigned int _iHeight, bool _bWindowMode)
{
	m_hWnd		  = _hWnd;
	m_tRS.iWidth  = _iWidth;
	m_tRS.iHeight = _iHeight;

	// 스왑체인(SwapChain)
	// 스왑체인 구조체 초기화
	DXGI_SWAP_CHAIN_DESC tSwapDesc = {};
	tSwapDesc.BufferDesc.Width					 = _iWidth;						// 백 버퍼의 넓이와 높이를 지정
	tSwapDesc.BufferDesc.Height					 = _iHeight;
	tSwapDesc.BufferDesc.RefreshRate.Numerator   = 60;							// 모니터 주사율 지정
	tSwapDesc.BufferDesc.RefreshRate.Denominator = 1;
	tSwapDesc.BufferCount						 = 1;							// 백 버퍼 사용 갯수
	tSwapDesc.OutputWindow						 = _hWnd;						// 렌더링에 사용될 윈도우 핸들을 지정
	tSwapDesc.Windowed							 = _bWindowMode;				// 윈도우 창모드 / 풀 스크린 모드 설정
	tSwapDesc.SwapEffect						 = DXGI_SWAP_EFFECT_DISCARD;	// 출력된 다음 백 버퍼를 비우도록 지정
	tSwapDesc.Flags								 = 0;							// 추가 옵션 플래그를 사용하지 않는다.

	// 픽셀 포맷 처리 형식 (표면 설정)
	//                     └> 표면이 실제 픽셀의 정보 저장을 담당한다.
	// DXGI_FORMAT_R8G8B8A8_UNORM : Unsigned Normalize ( 8bit 크기를 정규화 0 ~ 1 )
	tSwapDesc.BufferDesc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
	
	// 확대/축소(Scaling) 및 한 줄 읽어 들이는 방법(ScanlineOrdering)
	tSwapDesc.BufferDesc.Scaling		  = DXGI_MODE_SCALING_UNSPECIFIED;
	tSwapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	
	// 멀티 샘플링 
	// (이미지 계단 현상을 보완해주지만, 속도가 매우 느려서 사용하지 않는다.)
	tSwapDesc.SampleDesc.Count   = 1;
	tSwapDesc.SampleDesc.Quality = 0;
	
	// 백 버퍼의 사용 용도 지정 
	// (DXGI_USAGE_RENDER_TARGET_OUTPUT : 렌더 타겟(기본은 백버퍼)을 출력하기 위한 용도)
	tSwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	unsigned int iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	// 픽셀 포맷이 달라도 처리가 가능하게 해주는 속성
	iFlag |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	// 피처레벨을 DirectX 11로 설정
	D3D_FEATURE_LEVEL eFLevels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL eFeatureLevel;

	// 스왑체인, Device, Context 생성
	if (FAILED(D3D11CreateDeviceAndSwapChain(
			nullptr,					// 그래픽 카드 장치(nullptr : 기본으로 설정되어있는 그래픽 카드 사용)
			 D3D_DRIVER_TYPE_HARDWARE,	// 하드웨어(그래픽 카드)
			0,							// DriverType이 소프트웨어로 설정되있을 경우 별도로 설정하는 값이 있는 경우
			iFlag,						// 디바이스를 초기화할 때 어떤 옵션을 설정해서 만들 것인가에 대한 조합
			&eFLevels,					// Direct3D 장치가 대상으로하는 기능 집합
			1,							// D3D_FEATURE_LEVEL의 요소 갯수
			D3D11_SDK_VERSION,			// SDK 버전
			&tSwapDesc,					// 스왑체인의 값이 설정된 포인터 (위에서 스왑체인에 대한 설정이 이미 되어있다.)
			&m_pSwapChain,				// 생성된 스왑체인을 주소 값을 받는 IDXGISwapChain 타입의 포인터 변수
			&m_pDevice,					// 생성된 디바이스를 주소 값을 받는 ID3D11Device 타입의 포인터 변수
			&eFeatureLevel,				// 설정된 D3D_FEATURE_LEVEL 타입의 포인터 변수
			&m_pContext					// 생성된 컨텍스트를 주소 값을 받는 ID3D11DeviceContext 타입의 포인터 변수 
		)))
	{
		return false;
	}

	// 렌더 타겟뷰(RenderTargetView)
	ID3D11Texture2D* pBuffer = nullptr;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	m_pDevice->CreateRenderTargetView(pBuffer, nullptr, &m_pTargetView);
	SAFE_RELEASE(pBuffer); 

	// 깊이 스텐실 뷰(DepthStencilView)
	// 깊이 버퍼 구조체 초기화
	D3D11_TEXTURE2D_DESC tDepthBuffer = {};
	tDepthBuffer.Width		= _iWidth;						 // 깊이 버퍼의 넓이와 높이 (화면사이즈와 깊이버퍼의 크기는 같다)
	tDepthBuffer.Height		= _iHeight;
	tDepthBuffer.ArraySize	= 1;
	tDepthBuffer.BindFlags	= D3D11_BIND_DEPTH_STENCIL;		 // 깊이 버퍼를 사용하기 위한 플래그
	tDepthBuffer.Format		= DXGI_FORMAT_D24_UNORM_S8_UINT; // 깊이 버퍼는 어떤 픽셀 포맷을 사용할 것인지 지정
	tDepthBuffer.MipLevels  = 1;							 // 밉맵 레벨

	// 멀티 샘플링 (사용하지 않음)
	tDepthBuffer.SampleDesc.Count = 1;
	tDepthBuffer.SampleDesc.Quality = 0;

	// 리소스가 CPU/GPU 에서 접근여부를 지정
	// D3D11_USAGE_DEFAULT   : GPU에 의한 읽기 및 쓰기 액세스가 필요한 리소스
	// D3d11_USAGE_IMMUTABLE : GPU에서만 읽을 수만 있는 리소스(GPU 읽기 전용, CPU는 전혀 접근이 불가능)
	// D3D11_USAGE_DYNAMIC   : GPU(읽기 전용)와 CPU(쓰기 전용)가 모두 액세스 할 수있는 리소스
	// D3D11_USAGE_STAGING   : GPU에서 CPU로 데이터 전송(복사)을 지원하는 리소스.
	tDepthBuffer.Usage = D3D11_USAGE_DEFAULT;

	// 위에서 설정된 깊이 버퍼 구조체를 사용하여 깊이 버퍼 텍스처2D를 생성
	if (FAILED(m_pDevice->CreateTexture2D(&tDepthBuffer, nullptr, &pBuffer)))
	{
		return false;
	}
	m_pDevice->CreateDepthStencilView(pBuffer, nullptr, &m_pDepthView);
	SAFE_RELEASE(pBuffer);
	
	// 렌더 타겟 뷰와 깊이 버퍼를 렌더링 파이프라인 '출력 병합기(Output Merget)'단계에 묶는다.
	m_pContext->OMSetRenderTargets(1, &m_pTargetView, m_pDepthView);

	// 뷰 포트(View Port) 생성
	// (장면을 그려 넣고자 하는 후면 버퍼의 부분 직사각형 영역을 의미한다.)
	D3D11_VIEWPORT tVP	= {};
	tVP.Width			= (float)_iWidth;
	tVP.Height			= (float)_iHeight;
	tVP.MaxDepth		= 1.0f;

	// 래스터라이저 단계
	// (메쉬가 차지하는 픽셀의 범위를 산출하는 단계)
	// (래스터 라이저 단계에 뷰포트의 배열을 묶는다.)
	m_pContext->RSSetViewports(1, &tVP);

	// D2D Factory를 생성한다.
	D2D1_FACTORY_OPTIONS	tOption = {};
	tOption.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, tOption, &m_p2DFactory)))
	{
		return false;
	}

	// DirectX11 BackBuffer 타겟을 이용하여 2D 렌더링 타겟을 설정해준다.
	IDXGISurface* pBackSurface = nullptr;
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackSurface));

	// 2D 렌더타겟을 생성하기 위한 옵션 설정
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_HARDWARE,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

	if (FAILED(m_p2DFactory->CreateDxgiSurfaceRenderTarget(pBackSurface, props, &m_p2DTarget)))
	{
		return false;
	}
	SAFE_RELEASE(pBackSurface);
	
	return true;
}

void CDevice::Clear(float _arrFClearColor[4])
{
	// 렌더 타겟 뷰(현재 프로젝트에선 백 버퍼가 묶여있다.)를 지우고,
	// 지정된 색상으로 화면을 채우고, 깊이 버퍼를 담당하는 뷰도 지운다.
	m_pContext->ClearRenderTargetView(m_pTargetView, _arrFClearColor);
	m_pContext->ClearDepthStencilView(m_pDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void CDevice::Present()
{
	// 스왑체인에 설정된 버퍼들을 전환
	m_pSwapChain->Present(0, 0);
}

ID3D11Device* CDevice::GetDevice() const
{
	return m_pDevice;
}

ID3D11DeviceContext* CDevice::GetContext() const
{
	return m_pContext;
}

IDXGISwapChain* CDevice::GetSwapChain() const
{
	return m_pSwapChain;
}

Resolution CDevice::GetResolution() const
{
	return m_tRS;
}

Vector2 CDevice::GetWindowToDeviceResolution() const
{
	RECT rc = {};
	GetClientRect(m_hWnd, &rc);
	return Vector2(m_tRS.iWidth / (float)rc.right, m_tRS.iHeight / (float)rc.bottom);
}