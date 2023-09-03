import sys
import subprocess
import json

# 执行 pkg-config 命令，将输出结果以数组的形式返回给gn调用者
# 示例：
#    cflags += exec_script("build/pkg-config.py", ["--cflags", "glib-2.0"], "value")

flag_string = subprocess.check_output(["pkg-config"] + sys.argv[1:])
all_flags = str(flag_string, encoding="utf-8").strip().strip('\n').split(' ')

# 以"value"的形式接收返回值时，需要返回json格式
print(json.dumps(all_flags))
sys.exit(0)