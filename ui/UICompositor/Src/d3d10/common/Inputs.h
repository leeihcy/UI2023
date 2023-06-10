#pragma once

// 对应fx文件中的VS()参数
struct DXUT_SCREEN_VERTEX_10
{
    float x, y, z;
    UI::D3DCOLORVALUE color;
    float tu, tv;
};

class Inputs
{
public:
    static bool Init(ID3D10Device* pDevice);
    static void Rlease();

    // static void SetVertex4(DXUT_SCREEN_VERTEX_10 vertices[4]);

    static ID3D10InputLayout*  m_pInputLayout;
    // static ID3D10Buffer*  m_pVertexBuffer;

};