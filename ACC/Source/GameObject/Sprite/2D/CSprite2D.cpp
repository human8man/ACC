#include "CSprite2D.h"
#include "DirectX/CDirectX11.h"
#include "FileManager/LoadImage/LoadImage.h"
#include "FileManager/FileManager.h"
#include <string>
#include <locale>


// シェーダファイル名（ディレクトリも含む）.
const TCHAR SHADER_NAME[] = _T( "Data\\Shader\\Sprite2D.hlsl" );


//=============================================================================
//		スプライト2Dクラス.
//=============================================================================
CSprite2D::CSprite2D()
	: m_pDx11			( nullptr )
	, m_pDevice11		( nullptr )
	, m_pContext11		( nullptr )
	, m_pVertexShader	( nullptr )
	, m_pVertexLayout	( nullptr )
	, m_pPixelShader	( nullptr )
	, m_pConstantBuffer	( nullptr )
	, m_pVertexBuffer	( nullptr )
	, m_pSampleLinear	( nullptr )
	, m_pTexture		( nullptr )
	, m_vPosition		()
	, m_vRotation		()
	, m_vScale			( 1.f, 1.f, 1.f )
	, m_UV				( ZEROVEC2 )
	, m_Alpha			( 1.f )
	, m_SpriteState		()
	, m_PatternNo		()
	, m_PatternMax		()
{
}
CSprite2D::~CSprite2D()
{
	// 解放処理を呼び出す.
	Release();

	// 別のところで管理しているのでここではnullptrを入れて初期化.
	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
	m_pDx11 = nullptr;
}


//=============================================================================
//		初期化処理.
//=============================================================================
HRESULT CSprite2D::Init(const std::string& lpFileName)
{
	m_pDx11 = CDirectX11::GetInstance();
	m_pDevice11 = m_pDx11->GetDevice();
	m_pContext11 = m_pDx11->GetContext();

	// スプライト情報の取得.
	if (FAILED(SpriteStateDataLoad( lpFileName ))) return E_FAIL;

	// シェーダ作成.
	if (FAILED(CreateShader())) { return E_FAIL; }
	// 板ポリゴン作成.
	if( FAILED( CreateModel() )){ return E_FAIL; }
	// テクスチャ作成(パスの型を変換後).
	std::wstring wideStr;
	int bufferSize = MultiByteToWideChar(CP_UTF8, 0, lpFileName.c_str(), -1, nullptr, 0);
	 if (bufferSize <= 0) { return E_FAIL; }
	wideStr.resize(bufferSize);
	MultiByteToWideChar(CP_UTF8, 0, lpFileName.c_str(), -1, &wideStr[0], bufferSize);


	if( FAILED( CreateTexture( wideStr.c_str() ) ) ){ return E_FAIL; }
	// サンプラ作成.
	if( FAILED( CreateSampler() ) ){ return E_FAIL; }

	return S_OK;
}


//=============================================================================
//		解放処理.
//=============================================================================
void CSprite2D::Release()
{
	SAFE_RELEASE( m_pSampleLinear );
	SAFE_RELEASE( m_pTexture );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pConstantBuffer );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexShader );

	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
}


//=============================================================================
//		HLSLファイルを読み込みシェーダを作成する.
//=============================================================================
HRESULT CSprite2D::CreateShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;

