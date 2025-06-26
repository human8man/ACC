#pragma once

#include "Global.h"


//=============================================================================
//		ファイルの画像読込
//=============================================================================
namespace LoadImageFile 
{
	// 画像ファイルの幅、高さを取得する
	D3DXVECTOR2 GetImageSize( const std::string& FilePath );

	// jpgファイルの幅、高さを取得する
	D3DXVECTOR2 GetJpgImageSize( const std::string& FilePath );

	// pngファイルの幅、高さを取得する
	D3DXVECTOR2 GetPngImageSize( const std::string& FilePath );

	// bmpファイルの幅、高さを取得する
	D3DXVECTOR2 GetBmpImageSize( const std::string& FilePath );
}