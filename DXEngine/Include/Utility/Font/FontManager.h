#pragma once

ENGINE_BEGIN
class CFontManager
{
DECLARE_SINGLE(CFontManager)

private:
	// IDWriteFactory : DirectWrite ��ü�� ����µ� ���
	IDWriteFactory*								m_pWriteFactory;
	unordered_map<string, IDWriteTextFormat*>	m_mapFont;
	unordered_map<UINT, ID2D1SolidColorBrush*>	m_mapBrush;

public:
	bool Init();

// ��Ʈ
public:
	IDWriteTextFormat* FindFont(const string& _strKey);
	IDWriteTextFormat* MakeFont(const wchar_t* _pFontName, int _iWeight, int _iStyle, 
								int _iStretch, float _fSize, const wchar_t* _pLocalName);
// �귯��
public:
	ID2D1SolidColorBrush* FindBrush(const Vector4& _vColor);
	ID2D1SolidColorBrush* FindBrush(float _r, float _g, float _b, float _a);
	ID2D1SolidColorBrush* FindBrush(unsigned char _r, unsigned char _g,
									unsigned char _b, unsigned char _a);

	ID2D1SolidColorBrush* CreateBrush(const Vector4& _vColor);
	ID2D1SolidColorBrush* CreateBrush(float _r, float _g, float _b, float _a);
	ID2D1SolidColorBrush* CreateBrush(unsigned char _r, unsigned char _g,
									  unsigned char _b, unsigned char _a);
// ���� Ű
public:
	unsigned int CreateColorKey(float _r, float _g, float _b, float _a);
	unsigned int CreateColorKey(unsigned char _r, unsigned char _g,
								unsigned char _b, unsigned char _a);

// �ؽ�Ʈ ���̾ƿ�
// > �ؽ�Ʈ ����� ��Ÿ���� �������̽�
public:
	IDWriteTextLayout* CreateTextLayout(const wchar_t* _pText, IDWriteTextFormat* _pFormat,
										float _fWidth, float _fHeight);
	IDWriteTextLayout* CreateTextLayout(const wchar_t* _pText, const string& _strFontKey,
										float _fWidth, float _fHeight);
};
ENGINE_END