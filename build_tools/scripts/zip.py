#!/usr/bin/env python3
import argparse
import os
import zipfile
import sys

"""
python3 build_tools/scripts/zip.py --output /tmp/111.zip --base-dir=/.../UI2023/ script/env.sh script/env.
bat 
Added: script/env.sh as script/env.sh
Added: script/env.bat as script/env.bat
Successfully created: /tmp/111.zip with 2 files
"""

def main():
    parser = argparse.ArgumentParser(description='Zip files while preserving directory structure')
    parser.add_argument('--output', required=True, help='Output zip file')
    parser.add_argument('--base-dir', help='Base directory for relative paths in zip')
    parser.add_argument('sources', nargs='+', help='Source files to zip')
    
    args = parser.parse_args()
    print("Zip " + args.base_dir)
    
    try:
        # 确保输出目录存在
        os.makedirs(os.path.dirname(args.output), exist_ok=True)
        
        with zipfile.ZipFile(args.output, 'w', zipfile.ZIP_DEFLATED) as zipf:
            for source in args.sources:
                if not os.path.exists(source):
                    print(f"Warning: File not found: {source}", file=sys.stderr)
                    continue
                    
                # 确定在 zip 中的路径名
                if args.base_dir:
                    # 相对于 base_dir 的路径
                    arcname = os.path.relpath(source, args.base_dir)
                else:
                    # 使用文件的完整路径，但去掉开头的 / 或 ./
                    arcname = os.path.normpath(source)
                    if arcname.startswith('./'):
                        arcname = arcname[2:]
                    elif arcname.startswith('/'):
                        # 对于绝对路径，我们只保留最后一部分
                        # 更好的做法是使用 base-dir
                        arcname = os.path.basename(source)
                
                # 确保路径使用正斜杠（zip 标准）
                arcname = arcname.replace('\\', '/')
                
                zipf.write(source, arcname)
                print(f"  Added: {arcname}")
        
        print(f"Successfully created: {args.output} with {len(args.sources)} files")
        
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()