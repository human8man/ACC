

// 頂点シェーダー
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

PS_INPUT VSMain(VS_INPUT input)
{
	PS_INPUT output;
	float4 worldPos = float4(input.pos, 1.0f);
	worldPos = mul(worldPos, worldMatrix);
	worldPos = mul(worldPos, viewMatrix);
	worldPos = mul(worldPos, projectionMatrix);

	output.pos = worldPos;
	output.color = input.color;
	return output;
}

// ピクセルシェーダー
float4 PSMain(PS_INPUT input) : SV_TARGET
{
	return input.color;
}