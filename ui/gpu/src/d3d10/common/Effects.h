#pragma once
struct ID3D10EffectTechnique;
struct ID3D10EffectShaderResourceVariable;
struct ID3D10EffectMatrixVariable;
struct ID3D10EffectVectorVariable;
struct ID3D10EffectScalarVariable;
struct ID3D10Device;
struct ID3D10Effect;

class Effects
{
public:
    static bool Init(ID3D10Device* device);
    static void Release();

    static ID3D10Effect*  m_pEffect;

    static ID3D10EffectTechnique*   m_pTechDrawTexture;  // Technique: RenderUI
    static ID3D10EffectTechnique*   m_pTechDrawRect;  // Technique: RenderUI without texture
    static ID3D10EffectTechnique*   m_pTechFillRect; 
    static ID3D10EffectTechnique*   m_pTechFillRectMatrix;
    static ID3D10EffectTechnique*   m_pTechDrawTextureMatrix;

    static ID3D10EffectShaderResourceVariable*  m_pFxTexture10;
	static ID3D10EffectMatrixVariable*  m_pFxMatrix;
    static ID3D10EffectVectorVariable*  m_pFxVsDestPos;
	static ID3D10EffectMatrixVariable*  m_pFxOrthMatrix;
	static ID3D10EffectScalarVariable*  m_pFxAlpha;
};