import os
import sys
import subprocess

"""
 编译shader文件

 glslc需要包含在当前PATH环境变量中，如先运行script/env.sh
"""

output_dir = sys.argv[1]
script_dir = os.path.dirname(os.path.abspath(__file__))

def run(commands: [str]):
    ret = subprocess.run(commands, cwd=script_dir,
                         stdout=subprocess.PIPE, 
                         stderr=subprocess.PIPE)
    output = ret.stdout.decode("utf-8") + ret.stderr.decode("utf-8") 
    if output:
        print(output, end='')

if not os.path.exists(output_dir):
    os.makedirs(output_dir)

run(["glslc", "shader.vert", "-o", os.path.join(output_dir, "vert.spv")])
run(["glslc", "shader.frag", "-o", os.path.join(output_dir, "frag.spv")])
