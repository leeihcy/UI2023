// d3d10 shader file

Texture2D  g_Texture;

cbuffer cb0
{
	// 3D�任
	matrix    g_Matrix;       // �任����
	matrix    g_orthMatrix;   // ����ͶӰ�������ﲻ����͸��ͶӰ������Ϊ͸��ͶӰ�Ķ����С����z���ı䡣������������Ҫ������С���䣬ֻ����ת���ӣ�
	float2    g_vsDestPos = float2(0, 0);    // û�о���任ʱ����Ҫ���û��Ƶ�Ŀ��λ��(x,y)
	float     g_alpha = 1.0f;               // ��Ⱦʱ��alpha͸���ȣ�0~1��
}

// 2016.6.13
// ע�����ﲻ����WRAP.ʹ��WRAP�ᵼ�»��Ʒֿ�ʱ���ֿ�ı�Ե�����쳣���������ĳ�MIRROR�ͺ��ˣ�ԭ��δ֪��
SamplerState Sampler
{    
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;
};

struct VS_OUTPUT
{    
	float4 Pos : SV_POSITION;    
	float4 Dif : COLOR;    
	float2 Tex : TEXCOORD;
};

// ������ɫ����ִ��͸�ӳ�������ֻ���ͶӰ���󲿷֡�
// ͸�ӳ����������Ӳ�����
VS_OUTPUT VS( 
		float3 vPos : POSITION,              
		float4 Dif : COLOR,              
		float2 vTexCoord0 : TEXCOORD 
		)
{    
	VS_OUTPUT Output;   

	// ����Ĭ��vPos������ƫ����������ÿ�θ��µ�ʱ�򣬶�Ҫ��дvertex buffer.
	// ��ƫ��������g_vsDestPos���ݽ���
	vPos.x += g_vsDestPos.x;
	vPos.y += g_vsDestPos.y;

    // �任
	Output.Pos = mul(float4(vPos, 1.0f), g_orthMatrix);

	Output.Dif = Dif;    
	Output.Tex = vTexCoord0;    
	return Output;
}

// �������ӳ��
VS_OUTPUT VSMatrix( 
		float3 vPos : POSITION,              
		float4 Dif : COLOR,              
		float2 vTexCoord0 : TEXCOORD 
		)
{    
	VS_OUTPUT Output;    
	
	// ����Ĭ��vPos������ƫ����������ÿ�θ��µ�ʱ�򣬶�Ҫ��дvertex buffer.
	// ��ƫ��������g_vsDestPos���ݽ���
	vPos.x += g_vsDestPos.x;
	vPos.y += g_vsDestPos.y;

	// �任
	float4 fTrans = mul(float4(vPos, 1.0f), g_Matrix);    
	Output.Pos = mul(fTrans, g_orthMatrix);
	Output.Dif = Dif;    
	Output.Tex = vTexCoord0;    
	return Output;
}

float4 PS( VS_OUTPUT In ) : SV_Target
{    
	float4 color = g_Texture.Sample( Sampler, In.Tex ) * In.Dif;
	color.a = color.a * g_alpha;
	return color;
}

float4 PSUntex( VS_OUTPUT In ) : SV_Target
{    
	return In.Dif;
}

technique10 DrawTexture
{    
	pass P0    
	{        
		SetVertexShader( CompileShader( vs_4_0, VS() ) );        
		SetPixelShader( CompileShader( ps_4_0, PS() ) );        
	}
}

// �������
technique10 DrawTextureMatrix
{    
	pass P0    
	{        
		SetVertexShader( CompileShader( vs_4_0, VSMatrix() ) );        
		SetPixelShader( CompileShader( ps_4_0, PS() ) );      
    }
}

technique10 DrawRect
{    
	pass P0    
	{        
		SetVertexShader( CompileShader( vs_4_0, VS() ) );        
		SetPixelShader( CompileShader( ps_4_0, PSUntex() ) );        
	}
}

// ������m_pDevice->ClearRenderTargetView���ܣ�ֱ�Ӹ���Ŀ������
technique10 FillRect
{    
	pass P0    
	{        
		SetVertexShader( CompileShader( vs_4_0, VS() ) );        
		SetPixelShader( CompileShader( ps_4_0, PSUntex() ) );        
	}
}

// Ŀǰ������ʵ��stencil����
technique10 FillRectMatrix
{    
	pass P0    
	{        
		SetVertexShader( CompileShader( vs_4_0, VSMatrix() ) );        
		SetPixelShader( CompileShader( ps_4_0, PSUntex() ) );        
	}
}
