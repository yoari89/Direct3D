#pragma once

#define ENGINE_BEGIN namespace ENGINE {
#define ENGINE_END   }
#define ENGINE_USING using namespace ENGINE;

// 클라이언트 크기
#define SCREEN_WIDTH	1280
#define SCREEN_HEIGHT	720

// TRUE ASSERT
#define tassert(p) assert(!(p))

// Resolution
#define _RESOLUTION	GET_SINGLE(CDevice)->GetResolution()

// 메모리 해제
#define SAFE_DELETE(p)		 if(p != nullptr) { delete p;	  p = nullptr; }
#define SAFE_RELEASE(p)		 if(p != nullptr) { p->Release(); p = nullptr; }
#define SAFE_DELETE_ARRAY(p) if(p != nullptr) { delete[] p;   p = nullptr; }

// 컨테이너 메모리 해제
template<typename T>
void Safe_Delete_VecList(T& p)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = p.end();
	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}
	p.clear();
}


template<typename T>
void Safe_Release_VecList(T& p)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = p.end();
	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE((*iter));
	}
	p.clear();
}

template<typename T>
void Safe_Delete_Map(T& p)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = p.end();
	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}
	p.clear();
}

template<typename T>
void Safe_Release_Map(T& p)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = p.end();
	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second);
	}
	p.clear();
}

// 싱글톤
// DECLARE(선언), DEFINITION(정의)
#define DECLARE_SINGLE(Type)		\
	private:						\
		static Type* m_pInst;		\
	public:							\
		static Type* GetInst();		\
		static void DestroyInst();	\
	private:						\
		Type();						\
		~Type();

#define DEFINITION_SINGLE(Type)		\
	Type* Type::m_pInst = nullptr;	\
	Type* Type::GetInst()			\
	{								\
		if (m_pInst == nullptr)		\
			m_pInst = new Type;		\
			return m_pInst;			\
	}								\
	void Type::DestroyInst()		\
	{								\
		SAFE_DELETE(m_pInst);		\
	}

#define GET_SINGLE(Type)	 Type::GetInst()
#define DESTROY_SINGLE(Type) Type::DestroyInst()
















// D3D 객체
#define DEVICE			GET_SINGLE(CDevice)->GetDevice()
#define CONTEXT			GET_SINGLE(CDevice)->GetContext()
#define SWAPCHAIN		GET_SINGLE(CDevice)->GetSwapChain()

// Resolution
#define _RESOLUTION		GET_SINGLE(CDevice)->GetResolution()

// 윈도우 객체
#define	WINDOWHANDLE	GET_SINGLE(CCore)->GetWindowHandle()
#define	WINDOWINSTANCE	GET_SINGLE(CCore)->GetWindowInstance()

// 키 입력 상태
#define	KEYPUSH(key)	GET_SINGLE(CInput)->KeyPush(key)
#define	KEYPRESS(key)	GET_SINGLE(CInput)->KeyPress(key)
#define	KEYUP(key)		GET_SINGLE(CInput)->KeyUp(key)

// ========================================================================= //

// 경로
#define ROOT_PATH				"RootPath"
#define SHADER_PATH				"ShaderPath"
#define	TEXTURE_PATH			"TexturePath"
#define	SOUND_PATH				"SoundPath"

// Shader Key
#define STANDARD_COLOR_SHADER	"StandardColorShader"
#define	STANDARD_TEX_SHADER		"StandardTexShader"
#define	COLLIDER_SHADER			"ColliderShader"
#define	BUTTON_SHADER			"ButtonShader"
#define	BAR_SHADER				"BarShader"

// UserShader Key
#define MAPLELOGO_SHADER		"LogoShader"
#define FADE_SHADER				"FadeShader"


// InputLayout Key
#define	POS_COLOR_LAYOUT		"PosColorLayout"
#define	POS_UV_LAYOUT			"PosUVLayout"
#define	POS_LAYOUT				"PosLayout"

// Default Sampler Key 
#define	SAMPLER_LINEAR			"Linear"

// RenderState Key 
#define	ALPHA_BLEND				"AlphaBlend"
#define	DEPTH_DISABLE			"DepthDisable"