matrix  g_matWorld;		// ��� ���̺� : Ŭ���̾�Ʈ���� ���� �ڵ忡 �����ϱ� ���� ��¡���� ��Ī
matrix  g_matView;		// matrix == float4x4
matrix  g_matProj;
float	g_fCurHp = 0.f;
float4	g_vColorHp;

float4	g_vColorBorder;
texture g_BaseTexture;	// ��� ���̺�
int		g_iType = 0.f;

bool	g_bVertical = false;

bool	g_bGrayScale = false;

sampler BaseSampler = sampler_state	// ���� ���� ������ ��� �ִ� ����Ʈ ������ �ؽ��ĸ� ���̴������� sampler��� �θ���.
{									// = sampler_state�� ���۸� �����Ͽ� ���¸� �����ߴٰ� ǥ���ϴ� ����(�ݵ�� �־�� ��)
	texture = g_BaseTexture;
	
	minfilter = linear;
	magfilter = linear;
};

struct VS_IN
{
	vector		vPosition : POSITION;		// �빮��(Symantic) : ����� �Ӽ��� �����ϴ� ����, vector == float4 ����

	float2		vTexUV	: TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;		

	float2		vTexUV : TEXCOORD0;
};

// ���ؽ� ���̵�
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV  = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(In.vPosition, matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

// �ȼ� ���̵�

struct PS_IN	// �ȼ� ���̴� ����ü�� POSITION ���� ���� ���� ����.
{
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : COLOR0;	
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);		// 2���� ������ �ؽ��ķκ��� ���� �����Ͽ� ������ �Լ�(��ȯŸ���� vectorŸ��)

	if (Out.vColor.a != 0)
	{
		if (g_iType == 1)// SkillTime
		{
			if (In.vTexUV.y > 1.f - g_fCurHp)
			{
				Out.vColor.rgb -= 0.6f;
				Out.vColor.a = 1.f;
			}
		}
		else if (g_iType == 2) // Number Color
		{
			if (Out.vColor.a > 0.5f)
			{
				if (Out.vColor.r > 0.5f
					&& Out.vColor.g > 0.5f
					&& Out.vColor.b > 0.5f)
					Out.vColor = g_vColorHp;

				if (Out.vColor.r <= 0.5f		// �׵θ�
					&& Out.vColor.g <= 0.5f
					&& Out.vColor.b <= 0.5f)
					Out.vColor = g_vColorBorder;
			}
		}
		else
		{
			if (false == g_bVertical)
			{
				if (Out.vColor.r <= 0.0f
					&& Out.vColor.g <= 0.0f
					&& Out.vColor.b <= 0.0f)
					Out.vColor.a = 0.f;


				if (Out.vColor.r >= 0.9f
					&& Out.vColor.g >= 0.9f
					&& Out.vColor.b >= 0.9f)
				{
					if (In.vTexUV.x <= g_fCurHp)
					{
						Out.vColor.r = g_vColorHp.x;
						Out.vColor.g = g_vColorHp.y;
						Out.vColor.b = g_vColorHp.z;
						Out.vColor.a = g_vColorHp.w;
					}
					else
						Out.vColor.a = 0.f;
				}
			}
			else
			{
				if (Out.vColor.r > 0.8f
					&& Out.vColor.g > 0.8f
					&& Out.vColor.b > 0.8f)
					Out.vColor.a = 0.f;

				if (In.vTexUV.y < 1.f - g_fCurHp)
				{
					Out.vColor.r = g_vColorHp.x;
					Out.vColor.g = g_vColorHp.y;
					Out.vColor.b = g_vColorHp.z;
					Out.vColor.a = g_vColorHp.w;
				}
			}
		}

		if (g_bGrayScale)
			Out.vColor.gb = Out.vColor.r;
	}

	return Out;
}

technique Default_Device			// �ϳ��� ���̴� ������ �ϳ��� ��ũ��ũ�� ���� �� �ִ�. ��� �������� pass�� ���� ���� �ִ�.
{
	// ����� ĸ��ȭ
	pass Test
	{
		alphaBlendEnAbLe = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		//AlphaBlendEnable = true;
		//srcblend = srccolor;//srcalpha;
		//destblend = invsrccolor;//invsrcalpha;
		//ZWriteEnable = false;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader =  compile ps_3_0 PS_MAIN();

	}

	/*pass Sour
	{
		vertexshader = compile vs_3_0 VS_Temp();
		pixelshader = compile ps_3_0 PS_Sour();
	}*/
};
