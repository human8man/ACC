//=======================================================
//		レイ（線分）表示クラス用シェーダファイル
//=======================================================
// グローバル変数
// コンスタントバッファ
// アプリ側と同じバッファサイズになっている必要がある
cbuffer per_mesh : register( b0 )	//レジスタ番号
{
	matrix	g_mWVP;		// ワールド、ビュー、プロジェクションの変換合成行列
	float4	g_Color;	// 色（RGBA:xyzw）
}

// 頂点シェーダ
// 主にモデルの頂点の表示位置を決定する
float4 VS_Main( float4 Pos : POSITION ) : SV_Position
{
	Pos = mul( Pos, g_mWVP );
	
	return Pos;
}

// ピクセルシェーダ
// 画面上に評されるピクセル（ドット１つ分）の色を決定する
float4 PS_Main( float4 Pos : SV_Position ) : SV_Target
{
	return g_Color;
}