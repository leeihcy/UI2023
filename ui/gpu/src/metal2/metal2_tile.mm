#include "metal2_tile.h"
#import "src/metal2/import.h"

namespace ui {

Metal2TextureTile::Metal2TextureTile(IMetal2Bridge& bridge) :m_bridge(bridge) {}

Metal2TextureTile::~Metal2TextureTile() {
  if (m_texture_descriptorset) {
    [m_texture_descriptorset release];
    m_texture_descriptorset = nullptr;
  }
}

void Metal2TextureTile::Upload(ui::Rect &dirty_of_tile, ui::Rect &dirty_of_layer,
 ui::GpuUploadBitmap &source) {
}

void Metal2TextureTile::Compositor(long, long, long vertexStartIndex,
                               ui::GpuLayerCommitContext *pContext) {
  if (!m_texture_descriptorset) {
    m_texture_descriptorset = [MTLTextureDescriptor new];
    m_texture_descriptorset.textureType = MTLTextureType2D;
    m_texture_descriptorset.width = TILE_SIZE;
    m_texture_descriptorset.height = TILE_SIZE;
    m_texture_descriptorset.pixelFormat = MTLPixelFormatBGRA8Unorm;
    m_texture_descriptorset.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
  }
}

bool Metal2TextureTile::create() {
  MTLTextureDescriptor* desc = [MTLTextureDescriptor new];
  [m_bridge.GetMetalDevice() newTextureWithDescriptor:desc];
  [desc release];
  
  return true;
}

} // namespace ui