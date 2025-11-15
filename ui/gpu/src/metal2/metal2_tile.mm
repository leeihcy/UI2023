#include "metal2_tile.h"
#import "src/metal2/import.h"

namespace ui {

Metal2TextureTile::Metal2TextureTile(IMetal2Bridge& bridge) :m_bridge(bridge) {}

Metal2TextureTile::~Metal2TextureTile() {
  if (m_texture_descriptor) {
    [m_texture_descriptor release];
    m_texture_descriptor = nullptr;
  }
}

void Metal2TextureTile::Upload(ui::Rect &dirty_of_tile, ui::Rect &dirty_of_layer,
 ui::GpuUploadBitmap &source) {
}

void Metal2TextureTile::Compositor(long, long, long vertexStartIndex,
                               ui::GpuLayerCommitContext *pContext) {
  if (!m_texture_descriptor) {
    m_texture_descriptor = [MTLTextureDescriptor new];
    m_texture_descriptor.textureType = MTLTextureType2D;
    m_texture_descriptor.width = TILE_SIZE;
    m_texture_descriptor.height = TILE_SIZE;
    m_texture_descriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;
    m_texture_descriptor.usage =
        MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
  }
  if (!m_rendertarget_texture) {
    m_rendertarget_texture = [m_bridge.GetMetalDevice()
        newTextureWithDescriptor:m_texture_descriptor];
  }
}

bool Metal2TextureTile::create() {
  MTLTextureDescriptor* desc = [MTLTextureDescriptor new];
  [m_bridge.GetMetalDevice() newTextureWithDescriptor:desc];
  [desc release];
  
  return true;
}

} // namespace ui