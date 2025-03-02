#include "LoadImage.h"
#include <iostream>
#include <fstream>


namespace {
	constexpr int	_1BYTE_BIT_NUM		= 8;				// 1バイトのビット数.
	constexpr int	_3BYTE_BIT_NUM		= 24;				// 2バイトのビット数.
	constexpr int	_1BYTE_MAX_10BASE	= 256;				// 1バイトで表せる最大10進数.
	constexpr int	JPG_END_DQTMARKER	= 0xC0;				// jpgの移動を終了する数.
	constexpr int	SKIP_2BIT			= 2;				// 使用しない情報があるため飛ばす数.
	constexpr int	SKIP_5BIT			= 5;				// 使用しない情報があるため飛ばす数.
	constexpr int	SKIP_16BIT			= 16;				// 使用しない情報があるため飛ばす数.
	constexpr int	SKIP_18BIT			= 18;				// 使用しない情報があるため飛ばす数.
	constexpr float ERROR_SIZE[2]		= { -1.0f, -1.0f };	// エラーが出たときに時に返す幅、高さ.

	// jpg用の画像データ.
	struct stJpgData {
		std::uint16_t	Height;		// 画像の高さ.
		std::uint16_t	Width;		// 画像の幅.
	} typedef SJpgData;

	// jpg以外の画像データ.
	struct stData {
		std::uint32_t	Width;		// 画像の幅.
		std::uint32_t	Height;		// 画像の高さ.
	} typedef SData;
}


//=============================================================================
//		画像ファイルの幅、高さを取得する.
//=============================================================================
D3DXVECTOR2 LoadImageFile::GetImageSize( const std::string& FilePath )
{
	// 拡張子を見て判断して対応した関数を使用する.
	std::string Extension = FilePath;
	Extension.erase( 0, FilePath.rfind( "." ) );
	if ( Extension == ".jpg" || Extension == ".JPG" ) return GetJpgImageSize( FilePath );
	if ( Extension == ".png" || Extension == ".PNG" ) return GetPngImageSize( FilePath );
	if ( Extension == ".bmp" || Extension == ".BMP" ) return GetBmpImageSize( FilePath );

	// jpg/png/bmpファイルではない.
	return ERROR_SIZE;
}


//=============================================================================
//		jpgファイルの幅、高さを取得する.
//=============================================================================
D3DXVECTOR2 LoadImageFile::GetJpgImageSize( const std::string& FilePath )
{
	SJpgData Image;

	// ファイルを開く.
	std::ifstream file( FilePath.data(), std::ios_base::binary );
	if ( !file.is_open() ) { return ERROR_SIZE; };

	// 不要なセグメント分移動する.
	int SeekPoint = SKIP_2BIT;
	std::uint8_t DQTMarker;
	do{
		SeekPoint += SKIP_2BIT;

		// セグメントの長さ(バイト数)の取得.
		std::uint16_t ByteNum = 0;
		file.seekg( SeekPoint );
		if ( !file.read( ( char * )&ByteNum, sizeof( ByteNum ) ) ) {
			return ERROR_SIZE;
		}

		/*
		*	260の場合以下のビットのようになる.
		*	0000 0100	| 0000 0001
		*	↑0 ~ 255(値),	↑256が何個か(べき乗),
		*	そのため8ビット右にシフトして値を取得.
		*	8ビット左にシフトして値を消してから8ビット右にシフトしてべき乗を取得.
		**/
		const int Value	= ByteNum >> _1BYTE_BIT_NUM;
		ByteNum			<<= _1BYTE_BIT_NUM;
		const int Pow	= ByteNum >> _1BYTE_BIT_NUM;

		// ファイルに応じてビット数を異なるため取得し移動数に加える.
		SeekPoint += ( _1BYTE_MAX_10BASE * Pow ) + Value;

		// whileを抜けるか調べる.
		const int& s = SeekPoint + 1;
		file.seekg( s );
		if ( !file.read( ( char * )&DQTMarker, sizeof( DQTMarker ) ) ) {
			return ERROR_SIZE;
		}
	} while ( DQTMarker != JPG_END_DQTMARKER );

	// 幅、高さの前にいらない情報があるため移動して読み込む.
	SeekPoint += SKIP_5BIT;
	file.seekg( SeekPoint );
	if ( !file.read( ( char * )&Image, sizeof( Image ) ) ) {
		return ERROR_SIZE;
	}

	/*
	*	260の場合以下のビットのようになる.
	*	0000 0100	| 0000 0001
	*	↑0 ~ 255(値),	↑256が何個か(べき乗),	
	*	そのため8ビット右にシフトして値を取得.
	*	8ビット左にシフトして値を消してから8ビット右にシフトしてべき乗を取得.
	**/
	// 幅.
	const int w_Value	= Image.Width >> _1BYTE_BIT_NUM;
	Image.Width			<<= _1BYTE_BIT_NUM;
	const int w_Pow		= Image.Width >> _1BYTE_BIT_NUM;
	// 高さ.
	const int h_Value	= Image.Height >> _1BYTE_BIT_NUM;
	Image.Height		<<= _1BYTE_BIT_NUM;
	const int h_Pow		= Image.Height >> _1BYTE_BIT_NUM;

	// ファイルを閉じる.
	file.close();

	return D3DXVECTOR2( 
		static_cast<float>( _1BYTE_MAX_10BASE * w_Pow + w_Value ),
		static_cast<float>( _1BYTE_MAX_10BASE * h_Pow + h_Value )
	);
}


