#ifndef _UI_SDK_SRC_APPLICATION_CONFIG_CONFIG_H_
#define _UI_SDK_SRC_APPLICATION_CONFIG_CONFIG_H_

namespace ui {

class Config {
public:
  static Config& GetInstance();

  struct Debug {
    // 记录窗口的paint事件
    bool log_window_onpaint = true;
    // 记录和gpu相关的动作
    bool log_gpu = true;
    // 保护rendertarget到本地
    bool dump_render_target = false;
  };

public:
  Debug debug;
};

}

#endif // _UI_SDK_SRC_APPLICATION_CONFIG_CONFIG_H_
