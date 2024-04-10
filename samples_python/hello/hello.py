import os
import sys

bin_dir = os.path.abspath("../../out/debug")
sys.path.append(bin_dir)

# python只支持.so的扩展
# 将dylib重命名为so
if not os.path.exists(os.path.join(bin_dir, "uisdk.so")) and os.path.exists(os.path.join(bin_dir, "libuisdk.dylib")):
  os.symlink(os.path.join(bin_dir, "libuisdk.dylib"), os.path.join(bin_dir, "uisdk.so"))

cwd = os.getcwd()
os.chdir(bin_dir)
import uisdk
os.chdir(cwd)

def main():
  print(uisdk.ping())
  pass


if __name__ == "__main__":
  sys.exit(main())
