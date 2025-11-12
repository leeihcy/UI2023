#include "src/metal2/metal2_pipeline.h"

namespace metal2 {

PipeLine::PipeLine(ui::IMetal2Bridge& bridge) : m_bridge(bridge) {

}

bool PipeLine::Create() {
  MTLRenderPipelineDescriptor* desc = [MTLRenderPipelineDescriptor new];
  desc.label = @"Pipeline desc";
  desc.sampleCount = 1;

  id<MTLDevice> device = m_bridge.GetMetalDevice();

/*
// 加载库文件
NSError *error = nil;
id<MTLLibrary> myLibrary = [device makeLibraryWithFile:libraryFile error:&error];
if (!myLibrary) {
    NSLog(@"Failed to create library: %@", error.localizedDescription);
    return;
}

// 从库中获取具体的着色器函数
id<MTLFunction> vertexFunction = [myLibrary newFunctionWithName:@"vertexShader"];
id<MTLFunction> fragmentFunction = [myLibrary newFunctionWithName:@"fragmentShader"];
*/

  // desc.vertexFunction = [library newFunctionWithName:@"textureVertexShader"];
  // desc.fragmentFunction = [library newFunctionWithName:@"textureFragmentShader"];
  // desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

  NSError *error = nullptr;
  [device newRenderPipelineStateWithDescriptor:desc error:&error];

  [desc release];
  return true;
}

}