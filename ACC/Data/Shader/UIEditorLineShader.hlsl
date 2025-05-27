cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VS_INPUT
{
	float3 pos : POSITION; // ピクセル単位の座標 (例: (400, 300, 0))
	float4 color : COLOR; // 色
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION; // 出力ポジション
	float4 color : COLOR;
};

struct GS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
	VS_OUTPUT output;
	float4 worldPos = float4(input.pos, 1.0f);
	worldPos = mul(worldPos, worldMatrix);
	worldPos = mul(worldPos, viewMatrix);
	worldPos = mul(worldPos, projectionMatrix);

	output.pos = worldPos;
	output.color = input.color;
	return output;
}

[maxvertexcount(6)] // クアッド分 (2三角形)
void GS_Main(line VS_OUTPUT input[2], inout TriangleStream<GS_OUTPUT> triStream)
{
	float2 p0 = input[0].pos.xy;
	float2 p1 = input[1].pos.xy;

	float2 dir = normalize(p1 - p0);
	float2 normal = float2(-dir.y, dir.x);

	float thickness = 5.0f;

	float2 offset = normal * thickness * 0.5f;

	GS_OUTPUT v[4];
	v[0].pos = float4(p0 + offset, 0.0f, 1.0f);
	v[1].pos = float4(p0 - offset, 0.0f, 1.0f);
	v[2].pos = float4(p1 + offset, 0.0f, 1.0f);
	v[3].pos = float4(p1 - offset, 0.0f, 1.0f);

	v[0].color = input[0].color;
	v[1].color = input[0].color;
	v[2].color = input[1].color;
	v[3].color = input[1].color;

	triStream.Append(v[0]);
	triStream.Append(v[1]);
	triStream.Append(v[2]);

	triStream.Append(v[2]);
	triStream.Append(v[1]);
	triStream.Append(v[3]);
}

float4 PS_Main(GS_OUTPUT input) : SV_TARGET
{
	return input.color;
}