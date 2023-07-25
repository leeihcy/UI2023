#pragma once
struct SOURCE_BITMAP_DATA;

namespace ui
{
class GpuLayerCommitContext;
class Hard3DTransform;
}

class TextureTile
{
public:
    TextureTile();
    ~TextureTile();

    void  Upload(RECT& rcSrc, UploadGpuBitmapInfo& source);
    void  Compositor(
            long xOffset,
            long yOffset,
            long vertexStartIndex,
            UI::GpuLayerCommitContext* pContext);

    void  SetIndex(int xIndex, int yIndex);

private:
    bool  create();

private:
    long  m_width;
	long  m_height;
	long  m_xIndex;
	long  m_yIndex;

    ID3D10Texture2D*  m_pTextureBuffer;
    ID3D10ShaderResourceView*  m_pShaderResourceView;
};

