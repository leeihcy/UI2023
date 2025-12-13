#include "src/property/property_id.h"
#include "src/property/property.h"

#include <map>

namespace ui {

// 属性注册表缓存区，全局共享。
static Property g_properties[UI_SDK_PROPERTY_COUNT];

extern "C" {
Property* uisdk_property_register = (Property*)&g_properties;
}
}