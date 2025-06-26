/***************************************************************************************************
*	SkinMeshCode Version 2.40
*	LastUpdate	: 2024/06/14.
**/
//------------------------------------------------
//	定数
//------------------------------------------------
static const int MAX_BONE_MATRICES = 255;

//------------------------------------------------
//	グローバル変数
//------------------------------------------------
Texture2D		g_Texture	: register( t0 );	//テクスチャーは、レジスターt(n)
SamplerState	g_Sampler	: register( s0 );	//サンプラーは、レジスターs(n)

//------------------------------------------------
//	コンスタントバッファ
//------------------------------------------------
//メッシュ
cbuffer per_mesh		: register( b0 )
{
	float4x4 g_mW;		//ワールド行列
	float4x4 g_mWVP;	//ワールド,ビュー,プロジェクションの合成行列
};
//マテリアル
cbuffer per_material	: register( b1 )
{
	float4 g_Ambient;	//アンビエント色(環境色)
	float4 g_Diffuse;	//ディフューズ色(拡散反射色)
	float4 g_Specular;	//スペキュラ色(鏡面反射色)
};
//フレーム
cbuffer per_frame		: register( b2 )
{
    float4 g_CameraPos; //カメラ位置(始点位置)
    float4 g_vLightDir;  //ライトの方向ベクトル
};
//ボーンのポーズ行列が入る
cbuffer per_bones		: register( b3 )
{
	float4x4 g_mConstBoneWorld[MAX_BONE_MATRICES];
};

//スキニング後の頂点・法線が入る
struct Skin
{
	float4 Pos;
	float4 Norm;
};
//バーテックスバッファーの入力
struct VSSkinIn
{
	float3 Pos		: POSITION;		//位置.  
	float3 Norm		: NORMAL;		//頂点法線
	float2 UV		: TEXCOORD;		//UV座標（テクスチャー座標）
	uint4  Bones	: BONE_INDEX;	//ボーンのインデックス
	float4 Weights	: BONE_WEIGHT;	//ボーンの重み
};

//ピクセルシェーダーの入力（バーテックスバッファーの出力）　
struct PSSkinIn
{
	float4 Pos		: SV_Position;	//位置
	float3 Norm		: TEXCOORD0;	//頂点法線
	float2 UV		: TEXCOORD1;	//UV座標（テクスチャー座標）
	float3 Light	: TEXCOORD2;	//ライト
	float3 EyeVector: TEXCOORD3;	//視線ベクトル
	float4 PosWorld : TEXCOORD4;	//ワールド座標
	float4 Color	: COLOR;		//最終カラー（頂点シェーダーにおいての）
};


//指定した番号のボーンのポーズ行列を返す
//	サブ関数（バーテックスシェーダーで使用）
matrix FetchBoneMatrix( uint Bone )
{
	return g_mConstBoneWorld[Bone];
}


//頂点をスキニング（ボーンにより移動）する
//	サブ関数（バーテックスシェーダーで使用）
Skin SkinVert( VSSkinIn Input )
{
	Skin Output = (Skin)0;

	float4 Pos	= float4( Input.Pos, 1.f );		//座標のwは 1
	float4 Norm = float4( Input.Norm, 0.f );	//ベクトルのwは 0
	//ボーン0
	uint Bone	= Input.Bones.x;
	float Weight= Input.Weights.x;
	float4x4 m	= FetchBoneMatrix( Bone );
	Output.Pos	+= Weight * mul( Pos, m );
	Output.Norm += Weight * mul( Norm, m );
	
	//ボーン1
	Bone		= Input.Bones.y;
	Weight		= Input.Weights.y;
	m			= FetchBoneMatrix( Bone );
	Output.Pos	+= Weight * mul( Pos, m );
	Output.Norm += Weight * mul( Norm, m );
	
	//ボーン2
	Bone		= Input.Bones.z;
	Weight		= Input.Weights.z;
	m			= FetchBoneMatrix( Bone );
	Output.Pos	+= Weight * mul( Pos, m );
	Output.Norm += Weight * mul( Norm,m );
	
	//ボーン3
	Bone		= Input.Bones.w;
	Weight		= Input.Weights.w;
	m			= FetchBoneMatrix( Bone );
	Output.Pos	+= Weight * mul( Pos, m );
	Output.Norm += Weight * mul( Norm, m );

	return Output;
}

// バーテックスシェーダ
PSSkinIn VS_Main( VSSkinIn input )
{
	PSSkinIn output = (PSSkinIn)0;
	
	Skin vSkinned = SkinVert( input );

	//プロジェクション変換（ワールド、ビュー、プロジェクション）
	output.Pos	= mul( vSkinned.Pos, g_mWVP );
	
	//法線をモデルの姿勢に合わせる
	// (モデルが回転すれば法線も回転させる必要があるため)
	output.Norm = normalize( (float3)mul( vSkinned.Norm, g_mW ) );
	
	//ライト方向:
	// ディレクショナルライトは、どこでも同じ方向.位置は無関係
	output.Light = normalize( g_vLightDir.xyz );
	
	//ワールド座標
	output.PosWorld = mul( float4( input.Pos, 1.f ), g_mW );
	
	//視線ベクトル:
	// ワールド空間上での頂点から頂点へ向かうベクトル
	output.EyeVector = normalize( g_CameraPos.xyz - output.PosWorld.xyz );	//w以外で計算

	//UV座標（テクスチャ座標）.	
	output.UV	= input.UV;

	return output;
}

// ピクセルシェーダ
float4 PS_Main( PSSkinIn input ) : SV_Target
{
	//テクスチャからピクセル色を取り出す
	float4 texColor = g_Texture.Sample( g_Sampler, input.UV );

	//環境光 ※１
	float4 ambient = texColor * g_Ambient;

	//拡散反射光 ※２
	float NL = saturate( dot( input.Norm.xyz, input.Light.xyz ) );
	float4 diffuse = ( g_Diffuse * 0.5f + texColor * 0.5f ) * NL;

	//鏡面反射光 ※３
	float3 reflect = normalize( 2.f * NL * input.Norm.xyz - input.Light.xyz );
	float4 specular =
		pow( saturate( dot( reflect, input.EyeVector.xyz ) ), 4.f ) * g_Specular;

	//最終色　※１，２，３の合計
	float4 Color = ambient + diffuse + specular;
	
	// テクスチャのα値をそのまま使用
	Color.a = texColor.a;
	
	return Color;
}
