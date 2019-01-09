#pragma once

ENGINE_BEGIN

// ======== Resoulution ========

typedef struct ENGINE_DLL _tagResolution
{
	unsigned int iWidth;
	unsigned int iHeight;

	_tagResolution()
	{
		iWidth  = 0;
		iHeight = 0;
	}

	_tagResolution(const _tagResolution& _trs)
	{
		*this = _trs;
	}

	_tagResolution(unsigned int _iWidth, unsigned int _iHeight) 
		: iWidth(_iWidth), iHeight(_iHeight)
	{
	}

}Resolution, *PResolution;

// ======== VertexInfo - Position, Color ========

typedef struct _tagVertexColor
{
	Vector3	vPos;
	Vector4	vColor;

	_tagVertexColor() { }
	_tagVertexColor(const _tagVertexColor& v) { *this = v; }
	_tagVertexColor(const Vector3& vP, const Vector4& vC) :
		vPos(vP),
		vColor(vC)
	{
	}
}VertexColor, *PVertexColor;

// ======== VertexInfo - Position, Texture ========

typedef struct _tagVertexTex
{
	Vector3	vPos;
	Vector2	vUV;

	_tagVertexTex() { }
	_tagVertexTex(const _tagVertexTex& v) { *this = v; }
	_tagVertexTex(const Vector3& vP, const Vector2& vT) :
		vPos(vP),
		vUV(vT)
	{
	}
}VertexTex, *PVertexTex;

// ======== Shader - Default Constant Buffer ========

typedef struct ENGINE_DLL _tagCBuffer
{
	ID3D11Buffer*	pBuffer;
	int				iSize;
	int				iShaderType;
	int				iRegister;
}CBuffer, *PCBuffer;

// ======== Shader - Transform Constant Buffer ========
// > 셰이더 코드에서 선언한 상수버퍼 구조와 일치해야한다.

typedef struct ENGINE_DLL _tagTransformCBuffer
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;
	Matrix	matWV;
	Matrix	matWVP;
	Vector3 vPivot;
	float	fEmpty;
	Vector3	vLength;
	float	fEmpty1;
}TransformCBuffer, *PTransformCBuffer;

// ======== Shader - Material Constant Buffer ========

typedef struct ENGINE_DLL _tagMaterial
{
	// Diffuse  : 난 반사광
	// Specular : 정 반사광
	Vector4	vDiffuse;
	int		iMirrorX;
	int		iMirrorY;
	Vector2	vEmpty;

	_tagMaterial() :
		vDiffuse(Vector4::White),
		iMirrorX(0),
		iMirrorY(0)
	{
	}
}Material, *PMaterial;

typedef struct ENGINE_DLL _tagComponentCBuffer
{
	int		iTextureAnimation2D;
	int		iAnimationType;
	Vector2 vEmpty;

}ComponentCBuffer, *PComponentCBuffer;

// ======== Shader - Material Constant Buffer ========

typedef struct ENGINE_DLL _tagClip2DFrame
{
	Vector2 vLT;
	Vector2 vRB;
}Clip2DFrame, *PClip2DFrame;

typedef struct ENGINE_DLL _tagAnimationClip2D
{
	string				strName;
	ANIMATION2D_TYPE	eType;
	ANIMATION_OPTION	eOption;
	class CTexture*		pTexture;
	int					iTexRegister;
	float				fTextureWidth;
	float				fTextureHeight;
	vector<Clip2DFrame> vecFrame;
	int					iFrame;
	float				fPlayTime;
	float				fPlayFixedTime;
	bool				isAniPlaying;
	vector<float>		vecFrameTime;
	_tagAnimationClip2D()
	{
		isAniPlaying = false;
		fPlayTime = 0.f;
		fPlayFixedTime = 0.f;
	}
}AnimationClip2D, *PAnimationClip2D;

typedef struct ENGINE_DLL _tagAnimation2DCBuffer
{
	Vector2 vUVLT;
	Vector2 vUVRB;
	int		iFrame;
	Vector3 vEmpty;
}Animation2DCBuffer, *PAnimation2DCBuffer;

///////////////////////////////////////////////////////////

typedef struct ENGINE_DLL _tagRendererCBuffer
{
	void*	pBuffer;
	string	strName;
	int		iSize;
}RendererCBuffer, *PRendererCBuffer;

///////////////////////////////////////////////////////////

typedef struct ENGINE_DLL _tagBoxInfo
{
	Vector3	vMin;
	Vector3	vMax;
	Vector3 vLength;
}BoxInfo, *PBoxInfo;

typedef struct ENGINE_DLL _tagCircleInfo
{
	Vector3	vCenter;
	float   fRadius;
}CircleInfo, *PCircleInfo;

typedef struct ENGINE_DLL _tagOBB2DInfo
{
	Vector3	vCenter;
	Vector3	vAxis[2];
	float	fLength[2];
}OBB2DInfo, *POBB2DInfo;

///////////////////////////////////////////////////////////

typedef struct ENGINE_DLL _tagPixel24
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}Pixel24, *PPixel24;

typedef struct ENGINE_DLL _tagPixel32
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
}Pixel32, *PPixel32;

typedef struct ENGINE_DLL _tagPixelInfo
{
	BoxInfo			tBox;
	Pixel24*		pPixel;
	int				iWidth;
	int				iHeight;
	Pixel24			tYasuoPixel;
}PixelInfo, *PPixelInfo;

typedef struct ENGINE_DLL _tagButtonCBuffer
{
	Vector4		vColor;
}ButtonCBuffer, *PButtonCBuffer;

typedef struct ENGINE_DLL _tagBarCBuffer
{
	float	fPercent;
	int		iMoveDir;
	Vector2	vEmpty;
	Vector4	vLight;
}BarCBuffer, *PBarCBuffer;

// ======== User Constant Buffer ========

typedef struct ENGINE_DLL _tagLogoCBuffer
{
	float	ratio;
	Vector3 empty;
}MapleLogoCBuffer, *PMapleLogoCBuffer;

typedef struct ENGINE_DLL _tagFadeCBuffer
{
	Vector4 vColorMin;
	Vector4 vColorMax;
	float	ratio;
	Vector3 empty;
}FadeCBuffer, *PFadeCBuffer;

//////////////////////////////////////////////////////////

typedef struct _tagTextureData
{
	string  textureTag;
	wstring fileName;
	_tagTextureData()
	{
		textureTag = "";
		fileName   = L"";
	}
}TextureData, *PTextureData;

typedef struct _tagSoundData
{
	bool   isLoop;
	string fileName;
	_tagSoundData()
	{
		isLoop   = false;
		fileName = "";
	}
}SoundData, *PSoundData;


ENGINE_END