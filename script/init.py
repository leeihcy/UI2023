import os
import subprocess
import platform

"""
编译环境初始化脚本。

requirments:
subprocess
"""

script_dir = os.path.dirname(os.path.abspath(__file__))

def run(command_line: []):
    print(' '.join(command_line), end=' ')

    result = subprocess.run(command_line, capture_output=True, text=True)

    print(f"==> Exit code {result.returncode}")
    if result.stdout:
        print(result.stdout, end='')
    if result.stderr:
        print(result.stderr, end='')

def init_local_env():
    """
    需要先配置本机SDK目录
    """
    print("# 检测本机ENV配置")
    system = platform.system()
    if system == "Windows":
        local_env_path = os.path.join(script_dir, "local", "env.win.bat")
    elif system in "Darwin":
        local_env_path = os.path.join(script_dir, "local", "env.mac.sh")
    elif system == "Linux":
        local_env_path = os.path.join(script_dir, "local", "env.linux.sh")
    
    if not os.path.exists(local_env_path):
        raise Exception("请先参照local_env.template，配置本地ENV到：" + local_env_path)

def init_git_third_party():
    print("# 初始化git submodule")
    run(["git", "submodule", "update", "--init", "--force"])

def init_gn_ninja():
    print("# 更新编译工具")
    sync_py_path = os.path.join(script_dir, 'download_gn_ninja.py')
    run(["python3", sync_py_path])

def main():
    init_local_env()
    init_git_third_party()
    init_gn_ninja()


if __name__ == "__main__":
    main()