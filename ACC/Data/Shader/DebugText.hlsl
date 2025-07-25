/*********************************************************************
*	デバッグテキストクラス用シェーダファイル.
**/
//グローバル変数
//テクスチャは レジスタ t(n)
Texture2D		g_Texture	: register( t0 );
//サンプラは レジスタ s(n)
SamplerState	g_samLinear	: register( s0 );

//コンスタントバッファ
//アプリ側と同じバッファサイズになっている必要がある
cbuffer per_mesh : register( b0 )	//レジスタ番号
{
	matrix	g_mWorld	: packoffset( c0 );	//ワールド行列
	float4	g_Color		: packoffset( c4 );	//色（RGBA:xyzw）
	float	g_ViewPortW	: packoffset( c5 );	//ビューポート幅
	float	g_ViewPortH	: packoffset( c6 );	//ビューポート高さ
};

//頂点シェーダの出力パラメータ
//頂点シェーダで制御した複数の値をピクセルシェーダ側に渡すために用意している
struct VS_OUTPUT
{
	float4	Pos		: SV_Position;	//座標（SV_:System-Value Semantics）
	float2	UV		: TEXCOORD0;	//UV座標
};

//頂点シェーダ
//主にモデルの頂点の表示位置を決定する
VS_OUTPUT VS_Main(
	float4 Pos	: POSITION,
	float2 UV	: TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul( Pos, g_mWorld );
	
	//スクリーン座標に合わせる計算
	output.Pos.x = ( output.Pos.x / g_ViewPortW ) * 2.0f - 1.0f;
	output.Pos.y = 1.0f - ( output.Pos.y / g_ViewPortH ) * 2.0f;
	
	output.UV = UV;

	return output;
}

//ピクセルシェーダ
//画面上に評されるピクセル（ドット1つ分）の色を決定する
float4 PS_Main( VS_OUTPUT input ) : SV_Target
{
	float4 color = g_Texture.Sample( g_samLinear, input.UV );//色を返す

	//プログラム制御の色(RGBA)を設定する
    color *= g_Color;

	return color;
}