#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	// HLSLからバーテックスシェーダのブロブを作成.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,		// シェーダファイル名（HLSLファイル）.
			nullptr,			// マクロ定義の配列へのポインタ（未使用）.
			nullptr,			// インクルードファイルを扱うインターフェイスへのポインタ（未使用）.
			"VS_Main",			// シェーダエントリーポイント関数の名前.
			"vs_5_0",			// シェーダのモデルを指定する文字列（プロファイル）.
			uCompileFlag,		// シェーダコンパイルフラグ.
			0,					// エフェクトコンパイルフラグ（未使用）.
			nullptr,			// スレッド ポンプ インターフェイスへのポインタ（未使用）.
			&pCompiledShader,	// ブロブを格納するメモリへのポインタ.
			&pErrors,			// エラーと警告一覧を格納するメモリへのポインタ.
			nullptr)))			// 戻り値へのポインタ（未使用）.
	{
		_ASSERT_EXPR( false, _T( "hlsl読み込み失敗" ) );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );


	// 上記で作成したブロブから「バーテックスシェーダ」を作成.
	if (FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pVertexShader )))	// (out)バーテックスシェーダ.
	{
		_ASSERT_EXPR( false, _T( "バーテックスシェーダ作成失敗" ));
		return E_FAIL;
	}


	// 頂点インプットレイアウトを定義.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",						// 位置.
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,	// DXGIのフォーマット(32bit float型*3).
			0,
			0,								// データの開始位置.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD",						// テクスチャ位置.
			0,
			DXGI_FORMAT_R32G32_FLOAT,		// DXGIのフォーマット(32bit float型*2).
			0,
			12,								// データの開始位置.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};

	// 頂点インプットレイアウトの配列要素数を算出.
	UINT numElements = sizeof( layout) / sizeof(layout[0] );


	// 頂点インプットレイアウトを作成.
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout )))	// (out)頂点インプットレイアウト.
	{
		_ASSERT_EXPR( false, _T( "頂点インプットレイアウト作成失敗" ) );
		return E_FAIL;
	}
	SAFE_RELEASE( pCompiledShader );


	// HLSLからピクセルシェーダのブロブを作成.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,		//シェーダファイル名（HLSLファイル）.
			nullptr,			//マクロ定義の配列へのポインタ（未使用）.
			nullptr,			//インクルードファイルを扱うインターフェイスへのポインタ（未使用）.
			"PS_Main",			//シェーダエントリーポイント関数の名前.
			"ps_5_0",			//シェーダのモデルを指定する文字列（プロファイル）.
			uCompileFlag,		//シェーダコンパイルフラグ.
			0,					//エフェクトコンパイルフラグ（未使用）.
			nullptr,			//スレッド ポンプ インターフェイスへのポインタ（未使用）.
			&pCompiledShader,	//ブロブを格納するメモリへのポインタ.
			&pErrors,			//エラーと警告一覧を格納するメモリへのポインタ.
			nullptr )))			//戻り値へのポインタ（未使用）.
	{
		_ASSERT_EXPR( false, _T( "hlsl読み込み失敗" ) );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );


	// 上記で作成したブロブから「ピクセルシェーダ」を作成.
	if (FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pPixelShader )))	//(out)ピクセルシェーダ.
	{
		_ASSERT_EXPR( false, _T( "ピクセルシェーダ作成失敗" ) );
		return E_FAIL;
	}
	SAFE_RELEASE( pCompiledShader );


	// コンスタント（定数）バッファ作成.
	// シェーダに特定の数値を送るバッファ.
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		// コンスタントバッファを指定.
	cb.ByteWidth = sizeof(SHADER_CONSTANT_BUFFER);	// コンスタントバッファのサイズ.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// 書き込みでアクセス.
	cb.MiscFlags = 0;								// その他のフラグ（未使用）.
	cb.StructureByteStride = 0;						// 構造体のサイズ（未使用）.
	cb.Usage = D3D11_USAGE_DYNAMIC;					// 使用方法：直接書き込み.

	// コンスタントバッファの作成.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&cb,
			nullptr,
			&m_pConstantBuffer )))
	{
		_ASSERT_EXPR( false, _T( "コンスタントバッファ作成失敗" ) );
		return E_FAIL;
	}

	return S_OK;
}


//=============================================================================
//		モデル作成.
//=============================================================================
HRESULT CSprite2D::CreateModel()
{
	float w = m_SpriteState.Disp.w;
	float h = m_SpriteState.Disp.h;

	float u = m_SpriteState.Stride.w / m_SpriteState.Base.w; // １マスあたりの幅.
	float v = m_SpriteState.Stride.h / m_SpriteState.Base.h; // １マスあたりの高さ.

	// x,yそれぞれの最大マス数.
	m_PatternMax.x = static_cast<SHORT>( m_SpriteState.Base.w / m_SpriteState.Stride.w );
	m_PatternMax.y = static_cast<SHORT>( m_SpriteState.Base.h / m_SpriteState.Stride.h );

	// 板ポリ(四角形)の頂点を作成.
	VERTEX vertices[]=
	{
		// 頂点座標(x,y,z)、UV座標(u,v)
		D3DXVECTOR3( 0.f,   h, 0.f ), D3DXVECTOR2( 0.f,   v ),	// 頂点１(左下).
		D3DXVECTOR3( 0.f, 0.f, 0.f ), D3DXVECTOR2( 0.f, 0.f ),	// 頂点２(左上).
		D3DXVECTOR3(   w,   h, 0.f ), D3DXVECTOR2(   u,   v ),	// 頂点３(右下).
		D3DXVECTOR3(   w, 0.f, 0.f ), D3DXVECTOR2(   u, 0.f )	// 頂点４(右上).
	};
	// 最大要素数を算出する.
	UINT uVerMax = sizeof( vertices ) / sizeof( vertices[0] );

	// バッファ構造体.
	D3D11_BUFFER_DESC bd;
	bd.Usage			= D3D11_USAGE_DEFAULT;		// 使用方法（デフォルト）.
	bd.ByteWidth		= sizeof(VERTEX) * uVerMax;	// 頂点のサイズ.
	bd.BindFlags		= D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとして扱う.
	bd.CPUAccessFlags	= 0;	// CPUからはアクセスしない.
	bd.MiscFlags		= 0;	// その他のフラグ（未使用）.
	bd.StructureByteStride = 0;	// 構造体のサイズ（未使用）.

	// サブリソースデータ構造体.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices; // 板ポリの頂点をセット.

	// 頂点バッファの作成.
	if ( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, &m_pVertexBuffer )))
	{
		_ASSERT_EXPR( false, _T( "頂点バッファ作成失敗" ) );
		return E_FAIL;
	}

	return S_OK;
}


