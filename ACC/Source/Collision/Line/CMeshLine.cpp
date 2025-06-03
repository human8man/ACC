#include "CMeshLine.h"
#include "DirectX/CDirectX11.h"
#include "Mesh/Static/CStaticMeshObject.h"
#include "CGameObject.h"


// シェーダファイル名（ディレクトリも含む）.
const TCHAR SHADER_NAME[] = _T("Data\\Shader\\Ray.hlsl");


//============================================================================
//		レイクラス.
//============================================================================
CMeshLine::CMeshLine()
	: m_pDx11			( nullptr )
	, m_pDevice11		( nullptr )
	, m_pContext11		( nullptr )
	, m_pVertexShader	( nullptr )
	, m_pVertexLayout	( nullptr )
	, m_pPixelShader	( nullptr )
	, m_pConstantBuffer	( nullptr )
	, m_pVertexBuffer	( nullptr )
	, m_pIndexBuffer	()
	, m_vPosition		()
	, m_vRotation		()
{
}

CMeshLine::~CMeshLine()
{
	//解放処理を呼び出す.
	Release();

	//別のところで管理しているのでここではnullptrを入れて初期化.
	m_pContext11	= nullptr;
	m_pDevice11		= nullptr;
	m_pDx11			= nullptr;
}


//============================================================================
//		初期化.
//============================================================================
HRESULT CMeshLine::Init()
{
	m_pDx11 = CDirectX11::GetInstance();
	m_pDevice11 = m_pDx11->GetDevice();
	m_pContext11 = m_pDx11->GetContext();

	// シェーダ作成.
	if (FAILED(CreateShader())) { return E_FAIL; }

	return S_OK;
}


//============================================================================
//		解放.
//============================================================================
void CMeshLine::Release()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);

	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
}


//============================================================================
//		HLSLファイルを読み込みシェーダを作成する.
//============================================================================
HRESULT CMeshLine::CreateShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;

#ifdef _DEBUG
	uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	//---------------------------------------------------------------
	//	HLSLからバーテックスシェーダのブロブを作成.
	//---------------------------------------------------------------
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
		_ASSERT_EXPR(false, _T("hlsl読み込み失敗"));
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);


	//---------------------------------------------------------------
	//	上記で作成したブロブから「バーテックスシェーダ」を作成.
	//---------------------------------------------------------------
	if (FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pVertexShader)))	// (out)バーテックスシェーダ.
	{
		_ASSERT_EXPR(false, _T("バーテックスシェーダ作成失敗"));
		return E_FAIL;
	}


	//---------------------------------------------------------------
	//	頂点インプットレイアウトを定義.
	//---------------------------------------------------------------
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",						// 位置.
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,	// DXGIのフォーマット(32bit float型*3).
			0,
			0,								// データの開始位置.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	// 頂点インプットレイアウトの配列要素数を算出.
	UINT numElements = sizeof(layout) / sizeof(layout[0]);


	//---------------------------------------------------------------
	//	頂点インプットレイアウトを作成.
	//---------------------------------------------------------------
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout)))	// (out)頂点インプットレイアウト.
	{
		_ASSERT_EXPR(false, _T("頂点インプットレイアウト作成失敗"));
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);


	//---------------------------------------------------------------
	//	HLSLからピクセルシェーダのブロブを作成.
	//---------------------------------------------------------------
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,		// シェーダファイル名（HLSLファイル）.
			nullptr,			// マクロ定義の配列へのポインタ（未使用）.
			nullptr,			// インクルードファイルを扱うインターフェイスへのポインタ（未使用）.
			"PS_Main",			// シェーダエントリーポイント関数の名前.
			"ps_5_0",			// シェーダのモデルを指定する文字列（プロファイル）.
			uCompileFlag,		// シェーダコンパイルフラグ.
			0,					// エフェクトコンパイルフラグ（未使用）.
			nullptr,			// スレッド ポンプ インターフェイスへのポインタ（未使用）.
			&pCompiledShader,	// ブロブを格納するメモリへのポインタ.
			&pErrors,			// エラーと警告一覧を格納するメモリへのポインタ.
			nullptr)))			// 戻り値へのポインタ（未使用）.
	{
		_ASSERT_EXPR(false, _T("hlsl読み込み失敗"));
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);


	//---------------------------------------------------------------
	//	上記で作成したブロブから「ピクセルシェーダ」を作成.
	//---------------------------------------------------------------
	if (FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pPixelShader)))	// (out)ピクセルシェーダ.
	{
		_ASSERT_EXPR(false, _T("ピクセルシェーダ作成失敗"));
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);


	//---------------------------------------------------------------
	// コンスタント（定数）バッファ作成.
	// シェーダに特定の数値を送るバッファ.
	//---------------------------------------------------------------
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		// コンスタントバッファを指定.
	cb.ByteWidth = sizeof(SHADER_CONSTANT_BUFFER);	// コンスタントバッファのサイズ.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// 書き込みでアクセス.
	cb.MiscFlags = 0;								// その他のフラグ（未使用）.
	cb.StructureByteStride = 0;						// 構造体のサイズ（未使用）.
	cb.Usage = D3D11_USAGE_DYNAMIC;					// 使用方法：直接書き込み.


	//---------------------------------------------------------------
	//	コンスタントバッファの作成.
	//---------------------------------------------------------------
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&cb,
			nullptr,
			&m_pConstantBuffer)))
	{
		_ASSERT_EXPR(false, _T("コンスタントバッファ作成失敗"));
		return E_FAIL;
	}

	return S_OK;
}


