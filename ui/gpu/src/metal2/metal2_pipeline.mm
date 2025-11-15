#include "src/metal2/metal2_pipeline.h"
#include <mach-o/dyld.h>
#include <vector>

namespace metal2 {

PipeLine::PipeLine(ui::IMetal2Bridge& bridge) : m_bridge(bridge) {

}

bool PipeLine::Create() {
  MTLRenderPipelineDescriptor* desc = [MTLRenderPipelineDescriptor new];
  desc.label = @"Pipeline desc";
  desc.sampleCount = 1;

  id<MTLDevice> device = m_bridge.GetMetalDevice();

  // 加载库文件
  uint32_t executable_length = 0;
  _NSGetExecutablePath(NULL, &executable_length);
 
  std::vector<char> buffer(executable_length + 1 + 64);
  char* p = buffer.data();

  _NSGetExecutablePath(p, &executable_length);
  *strrchr(p, '/') = 0;
  strcat(p, "/shaders/metal2.metallib");

  NSError *error = nil;
  NSString* library_path = [NSString stringWithUTF8String: p];
  id<MTLLibrary> shader_lib = [device newLibraryWithFile:library_path error:&error];
  [library_path release];

  if (!shader_lib) {
    return false;
  }

  desc.vertexFunction = [shader_lib newFunctionWithName:@"vertexShader"];
  desc.fragmentFunction = [shader_lib newFunctionWithName:@"fragmentShader"];
  desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

  m_state = [device newRenderPipelineStateWithDescriptor:desc error:&error];

  [desc release];
  return true;
}

bool PipeLine::Destroy() {
  if (m_state) {
    [m_state release];
    m_state = nil;
  }
  return true;
}
}