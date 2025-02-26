
matrix		g_matWorld, g_matView, g_matProj;
texture		g_DiffuseTexture;
vector		g_vColor;
 
sampler DiffuseSampler = sampler_state
{
	MinFilter = linear;
	MagFilter = linear;
	MipFilter = linear;

	Texture = g_DiffuseTexture;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	//Out.vTexUV = In.vTexUV * 2.f;
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(VS_OUT In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	/*if(Out.vColor.a > 0)
		Out.vColor += g_vColor;*/
	////Out.vColor.a = In.vTexUV.y;
	//if (Out.vColor.r < 0.1f
	//	&& Out.vColor.g < 0.1f
	//	&& Out.vColor.b < 0.1f)
	//	Out.vColor.a = 0.f;

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering
	{
		AlphaBlendEnable = true;
		srcblend = srccolor;//srcalpha;
		destblend = invsrccolor;//invsrcalpha;
		ZWriteEnable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}