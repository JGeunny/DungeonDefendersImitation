

vector			g_vLightDir;
vector			g_vLightPos;
float			g_fRange = 0.f;
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

vector			g_vMtrlDiffuse = (vector)1.f;
vector			g_vMtrlAmbient = (vector)1.f;
vector			g_vMtrlSpecular = (vector)1.f;

vector			g_vCamPosition;

matrix			g_matProjInv;
matrix			g_matViewInv;

texture			g_NormalTexture; // Target_Normal

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
};

texture			g_DepthTexture; // Target_Normal

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};


struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vShade : COLOR0;
	vector		vSpecular : COLOR1;
};

PS_OUT PS_MAIN_DIRECTION(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vNormalInfo = tex2D(NormalSampler, In.vTexUV);
	vector	vDepthInfo = tex2D(DepthSampler, In.vTexUV);
	float	fViewZ = vDepthInfo.r * 1000.f;

	// 0 -> -1
	// 1 -> 1
	float3	vNormal = vNormalInfo.xyz * 2.f - 1.f;

	Out.vShade = (g_vLightDiffuse * g_vMtrlDiffuse) * (saturate(dot(normalize(g_vLightDir) * -1.f, vector(vNormal, 0.f))) + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vShade.a = 1.f;

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vector(vNormal, 0.f)));

	vector		vPosition = (vector)1.f;

	// 투영스페이스 상의 위치. => z나누기 이전의 상태( 투영행렬까지 곱해놓은 상태 )
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepthInfo.g * fViewZ;
	vPosition.w = fViewZ;

	vPosition = mul(vPosition, g_matProjInv);
	vPosition = mul(vPosition, g_matViewInv);

	vector		vLook = vPosition - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 30.f);
	Out.vSpecular *= 0.2f;
	Out.vSpecular.a = 0.0f;

	return Out;
}

PS_OUT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector	vNormalInfo = tex2D(NormalSampler, In.vTexUV);
	vector	vDepthInfo = tex2D(DepthSampler, In.vTexUV);
	float	fViewZ = vDepthInfo.r * 1000.f;

	vector		vPosition = (vector)1.f;

	// 투영스페이스 상의 위치. => z나누기 이전의 상태( 투영행렬까지 곱해놓은 상태 )
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepthInfo.g * fViewZ;
	vPosition.w = fViewZ;

	vPosition = mul(vPosition, g_matProjInv);
	vPosition = mul(vPosition, g_matViewInv);


	// 0 -> -1
	// 1 -> 1
	float3	vNormal = vNormalInfo.xyz * 2.f - 1.f;

	vector		vLightDir = vPosition - g_vLightPos;

	float		fDistance = length(vLightDir);

	float		fAtt = saturate((g_fRange - fDistance) / g_fRange);

	Out.vShade = (g_vLightDiffuse * g_vMtrlDiffuse) * (saturate(dot(normalize(vLightDir) * -1.f, vector(vNormal, 0.f))) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

	Out.vShade.a = 1.f;

	vector		vReflect = reflect(normalize(vLightDir), normalize(vector(vNormal, 0.f)));



	vector		vLook = vPosition - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 30.f) * fAtt;
	Out.vSpecular.a = 0.0f;

	return Out;
}

technique Default_Technique
{
	pass Direction
	{
		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_DIRECTION();
	}

	pass Point
	{
		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_POINT();
	}
}