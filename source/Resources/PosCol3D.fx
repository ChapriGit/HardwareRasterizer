// =========================
// Global variables
// =========================

float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WORLD;
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;
float3 gCameraPosition : CAMERA;

static const float3 gLightDirection : LightDirection = {0.577f, -0.577f, 0.577f};
static const float PI = 3.14159265f;
static const float gLightIntensity = 7.0f;
static const float gShininessIntensity = 25.0f;

SamplerState samplerState : register(s0);
// ==========================
// Input and Output Structs
// ==========================

struct VS_INPUT {
    float3 Position: POSITION;
    float2 Uv: TEXCOORD;
    float3 Normal: NORMAL;
    float3 Tangent: TANGENT;
};

struct VS_OUTPUT {
    float4 Position: SV_POSITION0;
    float4 WorldPosition : TEXCOORD0;
    float2 Uv: TEXCOORD1;
    float3 Normal: NORMAL;
    float3 Tangent: TANGENT;
};

// ==========================
// Vertex Shader
// ==========================

VS_OUTPUT VS(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.WorldPosition = mul(float4(input.Position, 1.f), gWorldMatrix);
    output.Uv = input.Uv;
    output.Normal = mul(normalize(input.Normal), (float3x3) gWorldMatrix);
    output.Tangent = mul(normalize(input.Tangent), (float3x3) gWorldMatrix);

    return output;
}

// ==========================
// Pixel Shader
// ==========================
float4 PS(VS_OUTPUT input) : SV_TARGET {
    float3 invViewDirection = normalize(gCameraPosition - input.WorldPosition.xyz);
    return gDiffuseMap.Sample(samplerState, input.Uv);
}

// ==========================
// Technique - Actual shader, particular to the Effects Framework
// Sets the functions for each stage. Can have multiple passes.
// ==========================
technique11 DefaultTechnique {
    pass P0 {
        SetVertexShader( CompileShader(vs_5_0, VS()) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(ps_5_0, PS()) );
    }
}

