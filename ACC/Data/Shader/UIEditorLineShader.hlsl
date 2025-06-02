cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float LineThickness;
	float3 padding; // 16バイトアラインメント用
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
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

[maxvertexcount(6)]
void GS_Main(line VS_OUTPUT input[2], inout TriangleStream<GS_OUTPUT> triStream)
{
	float2 p0 = input[0].pos.xy / input[0].pos.w;
	float2 p1 = input[1].pos.xy / input[1].pos.w;

	float2 dir = normalize(p1 - p0);
	float2 normal = float2(-dir.y, dir.x); // 外向き法線

	float2 offset = normal * (LineThickness * 0.5f);

	GS_OUTPUT verts[4];
	verts[0].pos = float4(p0 + offset, 0, 1);
	verts[1].pos = float4(p0 - offset, 0, 1);
	verts[2].pos = float4(p1 + offset, 0, 1);
	verts[3].pos = float4(p1 - offset, 0, 1);

	verts[0].color = input[0].color;
	verts[1].color = input[0].color;
	verts[2].color = input[1].color;
	verts[3].color = input[1].color;

	triStream.Append(verts[0]);
	triStream.Append(verts[1]);
	triStream.Append(verts[2]);

	triStream.Append(verts[2]);
	triStream.Append(verts[1]);
	triStream.Append(verts[3]);
}

float4 PS_Main(GS_OUTPUT input) : SV_TARGET
{
	return input.color;
}