//============================================================================
//		レンダリング用.
//============================================================================
void CMeshLine::Render(D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	// ワールド行列.
	D3DXMATRIX	mWorld;
	D3DXMATRIX	mTrans, mRot;

	// 回転行列.
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMatrixRotationY(&mYaw, m_vRotation.y);
	D3DXMatrixRotationX(&mPitch, m_vRotation.x);
	D3DXMatrixRotationZ(&mRoll, m_vRotation.z);
	mRot = mYaw * mPitch * mRoll;

	// 平行行列（平行移動）.
	D3DXMatrixTranslation(&mTrans, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// ワールド座標変換.
	mWorld = mRot * mTrans;

	// 使用するシェーダの登録.
	m_pContext11->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext11->PSSetShader(m_pPixelShader, nullptr, 0);

	// シェーダのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	SHADER_CONSTANT_BUFFER cb;


	//---------------------------------------------------------------
	//	バッファ内のデータの書き換え開始時にmap.
	//---------------------------------------------------------------
	if (SUCCEEDED(
		m_pContext11->Map(
			m_pConstantBuffer,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&pData)))
	{
		// ワールド,ビュー,プロジェクション行列を渡す.
		D3DXMATRIX m = mWorld * mView * mProj;
		D3DXMatrixTranspose(&m, &m);	// 行列を転置する.
		cb.mWVP = m;

		// カラー.
		cb.vColor = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		m_pContext11->Unmap(m_pConstantBuffer, 0);
	}

	// このコンスタントバッファをどのシェーダで使うか？.
	m_pContext11->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pContext11->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	// 頂点バッファをセット.
	UINT stride = sizeof(D3DXVECTOR3);	// データの間隔.
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// 頂点インプットレイアウトをセット.
	m_pContext11->IASetInputLayout(m_pVertexLayout);
	// プリミティブ・トポロジーをセット(LINELIST:頂点データを線の一覧として解釈).
	m_pContext11->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// プリミティブをレンダリング.
	// m_pContext11->Draw(2, 0);	// 元のコード (線分1つ)

	// メッシュの辺の数に基づいて描画する頂点数を計算
	UINT vertexCount = 0;
	if (!m_Vertices.empty())
	{
		vertexCount = static_cast<UINT>(m_Vertices.size());
		if (vertexCount % 3 == 0) // 三角形メッシュと仮定
		{
			vertexCount = vertexCount * 2; // 各辺に2つの頂点
		}
		else
		{
			// 三角形以外のポリゴンの場合は、辺の数を適切に計算する必要があります
			// ここでは単純に頂点数の2倍としていますが、正確ではありません
			// 正確な処理はメッシュの構造に依存します
			vertexCount = vertexCount * 2;
		}
	}
	m_pContext11->Draw(vertexCount, 0);
}


//=============================================================================
//		メッシュからワイヤーフレーム.
//=============================================================================
void CMeshLine::DrawMeshWireframeFromVertices(
	const CStaticMesh&	mesh,
	const CGameObject&	object,
	const D3DXMATRIX&	viewMatrix,
	const D3DXMATRIX&	projectionMatrix,
	const D3DXVECTOR4&	color,
	const float&		scale)
{
	// ワイヤーフレーム用ラスタライザーステート作成
	ID3D11RasterizerState* pWireRasterState = nullptr;
	ID3D11RasterizerState* pOldRasterState = nullptr;

	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;   // ← これが重要
	rasterDesc.CullMode = D3D11_CULL_BACK;        // 裏面カリング（必要ならD3D11_CULL_NONEでもOK）
	rasterDesc.DepthClipEnable = true;

	CDirectX11* directx11 = CDirectX11::GetInstance();
	auto* context = directx11->GetContext();
	auto* device = directx11->GetDevice();

	device->CreateRasterizerState(&rasterDesc, &pWireRasterState);

	// 現在の状態を取得して退避
	context->RSGetState(&pOldRasterState);

	// ワイヤーフレーム状態に切り替え
	context->RSSetState(pWireRasterState);

	//-----------------------------------------------------
	//		ワールド行列計算.
	//-----------------------------------------------------
	// ワールド変換行列.
	D3DXMATRIX mTrans, mRot, mScale;

	// 拡大縮小行列.
	D3DXMatrixScaling(&mScale,
		object.GetScale().x * scale,
		object.GetScale().y * scale,
		object.GetScale().z * scale);
	
	// 回転行列.
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMatrixRotationY(&mYaw,		object.GetRot().y);
	D3DXMatrixRotationX(&mPitch,	object.GetRot().x);
	D3DXMatrixRotationZ(&mRoll,		object.GetRot().z);
	mRot = mYaw * mPitch * mRoll;

	// 平行移動行列.
	D3DXMatrixTranslation(&mTrans,
		object.GetPos().x,
		object.GetPos().y,
		object.GetPos().z);

	// ワールド行列 = 拡大 * 回転 * 平行移動.
	D3DXMATRIX mWorld = mScale * mRot * mTrans;

	//-----------------------------------------------------
	//		メッシュの頂点を線分のリスト形式に変換.
	//-----------------------------------------------------
	m_pVertexBuffer = mesh.GetVertexBuffer();
	m_pIndexBuffer = mesh.GetIndexBuffer();
	m_Indices = mesh.GetIndex();

	// 使用するシェーダの登録.
	directx11->GetContext()->VSSetShader(m_pVertexShader, nullptr, 0);
	directx11->GetContext()->PSSetShader(m_pPixelShader, nullptr, 0);

	// コンスタントバッファの設定.
	D3D11_MAPPED_SUBRESOURCE pData;
	SHADER_CONSTANT_BUFFER cb;
	if (SUCCEEDED(
		directx11->GetContext()->Map(
			m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		D3DXMATRIX m = mWorld * viewMatrix * projectionMatrix;
		D3DXMatrixTranspose(&m, &m);
		cb.mWVP = m;
		cb.vColor = color;
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		directx11->GetContext()->Unmap(m_pConstantBuffer, 0);
		directx11->GetContext()->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
		directx11->GetContext()->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	}

	// ワイヤーフレーム用の頂点バッファをセット.
	UINT stride = mesh.GetMesh()->GetNumBytesPerVertex();
	UINT offset = 0;
	directx11->GetContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	

	// 頂点インプットレイアウトをセット.
	directx11->GetContext()->IASetInputLayout(mesh.GetVertexLayout());

	// プリミティブ・トポロジーをセット (LINELIST).
	directx11->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// プリミティブをレンダリング.
	for (size_t i = 0; i < m_pIndexBuffer.size();i++)
	{
		directx11->GetContext()->IASetIndexBuffer(m_pIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);
		directx11->GetContext()->DrawIndexed(m_Indices[i] * 3, 0, 0);
	}
	// 元のラスタライザーステートに戻す
	context->RSSetState(pOldRasterState);
}