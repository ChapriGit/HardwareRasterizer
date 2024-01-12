// =========================
// Global variables
// =========================

float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

static const float PI = 3.14159265f;

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
    float2 TexCoord: TEXCOORD;
};

// ==========================
// Vertex Shader
// ==========================
VS_OUTPUT VS(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
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
RasterizerState gRasterizerState {
    CullMode = none;
    FrontCounterClockwise = false;
};

BlendState gBlendState {
    BlendEnable[0] = true;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
    SrcBlendAlpha = zero;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState {
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less;
    StencilEnable = false;
};

technique11 DefaultTechnique {
    pass P0 {
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader( CompileShader(vs_5_0, VS()) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(ps_5_0, PS()) );
    }
}

