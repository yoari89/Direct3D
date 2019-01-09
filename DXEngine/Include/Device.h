#pragma once

ENGINE_BEGIN
class ENGINE_DLL CDevice
{
DECLARE_SINGLE(CDevice)

private:
	HWND					m_hWnd;
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	IDXGISwapChain*			m_pSwapChain;
	ID3D11RenderTargetView* m_pTargetView;
	ID3D11DepthStencilView* m_pDepthView;
	Resolution				m_tRS;

/* 2D Interface */
private:
	ID2D1RenderTarget*		m_p2DTarget;
	ID2D1Factory*			m_p2DFactory;

public:
	ID2D1RenderTarget*		Get2DRenderTarget()	const;

/* �ʱ�ȭ */
public:
	bool Init(HWND _hWnd, unsigned int _iWidth,
			  unsigned int _iHeight, bool _bWindowMode);

	// > ���� Ÿ�� �� �����
	void Clear(float _arrFClearColor[4]);

	// > ���� ��ȯ
	void Present();

public:
	ID3D11Device*		 GetDevice()	  const;
	ID3D11DeviceContext* GetContext()	  const;
	IDXGISwapChain*		 GetSwapChain()	  const;
	Resolution			 GetResolution()  const;
	Vector2 GetWindowToDeviceResolution() const;
};
ENGINE_END