//=============================================================================
//		テクスチャ作成.
//=============================================================================
HRESULT CSprite2D::CreateTexture( LPCTSTR lpFileName )
{
	// テクスチャ作成.
	if( FAILED( D3DX11CreateShaderResourceViewFromFile(
		m_pDevice11,		// リソースを使用するデバイスのポインタ.
		lpFileName,
		nullptr, nullptr,
		&m_pTexture,		// (out)テクスチャ.
		nullptr ) ) )
	{
		_ASSERT_EXPR( false, _T( "テクスチャ作成失敗" ) );
		return E_FAIL;
	}

	return S_OK;
}


//=============================================================================
//		サンプラ作成.
//=============================================================================
HRESULT CSprite2D::CreateSampler()
{
	// テクスチャ用のサンプラ構造体.
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof( samDesc ) );
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;// リニアフィルタ（線形補間）.
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	// ラッピングモード（WRAP:繰り返し）.
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	// MIRROR: 反転繰り返し.
	// CLAMP : 端の模様を引き伸ばす.
	// BORDER: 別途境界色を決める.

	// サンプラ作成.
	if( FAILED( m_pDevice11->CreateSamplerState(
		&samDesc, &m_pSampleLinear ) ) )// (out)サンプラ.
	{
		_ASSERT_EXPR( false, _T( "サンプラ作成失敗" ) );
		return E_FAIL;
	}

	return S_OK;
}


//=============================================================================
//		レンダリング用.
//=============================================================================
void CSprite2D::Render()
{
	// ワールド行列.
	D3DXMATRIX	mWorld;
	D3DXMATRIX	mTrans, mRot, mScale;

	// 拡大縮小行列.
	D3DXMatrixScaling( &mScale, m_vScale.x, m_vScale.y, m_vScale.z );

	// 回転行列.
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMatrixRotationY( &mYaw,		m_vRotation.y );
	D3DXMatrixRotationX( &mPitch,	m_vRotation.x );
	D3DXMatrixRotationZ( &mRoll,	m_vRotation.z );
	mRot = mYaw * mPitch * mRoll;
	// ※Yaw, Pitch, Roll の掛ける順番を変えると結果も変わる.

	// 平行行列（平行移動）.
	D3DXMatrixTranslation(&mTrans, m_vPosition.x, m_vPosition.y, m_vPosition.z );

	// ワールド座標変換.
	// 重要: 拡縮行列 * 回転行列 * 平行行列.
	mWorld = mScale * mRot * mTrans;


	m_pContext11 = CDirectX11::GetInstance()->GetContext();

	// 使用するシェーダの登録.
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );

	// シェーダのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	SHADER_CONSTANT_BUFFER cb;	// コンスタントバッファ.

	// バッファ内のデータの書き換え開始時にmap.
	if (SUCCEEDED( m_pContext11->Map( m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData )))
	{
		// ワールド行列を渡す.
		D3DXMATRIX m = mWorld;
		D3DXMatrixTranspose( &m, &m );	// 行列を転置する.
		cb.mWorld = m;

		// カラー.
		cb.vColor = D3DXVECTOR4( 1.f, 1.f, 1.f, m_Alpha );

		// テクスチャ座標.
		//	１マスあたりの割合にパターン番号（マス目）をかけて座標を設定する.
		cb.vUV.x = m_SpriteState.Stride.w / m_SpriteState.Base.w
			* static_cast<float>( m_PatternNo.x );
		cb.vUV.y = m_SpriteState.Stride.h / m_SpriteState.Base.h
			* static_cast<float>( m_PatternNo.y );

		// ビューポートの幅、高さを渡す.
		cb.fViewPortWidth = static_cast<float>( WND_W );
		cb.fViewPortHeight= static_cast<float>( WND_H );

		memcpy_s(pData.pData, pData.RowPitch, (void*)( &cb ), sizeof( cb ) );

		m_pContext11->Unmap(m_pConstantBuffer, 0);
	}

	// このコンスタントバッファをどのシェーダで使うか？.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );

	// 頂点バッファをセット.
	UINT stride = sizeof( VERTEX );	// データの間隔.
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

	// 頂点インプットレイアウトをセット.
	m_pContext11->IASetInputLayout( m_pVertexLayout );
	// プリミティブ・トポロジーをセット.
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// テクスチャをシェーダに渡す.
	m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinear );
	m_pContext11->PSSetShaderResources( 0, 1, &m_pTexture );

	// アルファブレンド有効にする.
	m_pDx11->SetAlphaBlend( true );

	// プリミティブをレンダリング.
	m_pContext11->Draw(4, 0); // 板ポリ（頂点4つ分）.

	// アルファブレンド無効にする.
	m_pDx11->SetAlphaBlend( false );

}


