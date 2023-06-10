// d3d10 shader file

Texture2D  g_Texture;

cbuffer cb0
{
	// 3D变换
	matrix    g_Matrix;       // 变换矩阵
	matrix    g_orthMatrix;   // 正交投影矩阵（这里不采用透视投影矩阵，因为透视投影的对象大小随着z而改变。而我们这里是要求对象大小不变，只是旋转而矣）
	float2    g_vsDestPos = float2(0, 0);    // 没有矩阵变换时，需要设置绘制的目标位置(x,y)
	float     g_alpha = 1.0f;               // 渲染时的alpha透明度（0~1）
}

// 2016.6.13
// 注：这里不能用WRAP.使用WRAP会导致绘制分块时，分块的边缘出现异常线条，而改成MIRROR就好了，原因未知。
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

// 顶点着色器不执行透视除法，它只完成投影矩阵部分。
// 透视除法会随后由硬件完成
VS_OUTPUT VS( 
		float3 vPos : POSITION,              
		float4 Dif : COLOR,              
		float2 vTexCoord0 : TEXCOORD 
		)
{    
	VS_OUTPUT Output;   

	// 现在默认vPos不包括偏移量，避免每次更新的时候，都要重写vertex buffer.
	// 将偏移量放在g_vsDestPos传递进来
	vPos.x += g_vsDestPos.x;
	vPos.y += g_vsDestPos.y;

    // 变换
	Output.Pos = mul(float4(vPos, 1.0f), g_orthMatrix);

	Output.Dif = Dif;    
	Output.Tex = vTexCoord0;    
	return Output;
}

// 带矩阵的映射
VS_OUTPUT VSMatrix( 
		float3 vPos : POSITION,              
		float4 Dif : COLOR,              
		float2 vTexCoord0 : TEXCOORD 
		)
{    
	VS_OUTPUT Output;    
	
	// 现在默认vPos不包括偏移量，避免每次更新的时候，都要重写vertex buffer.
	// 将偏移量放在g_vsDestPos传递进来
	vPos.x += g_vsDestPos.x;
	vPos.y += g_vsDestPos.y;

	// 变换
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

// 矩阵绘制
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

// 类似于m_pDevice->ClearRenderTargetView功能，直接覆盖目标像素
technique10 FillRect
{    
	pass P0    
	{        
		SetVertexShader( CompileShader( vs_4_0, VS() ) );        
		SetPixelShader( CompileShader( ps_4_0, PSUntex() ) );        
	}
}

// 目前是用于实现stencil剪裁
technique10 FillRectMatrix
{    
	pass P0    
	{        
		SetVertexShader( CompileShader( vs_4_0, VSMatrix() ) );        
		SetPixelShader( CompileShader( ps_4_0, PSUntex() ) );        
	}
}
