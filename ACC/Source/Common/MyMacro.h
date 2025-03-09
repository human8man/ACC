#pragma once
#include <crtdbg.h> // _ASSERT_EXPR()で必要.

//===========================================================
//		マクロ.
//===========================================================

#define SAFE_RELEASE(p)	{if(p!=nullptr){(p)->Release();(p)=nullptr;}}
#define SAFE_DELETE(p)	{if(p!=nullptr){delete (p);(p)=nullptr;}}
#define SAFE_DELETE_ARRAY(p){if(p!=nullptr){delete[] (p);(p)=nullptr;}}
#define ERR_MSG(str, title)	{ MessageBox( nullptr, str, title, MB_OK ); }
#define IMGUI_JP(str) reinterpret_cast<const char*>(u8##str)

inline int		StrToInt	( const std::string& value ) { return std::stoi(value); }
inline bool		StrToBool	( const std::string& value ) { return (value == "true" || value == "1"); }
inline float	StrToFloat	( const std::string& value ) { return std::stof(value); }