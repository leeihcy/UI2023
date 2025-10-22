#!/usr/bin/env python3
import argparse
import os

"""
递归枚举某个目录下的所有子文件，。

调用示例1:
python3 build_tools/scripts/tree.py --directory=/A/B/C
输出示例:
/A/B/C/D.txt
/A/B/C/E/F.txt

调用示例2:
python3 build_tools/scripts/tree.py --directory=/A/B/C --releative-to=/A/B/C
输出示例:
D.txt
E/F.txt
"""

def main():
    parser = argparse.ArgumentParser(description='Discover files in directory')
    parser.add_argument('--directory', required=True)
    parser.add_argument('--relative-to')
    
    args = parser.parse_args()
    
    files = []
    for root, dirs, filenames in os.walk(args.directory):
        for filename in filenames:
            # ignore .DS_Store on macos
            if filename.startswith("."):
                continue
            
            path = os.path.join(root, filename)
            if args.relative_to:
              path = os.path.relpath(path, args.relative_to)
            files.append(path)
    
    # 输出给 GN 的 exec_script
    for file in sorted(files):
        print(file)

if __name__ == '__main__':
    main()