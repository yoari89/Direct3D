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

	// 브러쉬 색상을 생성한다.
	CreateBrush(Vector4::Red);
	CreateBrush(Vector4::Green);
	CreateBrush(Vector4::Blue);
	CreateBrush(Vector4::Black);
	CreateBrush(Vector4::White);

	return true;
}

IDWriteTextFormat* CFontManager::FindFont(const string& _strKey)
{
	// 폰트 찾기
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

	// 폰트 키 값은 "서체"로 정한다.
	sprintf_s(strKey, "%s", strKey);
	IDWriteTextFormat* pFormat = FindFont(strKey);
	if (pFormat != nullptr)
	{
		return pFormat;
	}

	// 텍스트 레이아웃에 사용되는 객체를 생성
	// > 1번인자 : 폰트 이름
	// > 2번인자 : 폰트 컬렉션
	// > 3번인자 : 폰트 굵기
	// > 4번인자 : 폰트 기울기
	// > 5번인자 : 폰트 자간
	// > 6번인자 : 폰트 크기
	// > 7번인자 : 언어 지역 이름을 설정한다. (한국은 ko 미국은 en)
	// > 8번인자 : IDWriteTextFormat** 타입 객체
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

	// 생성된 IDWriteTextFormat 객체 추가
	m_mapFont.insert(make_pair(strKey, pFormat));

	return pFormat;
}

ID2D1SolidColorBrush* CFontManager::FindBrush(const Vector4& _vColor)
{
	// _vColor 타입으로 브러쉬 찾기
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
	// float 타입으로 브러쉬 찾기 (0 ~ 255)
	return FindBrush((unsigned char)(_r * 255), (unsigned char)(_g * 255), 
					 (unsigned char)(_b * 255), (unsigned char)(_a * 255));
}

ID2D1SolidColorBrush* CFontManager::FindBrush(unsigned char _r, unsigned char _g, 
											  unsigned char _b, unsigned char _a)
{
	// unsigned char 타입으로 브러쉬 찾기 (0 ~ 255)
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

	// 추가
	m_mapBrush.insert(make_pair(key, pBrush));

	return pBrush;
}

ID2D1SolidColorBrush* CFontManager::CreateBrush(float _r, float _g, float _b, float _a)
{
	// 브러쉬 생성
	// > 브러쉬의 키 값은 float 4개의 타입이다.
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
	
	// 키 생성
	UINT key = CreateColorKey(_r, _g, _b, _a);

	// 생성된 ID2D1SolidColorBrush 객체 추가
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
	// 비트 연산을 이용한 키 생성(rgba)
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
	// 텍스트 레이아웃 생성
	// > IDWriteTextFormat 객체가 있는 경우
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
	// 텍스트 레이아웃 생성
	// > IDWriteTextFormat 객체가 없는 경우 키 값을 받아 생성 후 넣어준다.
	IDWriteTextLayout* pLayout = nullptr;
	IDWriteTextFormat* pFormat = FindFont(_strFontKey);
	if (FAILED(m_pWriteFactory->CreateTextLayout(_pText, lstrlen(_pText),
												 pFormat, _fWidth, _fHeight, &pLayout)))
	{
		return nullptr;
	}

	return pLayout;
}