//=============================================================================
//		pngファイルの幅、高さを取得する.
//=============================================================================
D3DXVECTOR2 LoadImageFile::GetPngImageSize( const std::string& FilePath )
{
	SData Image;

	// ファイルを開く.
	std::ifstream file( FilePath.data(), std::ios_base::binary );
	if ( !file.is_open() ) {
		return ERROR_SIZE;
	}

	// 幅、高さの前にいらない情報があるため移動して読み込む.
	file.seekg( SKIP_16BIT );
	if ( !file.read( ( char * )&Image, sizeof( Image ) ) ) {
		return ERROR_SIZE;
	}

	/*
	*	260の場合以下のビットのようになる.
	*	0000 0100	| 0000 0001				| 0000 0000 | 0000 0000.	
	*	↑0 ~ 255(値),	↑256が何個か(べき乗),		↑多分これ以降使わない.
	*	そのため24ビット右にシフトして値を取得.
	*	8ビット左にシフトして値を消してから24ビット右にシフトしてべき乗を取得.
	**/
	// 幅.
	const int w_Value	= Image.Width >> _3BYTE_BIT_NUM;
	Image.Width			<<= _1BYTE_BIT_NUM;
	const int w_Pow		= Image.Width >> _3BYTE_BIT_NUM;
	// 高さ.
	const int h_Value	= Image.Height >> _3BYTE_BIT_NUM;
	Image.Height		<<= _1BYTE_BIT_NUM;
	const int h_Pow		= Image.Height >> _3BYTE_BIT_NUM;

	return D3DXVECTOR2(
		static_cast<float>( _1BYTE_MAX_10BASE * w_Pow + w_Value ),
		static_cast<float>( _1BYTE_MAX_10BASE * h_Pow + h_Value )
	);
}


//=============================================================================
//		jpgファイルの幅、高さを取得する.
//=============================================================================
D3DXVECTOR2 LoadImageFile::GetBmpImageSize( const std::string& FilePath )
{
	SData Image;

	// ファイルを開く.
	std::ifstream file( FilePath.data(), std::ios_base::binary );
	if ( !file.is_open() ) { return ERROR_SIZE; }

	// 幅、高さの前にいらない情報分移動して読み込む.
	file.seekg( SKIP_18BIT );
	if ( !file.read( (char*) &Image, sizeof( Image ) ) ) {
		return ERROR_SIZE;
	}

	return D3DXVECTOR2( 
		static_cast<float>( Image.Width ),
		static_cast<float>( Image.Height )
	);
}
