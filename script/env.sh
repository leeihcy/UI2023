# 设置编译所需要的相关环境变量
# 运行方法:
#   source script/env.sh
# 

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
ROOT_DIR=$(dirname "$SCRIPT_DIR")

source "$ROOT_DIR/script/local/env.linux.sh"

# depot tools 目前仅编译skia需要
DEPOT_TOOLS_DIR=$ROOT_DIR/third_party/depot_tools

# 编译工具
GN_NINJA_DIR=$ROOT_DIR/build_tools/linux

# vulkan
VULKAN_SDK_BIN_DIR=$VULKAN_SDK_DIR/bin
export VULKAN_SDK_DIR

# python
export PYTHON_INC_DIR
export PYTHON_LIB_DIR

export PATH=$DEPOT_TOOLS_DIR:$GN_NINJA_DIR:$VULKAN_SDK_BIN_DIR:$PATH
