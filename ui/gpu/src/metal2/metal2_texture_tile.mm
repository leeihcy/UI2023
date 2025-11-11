#include "metal2_texture_tile.h"
#import "src/metal2/import.h"

namespace ui {

Metal2TextureTile::Metal2TextureTile(IMetal2Bridge& bridge) :m_bridge(bridge) {}

Metal2TextureTile::~Metal2TextureTile() {

}

void Metal2TextureTile::Upload(ui::Rect &rcSrc, ui::GpuUploadBitmap &source) {
}

void Metal2TextureTile::Compositor(long, long, long vertexStartIndex,
                               ui::GpuLayerCommitContext *pContext) {
  
}

bool Metal2TextureTile::create() {
  MTLTextureDescriptor* desc = [MTLTextureDescriptor new];
  [m_bridge.GetMetalDevice() newTextureWithDescriptor:desc];
  [desc release];
  
  return true;
}

} // namespace ui