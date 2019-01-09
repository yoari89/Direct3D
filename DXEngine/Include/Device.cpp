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
		// ��� ��ġ ���ؽ�Ʈ�� �⺻ �������� �ٽ� ����
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

	// ����ü��(SwapChain)
	// ����ü�� ����ü �ʱ�ȭ
	DXGI_SWAP_CHAIN_DESC tSwapDesc = {};
	tSwapDesc.BufferDesc.Width					 = _iWidth;						// �� ������ ���̿� ���̸� ����
	tSwapDesc.BufferDesc.Height					 = _iHeight;
	tSwapDesc.BufferDesc.RefreshRate.Numerator   = 60;							// ����� �ֻ��� ����
	tSwapDesc.BufferDesc.RefreshRate.Denominator = 1;
	tSwapDesc.BufferCount						 = 1;							// �� ���� ��� ����
	tSwapDesc.OutputWindow						 = _hWnd;						// �������� ���� ������ �ڵ��� ����
	tSwapDesc.Windowed							 = _bWindowMode;				// ������ â��� / Ǯ ��ũ�� ��� ����
	tSwapDesc.SwapEffect						 = DXGI_SWAP_EFFECT_DISCARD;	// ��µ� ���� �� ���۸� ��쵵�� ����
	tSwapDesc.Flags								 = 0;							// �߰� �ɼ� �÷��׸� ������� �ʴ´�.

	// �ȼ� ���� ó�� ���� (ǥ�� ����)
	//                     ��> ǥ���� ���� �ȼ��� ���� ������ ����Ѵ�.
	// DXGI_FORMAT_R8G8B8A8_UNORM : Unsigned Normalize ( 8bit ũ�⸦ ����ȭ 0 ~ 1 )
	tSwapDesc.BufferDesc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
	
	// Ȯ��/���(Scaling) �� �� �� �о� ���̴� ���(ScanlineOrdering)
	tSwapDesc.BufferDesc.Scaling		  = DXGI_MODE_SCALING_UNSPECIFIED;
	tSwapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	
	// ��Ƽ ���ø� 
	// (�̹��� ��� ������ ������������, �ӵ��� �ſ� ������ ������� �ʴ´�.)
	tSwapDesc.SampleDesc.Count   = 1;
	tSwapDesc.SampleDesc.Quality = 0;
	
	// �� ������ ��� �뵵 ���� 
	// (DXGI_USAGE_RENDER_TARGET_OUTPUT : ���� Ÿ��(�⺻�� �����)�� ����ϱ� ���� �뵵)
	tSwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	unsigned int iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	// �ȼ� ������ �޶� ó���� �����ϰ� ���ִ� �Ӽ�
	iFlag |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	// ��ó������ DirectX 11�� ����
	D3D_FEATURE_LEVEL eFLevels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL eFeatureLevel;

	// ����ü��, Device, Context ����
	if (FAILED(D3D11CreateDeviceAndSwapChain(
			nullptr,					// �׷��� ī�� ��ġ(nullptr : �⺻���� �����Ǿ��ִ� �׷��� ī�� ���)
			 D3D_DRIVER_TYPE_HARDWARE,	// �ϵ����(�׷��� ī��)
			0,							// DriverType�� ����Ʈ����� ���������� ��� ������ �����ϴ� ���� �ִ� ���
			iFlag,						// ����̽��� �ʱ�ȭ�� �� � �ɼ��� �����ؼ� ���� ���ΰ��� ���� ����
			&eFLevels,					// Direct3D ��ġ�� ��������ϴ� ��� ����
			1,							// D3D_FEATURE_LEVEL�� ��� ����
			D3D11_SDK_VERSION,			// SDK ����
			&tSwapDesc,					// ����ü���� ���� ������ ������ (������ ����ü�ο� ���� ������ �̹� �Ǿ��ִ�.)
			&m_pSwapChain,				// ������ ����ü���� �ּ� ���� �޴� IDXGISwapChain Ÿ���� ������ ����
			&m_pDevice,					// ������ ����̽��� �ּ� ���� �޴� ID3D11Device Ÿ���� ������ ����
			&eFeatureLevel,				// ������ D3D_FEATURE_LEVEL Ÿ���� ������ ����
			&m_pContext					// ������ ���ؽ�Ʈ�� �ּ� ���� �޴� ID3D11DeviceContext Ÿ���� ������ ���� 
		)))
	{
		return false;
	}

	// ���� Ÿ�ٺ�(RenderTargetView)
	ID3D11Texture2D* pBuffer = nullptr;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	m_pDevice->CreateRenderTargetView(pBuffer, nullptr, &m_pTargetView);
	SAFE_RELEASE(pBuffer); 

	// ���� ���ٽ� ��(DepthStencilView)
	// ���� ���� ����ü �ʱ�ȭ
	D3D11_TEXTURE2D_DESC tDepthBuffer = {};
	tDepthBuffer.Width		= _iWidth;						 // ���� ������ ���̿� ���� (ȭ�������� ���̹����� ũ��� ����)
	tDepthBuffer.Height		= _iHeight;
	tDepthBuffer.ArraySize	= 1;
	tDepthBuffer.BindFlags	= D3D11_BIND_DEPTH_STENCIL;		 // ���� ���۸� ����ϱ� ���� �÷���
	tDepthBuffer.Format		= DXGI_FORMAT_D24_UNORM_S8_UINT; // ���� ���۴� � �ȼ� ������ ����� ������ ����
	tDepthBuffer.MipLevels  = 1;							 // �Ӹ� ����

	// ��Ƽ ���ø� (������� ����)
	tDepthBuffer.SampleDesc.Count = 1;
	tDepthBuffer.SampleDesc.Quality = 0;

	// ���ҽ��� CPU/GPU ���� ���ٿ��θ� ����
	// D3D11_USAGE_DEFAULT   : GPU�� ���� �б� �� ���� �׼����� �ʿ��� ���ҽ�
	// D3d11_USAGE_IMMUTABLE : GPU������ ���� ���� �ִ� ���ҽ�(GPU �б� ����, CPU�� ���� ������ �Ұ���)
	// D3D11_USAGE_DYNAMIC   : GPU(�б� ����)�� CPU(���� ����)�� ��� �׼��� �� ���ִ� ���ҽ�
	// D3D11_USAGE_STAGING   : GPU���� CPU�� ������ ����(����)�� �����ϴ� ���ҽ�.
	tDepthBuffer.Usage = D3D11_USAGE_DEFAULT;

	// ������ ������ ���� ���� ����ü�� ����Ͽ� ���� ���� �ؽ�ó2D�� ����
	if (FAILED(m_pDevice->CreateTexture2D(&tDepthBuffer, nullptr, &pBuffer)))
	{
		return false;
	}
	m_pDevice->CreateDepthStencilView(pBuffer, nullptr, &m_pDepthView);
	SAFE_RELEASE(pBuffer);
	
	// ���� Ÿ�� ��� ���� ���۸� ������ ���������� '��� ���ձ�(Output Merget)'�ܰ迡 ���´�.
	m_pContext->OMSetRenderTargets(1, &m_pTargetView, m_pDepthView);

	// �� ��Ʈ(View Port) ����
	// (����� �׷� �ְ��� �ϴ� �ĸ� ������ �κ� ���簢�� ������ �ǹ��Ѵ�.)
	D3D11_VIEWPORT tVP	= {};
	tVP.Width			= (float)_iWidth;
	tVP.Height			= (float)_iHeight;
	tVP.MaxDepth		= 1.0f;

	// �����Ͷ����� �ܰ�
	// (�޽��� �����ϴ� �ȼ��� ������ �����ϴ� �ܰ�)
	// (������ ������ �ܰ迡 ����Ʈ�� �迭�� ���´�.)
	m_pContext->RSSetViewports(1, &tVP);

	// D2D Factory�� �����Ѵ�.
	D2D1_FACTORY_OPTIONS	tOption = {};
	tOption.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, tOption, &m_p2DFactory)))
	{
		return false;
	}

	// DirectX11 BackBuffer Ÿ���� �̿��Ͽ� 2D ������ Ÿ���� �������ش�.
	IDXGISurface* pBackSurface = nullptr;
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackSurface));

	// 2D ����Ÿ���� �����ϱ� ���� �ɼ� ����
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
	// ���� Ÿ�� ��(���� ������Ʈ���� �� ���۰� �����ִ�.)�� �����,
	// ������ �������� ȭ���� ä���, ���� ���۸� ����ϴ� �䵵 �����.
	m_pContext->ClearRenderTargetView(m_pTargetView, _arrFClearColor);
	m_pContext->ClearDepthStencilView(m_pDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void CDevice::Present()
{
	// ����ü�ο� ������ ���۵��� ��ȯ
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