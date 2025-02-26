#include "Shader_Base.hpp"

texture g_BaseTexture1;

sampler BaseSampler1 = sampler_state
{
	texture = g_BaseTexture1;

	minfilter = linear;
	magfilter = linear;
};

texture g_FilterTexture;

sampler FilterSampler = sampler_state
{
	texture = g_FilterTexture;

	minfilter = linear;
	magfilter = linear;
};

texture g_AuraTexture;

sampler AuraSampler = sampler_state
{
	texture = g_AuraTexture;

	minfilter = linear;
	magfilter = linear;
};

float		g_fDetail;
float		g_fPower;
float		g_fRange;

vector		g_vPlayerPos;

vector		g_vLightDir = (vector)0.f;

vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlDiffuse;
vector		g_vMtrlAmbient;
vector		g_vMtrlSpecular;

vector		g_vCamPos;

struct		VS_IN
{
	vector		vPosition	: POSITION;
	vector		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT 
{
	vector		vPosition	: POSITION;
	vector		vShade		: COLOR0;
	vector		vSpecular	: COLOR1;
	float2		vTexUV		: TEXCOORD0;
	vector		vWorldPos	: TEXCOORD1;
};

VS_OUT		VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);


	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	//Out.vNormal = mul(vector(In.vNormal.xyz, 0.f), matWVP);

	vector vWorldNormal = mul(vector(In.vNormal.xyz, 0.f), g_matWorld);
	vector vWorldLight = g_vLightDir * -1.f;

	// ³­¹Ý»ç±¤(ºûÀÇ ¼¼±â)
	float	fIntensity = saturate(dot(normalize(vWorldNormal), normalize(vWorldLight)));
	//float	fIntensity = max(dot(normalize(vWorldNormal), normalize(vWorldLight)), 0.f);

	// ·¥¹öÆ® Á¶¸í °ø½Ä : ºûÀÇ ¼¼±â * (L.D * M.D) + (L.A * M.A)
	// Æþ Á¶¸í °ø½Ä     : ºûÀÇ ¼¼±â * (L.D * M.D) + (L.A * M.A) + (½ºÆåÅ§·¯ÀÇ ¼¼±â * (L.S * M.S))

	Out.vShade = fIntensity;
	Out.vShade.a = 1.f;

	// Á¤¹Ý»ç±¤(½ºÆåÅ§·¯ÀÇ ¼¼±â)

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector		vWorldPos = mul(vector(In.vPosition.xyz, 1.f), g_matWorld);
	vector		vLook = g_vCamPos - vWorldPos;

	Out.vSpecular	= pow(saturate(dot(normalize(vReflect), normalize(vLook))), g_fPower);
	Out.vSpecular.a = 1.f;

	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = vWorldPos;

	return Out;
}

struct PS_IN
{
	vector		vShade		: COLOR0;
	vector		vSpecular	: COLOR1;
	float2		vTexUV		: TEXCOORD0;
	vector		vWorldPos	: TEXCOORD1;
};

struct PS_OUT
{
	vector			vColor : COLOR0;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)Out;
	
	vector vColor[2];

	vColor[0] = tex2D(BaseSampler, In.vTexUV * g_fDetail);
	vColor[1] = tex2D(BaseSampler1, In.vTexUV * g_fDetail);
	vector vFilter = tex2D(FilterSampler, In.vTexUV);

	Out.vColor = vColor[0] * vFilter + vColor[1] * (1.f - vFilter);

	Out.vColor = Out.vColor * (g_vLightDiffuse * g_vMtrlDiffuse) * (In.vShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ (In.vSpecular * (g_vLightSpecular * g_vMtrlSpecular));

	
	vector		vAuraColor = (vector)0.f;

	if (g_vPlayerPos.x - g_fRange < In.vWorldPos.x &&
		g_vPlayerPos.x + g_fRange > In.vWorldPos.x &&
		g_vPlayerPos.z - g_fRange < In.vWorldPos.z &&
		g_vPlayerPos.z + g_fRange > In.vWorldPos.z)
	{
		float2		vTexUV;

		vTexUV.x = (In.vWorldPos.x - (g_vPlayerPos.x - g_fRange)) / (g_fRange * 2.f);
		vTexUV.y = ((g_vPlayerPos.z + g_fRange) - In.vWorldPos.z) / (g_fRange * 2.f);

		vAuraColor = tex2D(AuraSampler, vTexUV);

	}
	
	Out.vColor += vAuraColor;
	
	return Out;
}

technique Default_Device	
{
	// ±â´ÉÀÇ Ä¸½¶È­
	pass 
	{
		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}
};