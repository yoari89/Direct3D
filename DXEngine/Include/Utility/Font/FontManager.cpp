#include "PCHEngine.h"
#include "FontManager.h"
#include "Device.h"

// Util
#include <atlstr.h>

ENGINE_USING
DEFINITION_SINGLE(CFontManager)

CFontManager::CFontManager()
{
}

CFontManager::~CFontManager()
{
	SAFE_RELEASE(m_pWriteFactory);
	Safe_Release_Map(m_mapFont);
	Safe_Release_Map(m_mapBrush);
}

bool CFontManager::Init()
{
	if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
								   __uuidof(m_pWriteFactory), (IUnknown**)&m_pWriteFactory)))
	{
		return false;
	}

	// �귯�� ������ �����Ѵ�.
	CreateBrush(Vector4::Red);
	CreateBrush(Vector4::Green);
	CreateBrush(Vector4::Blue);
	CreateBrush(Vector4::Black);
	CreateBrush(Vector4::White);

	return true;
}

IDWriteTextFormat* CFontManager::FindFont(const string& _strKey)
{
	// ��Ʈ ã��
	unordered_map<string, IDWriteTextFormat*>::iterator	iter = m_mapFont.find(_strKey);
	if (iter == m_mapFont.end())
	{
		return nullptr;
	}
	
	return iter->second;
}

IDWriteTextFormat* CFontManager::MakeFont(const wchar_t* _pFontName, int _iWeight, int _iStyle,
										  int _iStretch, float _fSize, const wchar_t* _pLocalName)
{
	char strKey[MAX_PATH] = {};

#ifdef UNICODE
	// > unicode > multibyte
	WideCharToMultiByte(CP_ACP, 0, _pFontName, -1, strKey, lstrlen(_pFontName) * 2, NULL, NULL);
#else
	strcpy_s(strKey, pFontName);
#endif // UNICODE

	// ��Ʈ Ű ���� "��ü"�� ���Ѵ�.
	sprintf_s(strKey, "%s", strKey);
	IDWriteTextFormat* pFormat = FindFont(strKey);
	if (pFormat != nullptr)
	{
		return pFormat;
	}

	// �ؽ�Ʈ ���̾ƿ��� ���Ǵ� ��ü�� ����
	// > 1������ : ��Ʈ �̸�
	// > 2������ : ��Ʈ �÷���
	// > 3������ : ��Ʈ ����
	// > 4������ : ��Ʈ ����
	// > 5������ : ��Ʈ �ڰ�
	// > 6������ : ��Ʈ ũ��
	// > 7������ : ��� ���� �̸��� �����Ѵ�. (�ѱ��� ko �̱��� en)
	// > 8������ : IDWriteTextFormat** Ÿ�� ��ü
	if (FAILED(m_pWriteFactory->CreateTextFormat(_pFontName, 
												 nullptr,
												 DWRITE_FONT_WEIGHT_REGULAR,
												 DWRITE_FONT_STYLE_NORMAL,
												 DWRITE_FONT_STRETCH_NORMAL,
												 _fSize,
												 _pLocalName, 
												 &pFormat)))
	{
		return nullptr;
	}

	// ������ IDWriteTextFormat ��ü �߰�
	m_mapFont.insert(make_pair(strKey, pFormat));

	return pFormat;
}

ID2D1SolidColorBrush* CFontManager::FindBrush(const Vector4& _vColor)
{
	// _vColor Ÿ������ �귯�� ã��
	unsigned int key = CreateColorKey(_vColor.r, _vColor.g, _vColor.b, _vColor.a);
	unordered_map<unsigned int, ID2D1SolidColorBrush*>::iterator iter = m_mapBrush.find(key);
	if (iter == m_mapBrush.end())
	{
		return nullptr;
	}

	return iter->second;
}

ID2D1SolidColorBrush* CFontManager::FindBrush(float _r, float _g, float _b, float _a)
{
	// float Ÿ������ �귯�� ã�� (0 ~ 255)
	return FindBrush((unsigned char)(_r * 255), (unsigned char)(_g * 255), 
					 (unsigned char)(_b * 255), (unsigned char)(_a * 255));
}

ID2D1SolidColorBrush* CFontManager::FindBrush(unsigned char _r, unsigned char _g, 
											  unsigned char _b, unsigned char _a)
{
	// unsigned char Ÿ������ �귯�� ã�� (0 ~ 255)
	UINT key = CreateColorKey(_r, _g, _b, _a);
	unordered_map<UINT, ID2D1SolidColorBrush*>::iterator iter = m_mapBrush.find(key);
	if (iter == m_mapBrush.end())
	{
		return nullptr;
	}

	return iter->second;
}

