// 这几个宏只能在一个cpp文件中定义，不要放头文件中。
// 否则会出来重复定义。

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "src/metal2/inc.h"