//=============================================================================
//		スプライト情報の読込.
//=============================================================================
HRESULT CSprite2D::SpriteStateDataLoad(const std::string& FilePath)
{
	Json	m_SpriteStateData = nullptr;	// スプライト情報データ.

	// 補正値テキストの読み込み.
	std::string TextPath = FilePath;
	TextPath.erase(TextPath.rfind("\\"), TextPath.size()) += "\\OffSet.json";
	Json OffSetData = FileManager::JsonLoad(TextPath);

	// 同じ名前のテキストの読み込み.
	TextPath = FilePath;
	TextPath.erase(TextPath.find("."), TextPath.size()) += ".json";
	m_SpriteStateData = FileManager::JsonLoad(TextPath);

	// ファイルが無いためファイルを作成する.
	if (m_SpriteStateData.empty()) {
		if (FAILED(CreateSpriteState(FilePath))) return E_FAIL;

		// 作成できたためもう一度読み込み直す.
		SpriteStateDataLoad(FilePath);
		return S_OK;
	}
	
	// スプライト情報の取得.
	m_SpriteState.Pos.x = m_SpriteStateData["Pos"]["x"].get<float>();
	m_SpriteState.Pos.y = m_SpriteStateData["Pos"]["y"].get<float>();
	m_SpriteState.Pos.z = m_SpriteStateData["Pos"]["z"].get<float>();
	m_SpriteState.Disp.w = m_SpriteStateData["Disp"]["w"];
	m_SpriteState.Disp.h = m_SpriteStateData["Disp"]["h"];
	m_SpriteState.Base.w = m_SpriteStateData["Base"]["w"];
	m_SpriteState.Base.h = m_SpriteStateData["Base"]["h"];
	m_SpriteState.Stride.w = m_SpriteStateData["Stride"]["w"];
	m_SpriteState.Stride.h = m_SpriteStateData["Stride"]["h"];

#if _DEBUG
	// ファイルパスを更新する.
	m_SpriteStateData["FilePath"] = TextPath;
	if (FAILED(FileManager::JsonSave(TextPath, m_SpriteStateData))) return E_FAIL;
#endif	// #if _DEBUG.

	return S_OK;
}


//=============================================================================
//		スプライト情報をまとめたjsonファイルの作成.
//=============================================================================
HRESULT CSprite2D::CreateSpriteState(const std::string& FilePath)
{
	// 作成するファイルパス.
	std::string TextPath = FilePath;
	TextPath.erase(TextPath.find("."), TextPath.size()) += ".json";

	// 画像の幅、高さの取得.
	const D3DXVECTOR2& BaseSize = LoadImageFile::GetImageSize(FilePath);

	// 情報を追加していく.
	Json SpriteState;
	SpriteState["Pos"]["x"] = 0.0;
	SpriteState["Pos"]["y"] = 0.0;
	SpriteState["Pos"]["z"] = 0.0;
	SpriteState["Disp"]["w"] = BaseSize.x;
	SpriteState["Disp"]["h"] = BaseSize.y;
	SpriteState["Base"]["w"] = BaseSize.x;
	SpriteState["Base"]["h"] = BaseSize.y;
	SpriteState["Stride"]["w"] = BaseSize.x;
	SpriteState["Stride"]["h"] = BaseSize.y;

	// スプライト情報の作成.
	if (FAILED(FileManager::JsonSave(TextPath, SpriteState))) return E_FAIL;

	// 作成成功.
	return S_OK;
}