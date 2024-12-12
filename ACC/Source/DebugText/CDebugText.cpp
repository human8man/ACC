#include "CDebugText.h"
#include "DirectX/CDirectX11.h"

//シェーダファイル名（ディレクトリも含む）.
const TCHAR SHADER_NAME[] = _T( "Data\\Shader\\DebugText.hlsl" );

//コンストラクタ.
CDebugText::CDebugText()
	: m_pDx11			( nullptr )
	, m_pDevice11		( nullptr )
	, m_pContext11		( nullptr )
	, m_pVertexShader	( nullptr )
	, m_pVertexLayout	( nullptr )
	, m_pPixelShader	( nullptr )
	, m_pConstantBuffer	( nullptr )
	, m_pVertexBuffer	()
	, m_pSampleLinear	( nullptr )
	, m_pTexture		( nullptr )
	, m_Alpha			( 1.0f )
	, m_Color			( 1.0f, 1.0f, 1.0f )
	, m_Kerning			()
{
}

//デストラクタ.
CDebugText::~CDebugText()
{
	//解放処理を呼び出す.
	Release();

	//別のところで管理しているのでここではnullptrを入れて初期化.
	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
	m_pDx11 = nullptr;
}

//初期化.
HRESULT CDebugText::Init()
{
	m_pDx11 = CDirectX11::GetInstance();
	m_pDevice11 = m_pDx11->GetDevice();		//実態は別のところにある.他とも共有している.
	m_pContext11 = m_pDx11->GetContext();	//実態は別のところにある.他とも共有している.

	//シェーダ作成.
	if( FAILED( CreateShader() ))
	{
		return E_FAIL;
	}
	//板ポリゴン作成.
	if( FAILED( CreateModel() ))
	{
		return E_FAIL;
	}
	//テクスチャ作成.
	if( FAILED( CreateTexture( _T("Data\\DebugText\\ascii.png") )))
	{
		return E_FAIL;
	}
	//サンプラ作成.
	if( FAILED( CreateSampler() ) )
	{
		return E_FAIL;
	}

	return S_OK;
}

//解放.
void CDebugText::Release()
{
	SAFE_RELEASE( m_pSampleLinear );
	SAFE_RELEASE( m_pTexture );
	for (int i = SPRITE_MAX - 1; i >= 0; i--) {
		SAFE_RELEASE( m_pVertexBuffer[i] );
	}
	SAFE_RELEASE( m_pConstantBuffer );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexShader );

	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
}

//===========================================================
//	HLSLファイルを読み込みシェーダを作成する.
//	HLSL: High Level Shading Language の略.
//===========================================================
HRESULT CDebugText::CreateShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif//#ifdef _DEBUG

	//HLSLからバーテックスシェーダのブロブを作成.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,	//シェーダファイル名（HLSLファイル）.
			nullptr,		//マクロ定義の配列へのポインタ（未使用）.
			nullptr,		//インクルードファイルを扱うインターフェイスへのポインタ（未使用）.
			"VS_Main",		//シェーダエントリーポイント関数の名前.
			"vs_5_0",		//シェーダのモデルを指定する文字列（プロファイル）.
			uCompileFlag,	//シェーダコンパイルフラグ.
			0,				//エフェクトコンパイルフラグ（未使用）.
			nullptr,		//スレッド ポンプ インターフェイスへのポインタ（未使用）.
			&pCompiledShader,//ブロブを格納するメモリへのポインタ.
			&pErrors,		//エラーと警告一覧を格納するメモリへのポインタ.
			nullptr)))		//戻り値へのポインタ（未使用）.
	{
		_ASSERT_EXPR( false, _T( "hlsl読み込み失敗" ) );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );

	//上記で作成したブロブから「バーテックスシェーダ」を作成.
	if (FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pVertexShader )))	//(out)バーテックスシェーダ.
	{
		_ASSERT_EXPR( false, _T( "バーテックスシェーダ作成失敗" ));
		return E_FAIL;
	}

	//頂点インプットレイアウトを定義.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",						//位置.
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,	//DXGIのフォーマット(32bit float型*3).
			0,
			0,								//データの開始位置.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD",						//テクスチャ位置.
			0,
			DXGI_FORMAT_R32G32_FLOAT,		//DXGIのフォーマット(32bit float型*2).
			0,
			12,								//データの開始位置.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	//頂点インプットレイアウトの配列要素数を算出.
	UINT numElements = sizeof( layout) / sizeof(layout[0] );

	//頂点インプットレイアウトを作成.
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout )))	//(out)頂点インプットレイアウト.
	{
		_ASSERT_EXPR( false, _T( "頂点インプットレイアウト作成失敗" ) );
		return E_FAIL;
	}
	SAFE_RELEASE( pCompiledShader );

	//HLSLからピクセルシェーダのブロブを作成.
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

	//上記で作成したブロブから「ピクセルシェーダ」を作成.
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

	//コンスタント（定数）バッファ作成.
	//シェーダに特定の数値を送るバッファ.
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		//コンスタントバッファを指定.
	cb.ByteWidth = sizeof(SHADER_CONSTANT_BUFFER);	//コンスタントバッファのサイズ.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		//書き込みでアクセス.
	cb.MiscFlags = 0;	//その他のフラグ（未使用）.
	cb.StructureByteStride = 0;	//構造体のサイズ（未使用）.
	cb.Usage = D3D11_USAGE_DYNAMIC;	//使用方法：直接書き込み.

	//コンスタントバッファの作成.
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


