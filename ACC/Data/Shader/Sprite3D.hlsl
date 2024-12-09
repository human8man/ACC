/*********************************************************************
*	スプライト3Dクラス用シェーダファイル.
**/
//グローバル変数.
//テクスチャは レジスタ t(n).
Texture2D		g_Texture	: register( t0 );
//サンプラは レジスタ s(n).
SamplerState	g_samLinear	: register( s0 );

//コンスタントバッファ.
//アプリ側と同じバッファサイズになっている必要がある.
cbuffer per_mesh : register( b0 )	//レジスタ番号.
{
	matrix	g_mWVP;		//ワールド,ビュー,プロジェクションの変換合成行列.
	float4	g_Color;	//色（RGBA:xyzw）.
	float4	g_UV;		//UV座標（xyしか使わない）.
};

//頂点シェーダの出力パラメータ.
//頂点シェーダで制御した複数の値をピクセルシェーダ側に渡すために用意している.
struct VS_OUTPUT
{
	float4	Pos		: SV_Position;	//座標（SV_:System-Value Semantics）.
	float2	UV		: TEXCOORD0;	//UV座標.
};

//頂点シェーダ.
//主にモデルの頂点の表示位置を決定する.
VS_OUTPUT VS_Main(
	float4 Pos	: POSITION,
	float2 UV	: TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul( Pos, g_mWVP );
	output.UV = UV;
	
	//UVスクロール(UV座標を操作する).
    output.UV.x += g_UV.x;
    output.UV.y += g_UV.y;

	return output;
}

//ピクセルシェーダ.
//画面上に評されるピクセル（ドット1つ分）の色を決定する.
float4 PS_Main( VS_OUTPUT input ) : SV_Target
{
	float4 color = g_Texture.Sample( g_samLinear, input.UV );//色を返す.

	//プログラム制御のα値を設定する.
    color.a *= g_Color.a;

	return color;
}