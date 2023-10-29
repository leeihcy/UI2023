#include "vkapp.h"
#include "vulkan/vulkan_core.h"
#include <_types/_uint32_t.h>
#include <cstddef>
#include <cstdio>
#include <set>
#include <string>

namespace ui {
static VulkanApplication s_app;

VulkanApplication::VulkanApplication() {
#if defined(_DEBUG)
  m_enable_validation_layers = true;
#endif
}
VulkanApplication::~VulkanApplication() {}
VulkanApplication &VulkanApplication::Get() { return s_app; }

VkInstance &VulkanApplication::GetVkInstance() { return m_vk_instance; }

bool VulkanApplication::IsValidationLayersEnabled() {
  return m_enable_validation_layers;
}

bool VulkanApplication::Startup() {
  if (!create_vulkan_instance()) {
    printf("create_vulkan_instance failed\n");
    return false;
  }
  return true;
}

void VulkanApplication::Shutdown() {
  if (m_enable_validation_layers) {
    auto funcDestroy =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            m_vk_instance, "vkDestroyDebugUtilsMessengerEXT");
    funcDestroy(m_vk_instance, m_debug_messenger, nullptr);
  }

  vkDestroyInstance(m_vk_instance, nullptr);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkanLogCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                  void *pUserData) {

  bool raise_exception = false;

  const char *level = "";
  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    level = "VERBOSE";
  } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    level = "INFO";
  } else if (messageSeverity ==
             VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    level = "WARNING";
    printf("[%s] [GPU][Validation]: %s\n", level, pCallbackData->pMessage);
  } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    level = "ERROR";
    raise_exception = true;
    printf("[%s] [GPU][Validation]: %s\n", level, pCallbackData->pMessage);
    assert(false);
  }

  // 是否中断函数调用
  return raise_exception ? VK_TRUE : VK_FALSE;
}

bool VulkanApplication::is_extension_support(
    const std::vector<const char *> &extensions) {

  uint32_t instance_extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count,
                                         nullptr);
  if (instance_extension_count == 0) {
    return false;
  }

  std::vector<VkExtensionProperties> available_instance_extensions(
      instance_extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count,
                                         available_instance_extensions.data());

  for (const char *extension_name : extensions) {
    bool layerFound = false;

    for (const auto &extension : available_instance_extensions) {
      if (strcmp(extension_name, extension.extensionName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }
  return true;
}

bool VulkanApplication::is_layer_support(
    const std::vector<const char *> &layers) {
  if (layers.empty()) {
    return false;
  }

  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  if (layerCount == 0) {
    return false;
  }

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layer_name : layers) {
    bool layerFound = false;

    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layer_name, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }
  return true;
}

void VulkanApplication::get_required_layers(
    std::vector<const char *> &requiredLayers) {

  if (m_enable_validation_layers) {
    std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation",
        // "VK_LAYER_LUNARG_monitor",  // 在mac上没找到
        // "VK_LAYER_LUNARG_api_dump",
        // "VK_LAYER_KHRONOS_profiles",
        "VK_LAYER_KHRONOS_synchronization2", "VK_LAYER_KHRONOS_shader_object"};

    if (!is_layer_support(validationLayers)) {
      printf("debug validation layer not support!\n");
      m_enable_validation_layers = false;
    } else {
      requiredLayers.insert(requiredLayers.end(), validationLayers.begin(),
                            validationLayers.end());
    }
  }
}

void VulkanApplication::get_required_extensions(
    std::vector<const char *> &required_extensions) {

  required_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(OS_WIN)
  required_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(OS_MAC)
  required_extensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
  // + "VK_EXT_metal_surface"
#endif

  if (m_enable_validation_layers) {
    // 启用了该extension才能获取到 vkCreateDebugUtilsMessengerEXT 函数
    required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
}

bool VulkanApplication::create_vulkan_instance() {
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

  VkApplicationInfo application_info = {};
  application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  application_info.pApplicationName = "UIGPU";
  application_info.applicationVersion = 1;
  application_info.apiVersion = VK_API_VERSION_1_0;
  application_info.pEngineName = "Vulkan Graphics Engine";
  application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  create_info.pApplicationInfo = &application_info;

  std::vector<const char *> required_layers;
  get_required_layers(required_layers);

  std::vector<const char *> required_extensions;
  get_required_extensions(required_extensions);
  if (!is_extension_support(required_extensions)) {
    printf("is_extension_support failed");
    return false;
  }

  create_info.enabledLayerCount = required_layers.size();
  create_info.ppEnabledLayerNames = required_layers.data();
  create_info.enabledExtensionCount = required_extensions.size();
  create_info.ppEnabledExtensionNames = required_extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
  if (m_enable_validation_layers) {
    debug_create_info.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
    debug_create_info.pfnUserCallback = vulkanLogCallback;
    debug_create_info.pUserData = nullptr;

    // 为vkCreateInstance这个调用过程输出日志！
    // 对其它API调用无效。其它API需要通过调用vkCreateDebugUtilsMessengerEXT启用。
    create_info.pNext =
        (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
  }

#if defined(OS_MAC)
  // 修正MACOS上的SDK BUG。
  // if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    required_extensions.push_back(
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    required_extensions.push_back("VK_KHR_get_physical_device_properties2");

    create_info.enabledExtensionCount = required_extensions.size();
    create_info.ppEnabledExtensionNames = required_extensions.data();

    // result = vkCreateInstance(&create_info, nullptr, &m_vk_instance);
  // }
#endif
  VkResult result = vkCreateInstance(&create_info, nullptr, &m_vk_instance);
  if (result != VK_SUCCESS) {
    printf("vkCreateInstance failed, result = %d\n", result);
    return false;
  }

  // 上面的debug_create_info仅是开启了vkCreateInstance的日志。
  // 因为vkGetInstanceProcAddr依赖于VkInstance，导致创建vkInstance时，
  // 还没有debug_messenger，所以要分开处理。
  if (m_enable_validation_layers) {
    // 必须先启用VK_EXT_DEBUG_UTILS_EXTENSION_NAME，否则这里拿不到该函数。
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        m_vk_instance, "vkCreateDebugUtilsMessengerEXT");
    VkResult result =
        func(m_vk_instance, &debug_create_info, nullptr, &m_debug_messenger);
    if (result != VK_SUCCESS) {
      printf("vkCreateDebugUtilsMessengerEXT failed, result = %d\n", result);
    }
  }
  return true;
}

} // namespace ui
