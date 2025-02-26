#include "Shader_Base.hpp"

float		g_fDetail;
float		g_fPower;
float2		g_vTexUV;
vector		g_vLightDir = (vector)0.f;

vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlDiffuse;
vector		g_vMtrlAmbient;
vector		g_vMtrlSpecular;

vector		g_vCamPos;

float		g_fType;

bool		g_bGrayScale = false;

struct		VS_IN
{
	vector		vPosition	: POSITION;
	vector		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT 
{
	vector		vPosition : POSITION;
	float4		vNormal : NORMAL;
	vector		vShade : COLOR0;
	vector		vSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;
	vector		vProjPos : TEXCOORD2;
};

VS_OUT		VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);


	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);

	//vector vWorldNormal = mul(vector(In.vNormal.xyz, 0.f), g_matWorld);
	//vector vWorldLight = g_vLightDir * -1.f;

	//float	fIntensity = saturate(dot(normalize(vWorldNormal), normalize(vWorldLight)));
	//
	//Out.vShade = fIntensity;
	//Out.vShade.a = 1.f;

	// 정반사광(스펙큘러의 세기)

	//vector		vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	//vector		vWorldPos = mul(vector(In.vPosition.xyz, 1.f), g_matWorld);
	//vector		vLook = g_vCamPos - vWorldPos;

	//Out.vSpecular = pow(saturate(dot(normalize(vReflect), normalize(vLook))), g_fPower);
	//Out.vSpecular.a = 1.f;

	Out.vTexUV = In.vTexUV;

	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	vector		vShade : COLOR0;
	vector		vSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;
	float4		vNormal : NORMAL;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector			vColor : COLOR0;
	vector			vNormal : COLOR1;
	vector			vDepth : COLOR2;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)Out;

	In.vTexUV += g_vTexUV;
	vector vColor = tex2D(BaseSampler, In.vTexUV);

	float	fAlpha = vColor.a;

	Out.vColor = vColor;// *(g_vLightDiffuse * g_vMtrlDiffuse) * (In.vShade + (g_vLightAmbient * g_vMtrlAmbient));
		//+ (In.vSpecular * (g_vLightSpecular * g_vMtrlSpecular));

	if (g_fType == 1.f) // 초록색
	{
		Out.vColor.r -= 0.3f;
		if (Out.vColor.r < 0.f)
			Out.vColor.r = 0.f;
		Out.vColor.g += 0.2f;
		if (Out.vColor.g > 1.f)
			Out.vColor.g = 1.f;
		Out.vColor.b -= 0.3f;
		if (Out.vColor.b < 0.f)
			Out.vColor.b = 0.f;
		Out.vColor.a = fAlpha;
	}
	else if(g_fType == 2.f) //빨간색
	{
		Out.vColor.r += 0.2f;
		if (Out.vColor.r > 1.f)
			Out.vColor.r = 1.f;
		Out.vColor.g -= 0.3f;
		if (Out.vColor.g < 0.f)
			Out.vColor.g = 0.f;
		Out.vColor.b -= 0.3f;
		if (Out.vColor.b < 0.f)
			Out.vColor.b = 0.f;
		Out.vColor.a = fAlpha;
	}
	else if (g_fType == 3.f) //검정색 제거
	{
		//if (Out.vColor.r < 0.2f
		//	&& Out.vColor.g < 0.2f
		//	&& Out.vColor.b < 0.2f)
		//	Out.vColor.a = 0.f;
	}
	else if (g_fType == 4.f) //에너지 볼
	{
		Out.vColor.r += 0.1f;
		Out.vColor.g += 0.4f;
		Out.vColor.b += 0.5f;
	}
	else
		Out.vColor.a = fAlpha;

	if(g_bGrayScale)
		Out.vColor.gb = Out.vColor.r;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / 1000.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}

PS_OUT		PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	/*Out.vColor = vColor * (g_vLightDiffuse * g_vMtrlDiffuse) * (In.vShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ (In.vSpecular * (g_vLightSpecular * g_vMtrlSpecular));*/

	return Out;
}


technique Default_Device	
{
	// 기능의 캡슐화
	pass 
	{
		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}

	pass	AlphaTesting
	{
		alphatestenable = true;
		alphafunc = Greater;
		alpharef = 0xc0;
		cullmode = none;

		//AlphaBlendEnable = true;
		//srcblend = srccolor;//srcalpha;
		//destblend = invsrccolor;//invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN_ALPHA();
	}
};