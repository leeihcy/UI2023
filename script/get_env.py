import os
import sys

"""
用于在gn中，获取环境变量的值，使用示例：

VULKAN_SDK_DIR = exec_script("//script/get_env.py", ["VULKAN_SDK_DIR"], "string")
"""

var_name = sys.argv[1]
print(os.getenv(var_name, ""), end='')