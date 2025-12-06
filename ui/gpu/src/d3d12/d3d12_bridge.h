#ifndef _UI_GPU_SRC_D3D12_D3D12BRIDGE_H_
#define _UI_GPU_SRC_D3D12_D3D12BRIDGE_H_

struct ID3D12GraphicsCommandList;
struct ID3D12CommandQueue;

namespace d3d12 {
class InFlightFrame;
class SwapChain;

struct IBridge {
  virtual void DeviceWaitIdle() = 0;
  virtual ID3D12CommandQueue* GetCommandQueue() = 0;
  virtual ID3D12GraphicsCommandList* GetCurrentCommandList() = 0;
  virtual InFlightFrame& GetCurrentInflightFrame() = 0;
  virtual SwapChain& GetSwapChain() = 0;
}; 

}

#endif