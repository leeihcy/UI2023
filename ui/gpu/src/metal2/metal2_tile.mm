#include "metal2_tile.h"
#import "src/metal2/import.h"

namespace ui {

Metal2TextureTile::Metal2TextureTile(IMetal2Bridge& bridge) :m_bridge(bridge) {}

Metal2TextureTile::~Metal2TextureTile() {
  if (m_texture_descriptor) {
    [m_texture_descriptor release];
    m_texture_descriptor = nullptr;
  }
  if (m_texture) {
    [m_texture release];
    m_texture = nullptr;
  }
}

void Metal2TextureTile::Upload(ui::Rect &dirty_of_tile,
                               ui::Rect &dirty_of_layer,
                               ui::GpuUploadBitmap &source) {
  if (!m_texture_descriptor) {
    m_texture_descriptor = [MTLTextureDescriptor new];
    m_texture_descriptor.textureType = MTLTextureType2D;
    m_texture_descriptor.width = TILE_SIZE;
    m_texture_descriptor.height = TILE_SIZE;
    m_texture_descriptor.storageMode = MTLStorageModeManaged;
    m_texture_descriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;
    m_texture_descriptor.usage = MTLTextureUsageShaderRead;
  }
  if (!m_texture) {
    m_texture = [m_bridge.GetMetalDevice()
        newTextureWithDescriptor:m_texture_descriptor];
  }

  MTLRegion region = {0};
  region.origin.x = dirty_of_tile.left;
  region.origin.y = dirty_of_tile.top;
  region.size.width = dirty_of_tile.Width();
  region.size.height = dirty_of_tile.Height();
  region.size.depth = 1; // 这里不能写0

  const void *bits = source.bits + (dirty_of_layer.top * source.pitch +
                                    dirty_of_layer.left * 4);
  [m_texture replaceRegion:region
               mipmapLevel:0
                 withBytes:bits
               bytesPerRow:source.pitch];
}

void Metal2TextureTile::Compositor(id<MTLBuffer> index_buffer, long vertex_start_index,
                               ui::GpuLayerCommitContext *pContext) {
  id<MTLRenderCommandEncoder> renderEncoder = m_bridge.GetRenderEncoder();

  [renderEncoder setFragmentTexture:m_texture atIndex:0];

  //[renderEncoder setCullMode:MTLCullModeBack];
  //[renderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];


  [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangleStrip
                            indexCount:4
                             indexType:MTLIndexTypeUInt16
                           indexBuffer:index_buffer
                     indexBufferOffset:vertex_start_index*2 // Byte offset 
                         instanceCount:1];
}

bool Metal2TextureTile::create() {
  MTLTextureDescriptor* desc = [MTLTextureDescriptor new];
  [m_bridge.GetMetalDevice() newTextureWithDescriptor:desc];
  [desc release];
  
  return true;
}

} // namespace ui