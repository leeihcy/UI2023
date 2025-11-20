#ifndef _UI_BASE_COMMAND_LINE_H_
#define _UI_BASE_COMMAND_LINE_H_
#include <cstddef>
#include <string>
#include <vector>
#include <map>

// ----------------------------
// sdk模块参数定义

// 是否启用硬件合成，可选值：1 | 0
#define SWITCH_SDK_ENABLE_HARDWARE "sdk-enable-hardware-composition"

// ----------------------------
// gpu模块参数定义

// 引擎设置，可选值：d3d11 | d3d12 | metal2 | vulkan
#define SWITCH_GPU_API "gpu-api"



#if defined(OS_MAC)
extern "C" {
  extern const char*** _NSGetArgv(void);
  extern int* _NSGetArgc();
}
#endif

namespace ui {

constexpr const char* g_switch_prefixes[] = { "--", "-", "/" };
constexpr int g_switch_prefix_count = std::size(g_switch_prefixes);

class CommandLine {
public:
  void InitForCurrentProcess() {
#if defined(OS_WIN)
    assert(false); 
    // GetCommandLineA
#elif defined (OS_LINUX)
    assert(false);
    // read from "/proc/self/cmdline"
#elif defined(OS_MAC)
    Init(*_NSGetArgc(), *_NSGetArgv());
#endif
  }
  void Init(int argc, const char** argv) {
    std::vector<std::string> arg_vec;
    arg_vec.reserve(argc);
    for (int i = 0; i < argc; i++) {
      arg_vec.push_back(argv[i]);
    }
    parse(arg_vec);
  }
  bool HasSwitch(const std::string& switch_string) {
    return m_switch_value_map.find(switch_string) != m_switch_value_map.end();
  }
  std::string GetSwitchValue(const std::string& switch_string, bool* exist=nullptr) {
    if (exist) { *exist = false; }
    if (m_switch_value_map.empty()) {
      return "";
    }

    auto iter = m_switch_value_map.find(switch_string);
    if (iter == m_switch_value_map.end()) {
      return "";
    }
    if (exist) { *exist = true; }
    return iter->second;
  }

private:
  void parse(const std::vector<std::string>& arg_vec) {
    for (const std::string& arg : arg_vec) {
      int prefix_length = getSwitchPrefixLength(arg);
      if (prefix_length == 0 || prefix_length == arg.length()) {
        // not a switch
        continue;
      }
      size_t equal_index = arg.find("=");
      std::string switch_string = arg.substr(prefix_length, equal_index-prefix_length);
      std::string switch_value;
      if (equal_index != std::string::npos) {
        switch_value = arg.substr(equal_index+1);
      }
      m_switch_value_map[switch_string] = switch_value;
    }
  }

  int getSwitchPrefixLength(const std::string& args) {
    for (size_t i = 0; i < g_switch_prefix_count; i++) {
      const char* prefix = g_switch_prefixes[i];
      int prefix_len = strlen(prefix);
      if (args.substr(0, prefix_len) == prefix) {
        return prefix_len;
      }
    }
    return 0;
  }

private:
  std::map<std::string, std::string> m_switch_value_map;
};

}
#endif