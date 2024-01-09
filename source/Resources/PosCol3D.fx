// =========================
// Global variables
// =========================

float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

SamplerState samplerState : register(s0);
// ==========================
// Input and Output Structs
// ==========================

struct VS_INPUT {
    float3 Position: POSITION;
    float3 Color: COLOR;
    float2 TexCoord: TEXCOORD;
};

struct VS_OUTPUT {
    float4 Position: SV_POSITION;
    float3 Color: COLOR;
    float2 TexCoord: TEXCOORD;
};

// ==========================
// Vertex Shader
// ==========================

VS_OUTPUT VS(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;

    return output;
}

// ==========================
// Pixel Shader
// ==========================
float4 PS(VS_OUTPUT input) : SV_TARGET {
    return gDiffuseMap.Sample(samplerState, input.TexCoord);
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

