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

static const float PI = 3.14159265f;

static const float3 gLightDirection : LightDirection = {0.577f, -0.577f, 0.577f};
static const float gLightIntensity = 7.0f;
static const float gShininessIntensity = 25.0f;
static const float4 gAmbientColor = {.03f, .03f, .03f, 1.f};

bool gNormalMappingEnabled : NormalMappingEnabled = true;

SamplerState samplerState : register(s0);
// ==========================
// Input and Output Structs
// ==========================

struct VS_INPUT {
    float3 Position: POSITION;
    float2 TexCoord: TEXCOORD;
    float3 Normal: NORMAL;
    float3 Tangent: TANGENT;
};

struct VS_OUTPUT {
    float4 Position: SV_POSITION0;
    float4 WorldPosition : TEXCOORD0;
    float2 TexCoord: TEXCOORD1;
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
    output.TexCoord = input.TexCoord;
    output.Normal = mul(normalize(input.Normal), (float3x3) gWorldMatrix);
    output.Tangent = mul(normalize(input.Tangent), (float3x3) gWorldMatrix);
    return output;
}

// ==========================
// Pixel Shader
// ==========================
float3 CalculateNormal(float3 normal, float3 tangent, float2 texCoord) {
    float3 binormal = cross(normal, tangent);
    float3x3 tangentSpaceAxis = {tangent, binormal, normal};

    float3 normalSample = gNormalMap.Sample(samplerState, texCoord).xyz;
    normalSample = 2.f * normalSample - 1.f;

    float3 normalOut = mul(normalSample, tangentSpaceAxis);

    return normalize(normalOut);
}

float4 GetDiffuseColor(float2 texCoord) {
    float4 color = gDiffuseMap.Sample(samplerState, texCoord);
    return color * gLightIntensity / PI;
}

float4 GetPhongColor(float3 viewDirection, float3 normal, float2 texCoord) {
    float4 color = gSpecularMap.Sample(samplerState, texCoord);
    float4 expColor = gGlossinessMap.Sample(samplerState, texCoord);

    float exp = gShininessIntensity * expColor.x;

    float3 reflectance = reflect(normalize(gLightDirection), normalize(normal));
    float cosine = dot(normalize(reflectance), viewDirection);
    if (cosine < 0) {
        float4 black = {0.f, 0.f, 0.f, 1.f};
        return black;
    }

    return pow(cosine, exp) * color;
}

float GetArea(float3 normal) {
    float area = dot(-normalize(gLightDirection), normalize(normal));
    return area > 0 ? area : 0;
}

float4 PS(VS_OUTPUT input) : SV_TARGET {
    float3 normal = input.Normal;
    if (gNormalMappingEnabled) {
        normal = CalculateNormal(normalize(input.Normal), normalize(input.Tangent), input.TexCoord);
    }

    float3 invViewDirection = normalize(gCameraPosition - input.WorldPosition.xyz);
    float area = GetArea(normal);
    float4 color = GetDiffuseColor(input.TexCoord) * area;
    color += area * gAmbientColor;
    color += GetPhongColor(invViewDirection, normal, input.TexCoord);

    saturate(color);

    return color;
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

