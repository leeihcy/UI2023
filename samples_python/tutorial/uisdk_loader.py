import os
import sys
import importlib


def load():
  bin_dir = os.path.abspath("../../out/debug")
  sys.path.append(bin_dir)

  # python只支持.so的扩展
  # 将dylib重命名为so
  if not os.path.exists(os.path.join(bin_dir, "uisdk.so")) and os.path.exists(os.path.join(bin_dir, "libuisdk.dylib")):
    os.symlink(os.path.join(bin_dir, "libuisdk.dylib"), os.path.join(bin_dir, "uisdk.so"))

  cwd = os.getcwd()
  os.chdir(bin_dir)
  # import uisdk
  uisdk = importlib.import_module("uisdk")
  os.chdir(cwd)
  
  return uisdk


uisdk = load()
