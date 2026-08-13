#include "mojo/core/node_controller.h"

#include <random>
#include <assert.h>

namespace mojo {
namespace core {

static  void GenerateRandomName(ports::NodeName *name) {
  // 玩具版：直接用随机数填满两个64位字段
  // 在真实代码中绝对不要这样用！！

  // 用标准库的随机数生成器（Mersenne Twister）
  static std::random_device rd;                        // 获取随机种子
  static std::mt19937_64 gen(rd());                    // 64位随机数生成器
  static std::uniform_int_distribution<uint64_t> dist; // 均匀分布

  name->v1 = dist(gen);
  name->v2 = dist(gen);
}
ports::NodeName GetRandomNodeName() {
  ports::NodeName name;
  GenerateRandomName(&name);
  return name;
}

NodeController::NodeController()
    : name_(GetRandomNodeName()), node_(new ports::Node(name_, this)) {
}

base::WritableSharedMemoryRegion NodeController::CreateSharedBuffer(size_t num_bytes) {
  return base::WritableSharedMemoryRegion::Create(num_bytes);
}


void NodeController::ForwardEvent(const ports::NodeName& node,
                                  ports::ScopedEvent event) {
}

void NodeController::BroadcastEvent(ports::ScopedEvent event) {
}

void NodeController::PortStatusChanged(const ports::PortRef& port) {
  std::shared_ptr<ports::UserData> user_data;
  node_->GetUserData(port, &user_data);

  PortObserver* observer = static_cast<PortObserver*>(user_data.get());
  if (observer) {
    observer->OnPortStatusChanged();
  }
}

void NodeController::SetPortObserver(const ports::PortRef& port,
                                     std::shared_ptr<PortObserver> observer) {
  node_->SetUserData(port, std::move(observer));
}

int NodeController::SendUserMessage(
    const ports::PortRef& port,
    std::unique_ptr<ports::UserMessageEvent> message) {
  return node_->SendUserMessage(port, std::move(message));
}

}
}