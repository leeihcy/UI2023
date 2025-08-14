#ifndef _UI_SDK_SRC_APPLICATION_CONFIG_CONFIG_H_
#define _UI_SDK_SRC_APPLICATION_CONFIG_CONFIG_H_

namespace ui {

class Config {
public:
  static Config& GetInstance();

  struct Debug {
    // 记录窗口的paint事件
    bool log_window_onpaint = false;
    // 记录和gpu相关的动作
    bool log_gpu = false;
    // 保护rendertarget到本地
    bool dump_render_target = false;
  };

public:
  Debug debug;

  // 启用多线程渲染
  bool enable_render_thread = true;
};

}

#endif // _UI_SDK_SRC_APPLICATION_CONFIG_CONFIG_H_