//モデル作成.
HRESULT CDebugText::CreateModel()
{
	//定数.
	constexpr float CHAR_W = 10.0f;		//１文字あたりの幅.
	constexpr float CHAR_H = 12.0f;		//１文字あたりの高さ.
	constexpr float TEXTURE_W = 128.0f;	//テクスチャの幅.
	constexpr float TEXTURE_H = 128.0f;	//テクスチャの高さ.

	//フォント毎に矩形作成.
	float left		= 0.0f;
	float top		= 0.0f;
	float right		= 0.0f;
	float bottom	= 0.0f;

	//作成済みカウンタ.
	int count = 0;

	//２重ループで１文字ずつ指定する.
	for (int x = 0; x < SPRITE_MAX_W; x++)
	{
		for (int y = 0; y < SPRITE_MAX_H; y++)
		{
			//カーニング(文字同士の間隔)の設定.
			m_Kerning[count] = 10.0f;	//文字ごとで調整可能.

			//１つあたりの大きさ.
			left	= x * CHAR_W;
			top		= y * CHAR_H;
			right	= left + m_Kerning[count];
			bottom	= top + CHAR_H;

			//テクスチャの大きさから比率計算.
			left	/= TEXTURE_W;
			top		/= TEXTURE_H;
			right	/= TEXTURE_W;
			bottom	/= TEXTURE_H;

			//板ポリ(四角形)の頂点を作成.
			VERTEX vertices[]=
			{
				//頂点座標(x,y,z)					 UV座標(u,v)
				D3DXVECTOR3(   0.0f, CHAR_H, 0.0f ), D3DXVECTOR2( left,  bottom ),	//頂点１(左下).
				D3DXVECTOR3(   0.0f,   0.0f, 0.0f ), D3DXVECTOR2( left,  top    ),	//頂点２(左上).
				D3DXVECTOR3( CHAR_W, CHAR_H, 0.0f ), D3DXVECTOR2( right, bottom ),	//頂点３(右下).
				D3DXVECTOR3( CHAR_W,   0.0f, 0.0f ), D3DXVECTOR2( right, top    )	//頂点４(右上).
			};
			//最大要素数を算出する.
			UINT uVerMax = sizeof( vertices ) / sizeof( vertices[0] );

			//バッファ構造体.
			D3D11_BUFFER_DESC bd;
			bd.Usage			= D3D11_USAGE_DEFAULT;		//使用方法（デフォルト）.
			bd.ByteWidth		= sizeof(VERTEX) * uVerMax;	//頂点のサイズ.
			bd.BindFlags		= D3D11_BIND_VERTEX_BUFFER;	//頂点バッファとして扱う.
			bd.CPUAccessFlags	= 0;	//CPUからはアクセスしない.
			bd.MiscFlags		= 0;	//その他のフラグ（未使用）.
			bd.StructureByteStride = 0;	//構造体のサイズ（未使用）.

			//サブリソースデータ構造体.
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = vertices;	//板ポリの頂点をセット.

			//頂点バッファの作成.
			if (FAILED(m_pDevice11->CreateBuffer(
				&bd, &InitData, &m_pVertexBuffer[count] )))
			{
				_ASSERT_EXPR( false, _T( "頂点バッファ作成失敗" ) );
				return E_FAIL;
			}

			count++;	//１文字作成したので次へ.
		}
	}

	return S_OK;
}

