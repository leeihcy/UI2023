# 编译环境搭建脚本
# 运行命令： python3 sync.py

import os
import sys
import requests  
import zipfile

class Context:
  def __init__(self) -> None:
    self.build_tools_dir = None
    
    # build_tools下的目录：
    self.linux_dir = None
    self.windows_dir = None
    self.mac_dir = None

ctx = Context()


def initialize():
  file_path = os.path.abspath(__file__)
  root_dir = os.path.dirname(os.path.dirname(file_path))
  ctx.build_tools_dir = os.path.join(root_dir, "build_tools")
  
  ctx.linux_dir = os.path.join(ctx.build_tools_dir, "linux")
  ctx.windows_dir = os.path.join(ctx.build_tools_dir, "windows")
  ctx.mac_dir = os.path.join(ctx.build_tools_dir, "mac")

  if not os.path.exists(ctx.linux_dir):
    os.mkdir(ctx.linux_dir)
  if not os.path.exists(ctx.windows_dir):    
    os.mkdir(ctx.windows_dir)
  if not os.path.exists(ctx.mac_dir):
    os.mkdir(ctx.mac_dir)

  os.chdir(ctx.build_tools_dir)


# 下载一个文件到指定目录
def download(url, save_path):
  print(f"正在下载：{url}")
  response = requests.get(url, stream=True)
  if response.status_code != 200:
      print('下载失败，status code: ', response.status_code)
      return False
  
  with open(save_path, 'wb') as f:  
    for chunk in response.iter_content(chunk_size=4*1024):
      if chunk: 
        f.write(chunk) 
        f.flush() 

  print(f'下载完成：{save_path}')  
  return True


def sync_gn():
  # 不下载代码了，直接下载二进制文件
  # if not os.path.exists("gn"):
  #   os.system("git clone https://gn.googlesource.com/gn")

  # 这里只下载x64的版本，其它架构的后续再考虑
  # https://chrome-infra-packages.appspot.com/p/gn/gn
  gn_download_list = [
    (ctx.linux_dir, "https://chrome-infra-packages.appspot.com/dl/gn/gn/linux-amd64/+/latest", "gn"),
    (ctx.windows_dir, "https://chrome-infra-packages.appspot.com/dl/gn/gn/windows-amd64/+/latest", "gn.exe"),
    (ctx.mac_dir, "https://chrome-infra-packages.appspot.com/dl/gn/gn/mac-amd64/+/latest", "gn"),
  ]
  for i in gn_download_list:
    path = os.path.join(i[0], i[2])
    if os.path.exists(path):
      continue

    zip_path = os.path.join(i[0], "gn.zip")
    if not download(i[1], zip_path):
      continue

     # 解压zip
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extract(i[2], i[0])

     # 删除下载的zip文件
    os.unlink(zip_path)
    
    if os.name == 'posix' and not path.endswith(".exe"):
      os.system(f"chmod a+x {path}")
      

def sync_ninja():
  # 不下载代码了，直接下载二进制文件
  # if not os.path.exists("ninja"):
  #   os.system("git clone git://github.com/ninja-build/ninja.git")
  #   os.system("cd ninja && git checkout release")

  # 这里只下载x64的版本，其它架构的后续再考虑
  version = "v1.12.1"
  gn_download_list = [
    (ctx.linux_dir, f"https://github.com/ninja-build/ninja/releases/download/{version}/ninja-linux.zip", "ninja"),
    (ctx.windows_dir, f"https://github.com/ninja-build/ninja/releases/download/{version}/ninja-win.zip", "ninja.exe"),
    (ctx.mac_dir, f"https://github.com/ninja-build/ninja/releases/download/{version}/ninja-mac.zip", "ninja"),
  ]
  for i in gn_download_list:
    path = os.path.join(i[0], i[2])
    if os.path.exists(path):
      continue

    zip_path = os.path.join(i[0], "ninja.zip")
    if not download(i[1], zip_path):
      continue

    # 解压zip
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extractall(i[0])

    # 删除下载的zip文件
    os.unlink(zip_path)

    if os.name == 'posix' and not path.endswith(".exe"):
      os.system(f"chmod a+x {path}")

def main():
  sync_gn()
  sync_ninja()


if __name__ == "__main__":
  initialize()
  main()