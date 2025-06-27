#pragma once

// 警告についてのコード分析を無効にする.4005:再定義
#pragma warning(disable:4005)

// 前方宣言
class DirectX11;
class StaticMesh;
class GameObject;

// 頂点の構造体
struct VERTEX
{
	D3DXVECTOR3 Pos;
};

//============================================================================
//		メッシュの表示クラス
//============================================================================
class MeshLine
{
public:
	//------------------------------------------------------
	//	構造体
	//------------------------------------------------------
	// コンスタントバッファのアプリ側の定義
	//	※シェーダ内のコンスタントバッファと一致している必要あり
	struct SHADER_CONSTANT_BUFFER
	{
		D3DXMATRIX	mWVP;
		D3DXVECTOR4	vColor;
	};

public:
	MeshLine();
	~MeshLine();

	// 初期化
	HRESULT Init();

	// 解放
	void Release();

	// シェーダ作成
	HRESULT CreateShader();

	// レンダリング用
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj);


	/****************************************************************
	// @brief メッシュからワイヤーフレーム
	// @param mesh				メッシュ
	// @param object			オブジェクト
	// @param viewMatrix		ビュー行列
	// @param projectionMatrix	プロジェクション行列
	// @param color				色
	// @param scale				スケール
	****************************************************************/
	void DrawMeshWireframeFromVertices(
		const StaticMesh&	mesh,
		const GameObject&	object,
		const D3DXMATRIX&	viewMatrix,
		const D3DXMATRIX&	projectionMatrix,
		const D3DXVECTOR4&	color,
		const float&		scale);

private:
	DirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*	m_pVertexShader;	// 頂点シェーダ
	ID3D11InputLayout*	m_pVertexLayout;	// 頂点レイアウト
	ID3D11PixelShader*	m_pPixelShader;		// ピクセルシェーダ
	
	ID3D11Buffer*	m_pConstantBuffer;	// コンスタントバッファ
	ID3D11Buffer*	m_pVertexBuffer;	// 頂点バッファ
	std::vector<ID3D11Buffer*>	m_pIndexBuffer;	// インデックスバッファ
	std::vector<DWORD>			m_Indices;		// インデックス数

	std::vector<D3DXVECTOR3> m_Vertices;	// メッシュ頂点情報
	D3DXVECTOR3		m_vPosition;	// 座標
	D3DXVECTOR3		m_vRotation;	// 回転
};