//テクスチャ作成.
HRESULT CDebugText::CreateTexture( LPCTSTR lpFileName )
{
	//テクスチャ作成.
	if( FAILED( D3DX11CreateShaderResourceViewFromFile(
		m_pDevice11,		//リソースを使用するデバイスのポインタ.
		lpFileName,			//ファイル名.
		nullptr, nullptr,
		&m_pTexture,		//(out)テクスチャ.
		nullptr ) ) )
	{
		_ASSERT_EXPR( false, _T( "テクスチャ作成失敗" ) );
		return E_FAIL;
	}

	return S_OK;
}


//サンプラ作成.
HRESULT CDebugText::CreateSampler()
{
	//テクスチャ用のサンプラ構造体.
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof( samDesc ) );
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//リニアフィルタ（線形補間）.
						//POINT:高速だが粗い.
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	//ラッピングモード（WRAP:繰り返し）.
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//MIRROR: 反転繰り返し.
	//CLAMP : 端の模様を引き伸ばす.
	//BORDER: 別途境界色を決める.

	//サンプラ作成.
	if( FAILED( m_pDevice11->CreateSamplerState(
		&samDesc, &m_pSampleLinear ) ) )//(out)サンプラ.
	{
		_ASSERT_EXPR( false, _T( "サンプラ作成失敗" ) );
		return E_FAIL;
	}

	return S_OK;
}

//フォントレンダリング.
void CDebugText::RenderFont( int FontIndex, float x, float y )
{
	//ワールド行列.
	D3DXMATRIX	mWorld;
	D3DXMATRIX	mTrans, mScale;

	//拡大縮小行列.
	D3DXMatrixScaling( &mScale, 1.0f, 1.0f, 1.0f );

	//平行行列（平行移動）.
	D3DXMatrixTranslation( &mTrans, x, y, 0.0f );

	//ワールド座標変換.
	//重要: 拡縮行列 * 平行行列.
	mWorld = mScale * mTrans;

	//シェーダのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	SHADER_CONSTANT_BUFFER cb;	//コンスタントバッファ.
	//バッファ内のデータの書き換え開始時にmap.
	if (SUCCEEDED(
		m_pContext11->Map( m_pConstantBuffer,
			0, D3D11_MAP_WRITE_DISCARD, 0, &pData )))
	{
		//ワールド行列を渡す.
		D3DXMATRIX m = mWorld;
		D3DXMatrixTranspose( &m, &m );	//行列を転置する.
		cb.mWorld = m;

		//カラー.
		cb.vColor = D3DXVECTOR4(
			m_Color.x, m_Color.y, m_Color.z, m_Alpha );

		//ビューポートの幅、高さを渡す.
		cb.fViewPortWidth = static_cast<float>( WND_W );
		cb.fViewPortHeight= static_cast<float>( WND_H );

		memcpy_s(pData.pData, pData.RowPitch,
			(void*)( &cb ), sizeof( cb ) );

		m_pContext11->Unmap(m_pConstantBuffer, 0);
	}

	//頂点バッファをセット.
	UINT stride = sizeof( VERTEX );	//データの間隔.
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1,
		&m_pVertexBuffer[FontIndex], &stride, &offset);

	//アルファブレンド有効にする.
	m_pDx11->SetAlphaBlend( true );

	//プリミティブをレンダリング.
	m_pContext11->Draw(4, 0);//板ポリ（頂点4つ分）.

	//アルファブレンド無効にする.
	m_pDx11->SetAlphaBlend( false );

}

//レンダリング用.
void CDebugText::Render( LPCTSTR text, int x, int y )
{
	//使用するシェーダの登録.
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );

	//このコンスタントバッファをどのシェーダで使うか？.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );

	//頂点インプットレイアウトをセット.
	m_pContext11->IASetInputLayout( m_pVertexLayout );
	//プリミティブ・トポロジーをセット.
	m_pContext11->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	//テクスチャをシェーダに渡す.
	m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinear );
	m_pContext11->PSSetShaderResources( 0, 1, &m_pTexture );

	float fx = static_cast<float>( x );
	float fy = static_cast<float>( y );

	//文字数分ループ.
	for( int i = 0; i < lstrlen( text ); i++ )
	{
		TCHAR font = text[i];
		int index = font - 32;	//フォントインデックス作成.

		//フォントレンダリング.
		RenderFont( index, fx, fy );

		fx += m_Kerning[index];
	}

}