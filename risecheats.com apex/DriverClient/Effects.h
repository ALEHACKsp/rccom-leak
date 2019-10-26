#pragma once
const char VertexShader[] =
{
"struct VS_OUTPUT\n"
"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float4 Color : COLOR;\n"
"};\n"
"VS_OUTPUT VS(float4 inPos : POSITION, float4 inColor : COLOR)\n"
"{\n"
	"VS_OUTPUT output;\n"

	"output.Pos = inPos;\n"
	"output.Color = inColor;\n"

	"return output;\n"
"}\n"
};
const char PixelShader[] =
{
"struct VS_OUTPUT\n"
"{\n"
"float4 Pos : SV_POSITION;\n"
"float4 Color : COLOR;\n"
"};\n"
"float4 PS(VS_OUTPUT input) : SV_TARGET\n"
"{\n"
"return input.Color;\n"
"}\n"
};