ID2D1SolidColorBrush* CFontManager::CreateBrush(const Vector4& _vColor)
{
	float r, g, b, a;
	r = _vColor.r;
	g = _vColor.g;
	b = _vColor.b;
	a = _vColor.a;

	ID2D1SolidColorBrush* pBrush = FindBrush(r, g, b, a);
	if (pBrush != nullptr)
	{
		return pBrush;
	}

	if (FAILED(GET_SINGLE(CDevice)->Get2DRenderTarget()->CreateSolidColorBrush(
			D2D1::ColorF(r, g, b, a), &pBrush)))
	{
		return nullptr;
	}

	unsigned int key = CreateColorKey(r, g, b, a);

	// �߰�
	m_mapBrush.insert(make_pair(key, pBrush));

	return pBrush;
}

ID2D1SolidColorBrush* CFontManager::CreateBrush(float _r, float _g, float _b, float _a)
{
	// �귯�� ����
	// > �귯���� Ű ���� float 4���� Ÿ���̴�.
	ID2D1SolidColorBrush* pBrush = FindBrush(_r, _g, _b, _a);
	if (pBrush != nullptr)
	{
		return pBrush;
	}

	if (FAILED(GET_SINGLE(CDevice)->Get2DRenderTarget()->
		CreateSolidColorBrush(D2D1::ColorF(_r, _g, _b, _a), &pBrush)))
	{
		return nullptr;
	}
	
	// Ű ����
	UINT key = CreateColorKey(_r, _g, _b, _a);

	// ������ ID2D1SolidColorBrush ��ü �߰�
	m_mapBrush.insert(make_pair(key, pBrush));

	return pBrush;
}

ID2D1SolidColorBrush* CFontManager::CreateBrush(unsigned char _r, unsigned char _g, 
												unsigned char _b, unsigned char _a)
{
	ID2D1SolidColorBrush* pBrush = FindBrush(_r, _g, _b, _a);
	if (pBrush != nullptr)
	{
		return pBrush;
	}

	if (FAILED(GET_SINGLE(CDevice)->Get2DRenderTarget()->CreateSolidColorBrush(
		D2D1::ColorF(_r / 255.f, _g / 255.f, _b / 255.f, _a / 255.f), &pBrush)))
	{
		return nullptr;
	}

	unsigned int key = CreateColorKey(_r, _g, _b, _a);
	m_mapBrush.insert(make_pair(key, pBrush));

	return pBrush;
}

unsigned int CFontManager::CreateColorKey(float _r, float _g, float _b, float _a)
{
	return CreateColorKey((unsigned char)(_r * 255), (unsigned char)(_g * 255), 
						  (unsigned char)(_b * 255), (unsigned char)(_a * 255));
}

unsigned int CFontManager::CreateColorKey(unsigned char _r, unsigned char _g, 
										  unsigned char _b, unsigned char _a)
{
	// ��Ʈ ������ �̿��� Ű ����(rgba)
	unsigned int key = _a;
	key <<= 8;
	key |= _r;
	key <<= 8;
	key |= _g;
	key <<= 8;
	key |= _b;

	return key;
}

IDWriteTextLayout* CFontManager::CreateTextLayout(const wchar_t* _pText, IDWriteTextFormat* _pFormat,
											      float _fWidth, float _fHeight)
{
	// �ؽ�Ʈ ���̾ƿ� ����
	// > IDWriteTextFormat ��ü�� �ִ� ���
	IDWriteTextLayout* pLayout = nullptr;
	if (FAILED(m_pWriteFactory->CreateTextLayout(_pText, lstrlen(_pText),
												 _pFormat, _fWidth, _fHeight, &pLayout)))
	{
		return nullptr;
	}

	return pLayout;
}

IDWriteTextLayout* CFontManager::CreateTextLayout(const wchar_t* _pText, const string& _strFontKey,
												  float _fWidth, float _fHeight)
{
	// �ؽ�Ʈ ���̾ƿ� ����
	// > IDWriteTextFormat ��ü�� ���� ��� Ű ���� �޾� ���� �� �־��ش�.
	IDWriteTextLayout* pLayout = nullptr;
	IDWriteTextFormat* pFormat = FindFont(_strFontKey);
	if (FAILED(m_pWriteFactory->CreateTextLayout(_pText, lstrlen(_pText),
												 pFormat, _fWidth, _fHeight, &pLayout)))
	{
		return nullptr;
	}

	return pLayout;
}
