# 设置编译所需要的相关环境变量
# 运行方法:
#   source script/env.sh
# 

if [ "$(uname)" = "Darwin" ]; then
  OS="mac"
else
  OS="linux"
fi

if [[ -n "$BASH_SOURCE" ]]; then
    # Bash
    SCRIPT_PATH="${BASH_SOURCE[0]}"
elif [[ -n "$ZSH_SCRIPT" ]]; then
    # Zsh ≥5.3
    SCRIPT_PATH="$ZSH_SCRIPT"
else
    # Fallback for older Zsh
    SCRIPT_PATH="${(%):-%x}"
fi
SCRIPT_DIR="$(cd "$(dirname "$SCRIPT_PATH")" && pwd)"
ROOT_DIR=$(dirname "$SCRIPT_DIR")

source "${ROOT_DIR}/script/local/env.${OS}.sh"

# depot tools 目前仅编译skia需要
DEPOT_TOOLS_DIR=${ROOT_DIR}/third_party/depot_tools

# 编译工具
GN_NINJA_DIR=${ROOT_DIR}/build_tools/${OS}

# vulkan
VULKAN_SDK_BIN_DIR=${VULKAN_SDK_DIR}/bin
export VULKAN_SDK_DIR
echo VULKAN_SDK_DIR = $VULKAN_SDK_DIR

# python
export PYTHON_INC_DIR
echo PYTHON_INC_DIR = $PYTHON_INC_DIR
export PYTHON_LIB_DIR
echo PYTHON_LIB_DIR = $PYTHON_LIB_DIR

export PATH=$DEPOT_TOOLS_DIR:$GN_NINJA_DIR:$VULKAN_SDK_BIN_DIR:$PATH
echo PATH updated.

echo ""
echo "Environment initialized for ${OS}"