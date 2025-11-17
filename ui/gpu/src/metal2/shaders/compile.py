import os
import sys
import subprocess

"""
 编译metal文件

 示例： python3 compile.py /tmp/
 输出： metal2.air,  metal2.metallib
"""

output_dir = sys.argv[1]
script_dir = os.path.dirname(os.path.abspath(__file__))

def run(commands: list[str]):
    print(" ".join(commands))
    ret = subprocess.run(commands, cwd=script_dir,
                         stdout=subprocess.PIPE, 
                         stderr=subprocess.PIPE)
    output = ret.stdout.decode("utf-8") + ret.stderr.decode("utf-8") 
    if output:
        print(output, end='')
    if ret.returncode:
        exit(ret.returncode)

if not os.path.exists(output_dir):
    os.makedirs(output_dir)

# xcrun -sdk macosx metal metal2.metal -c -o metal2.air
run(["xcrun", "-sdk", "macosx", "metal", "metal2.metal", 
     "-c", "-o", 
     os.path.join(output_dir, "metal2.air")])

# xcrun -sdk macosx metallib YourShader.air -o YourShader.metallib
run(["xcrun", "-sdk", "macosx", "metallib",
     os.path.join(output_dir, "metal2.air"), 
     "-o", 
     os.path.join(output_dir, "metal2.metallib")])

