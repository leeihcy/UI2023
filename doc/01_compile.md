
## 搭建编译环境

运行命令拉取编译环境，将下载编译所需要的各种依赖文件。
注意：可能需要搭梯子。

1. 安装python sdk

   https://www.python.org/downloads/windows/

2. 安装vulkan sdk
  https://vulkan.lunarg.com/sdk/home
  例如：https://sdk.lunarg.com/sdk/download/1.4.313.0/linux/vulkansdk-linux-x86_64-1.4.313.0.tar.xz


```sh
$ python3 script/init.py
```





## 编译 


linux:
```sh
  sudo apt install pkg-config
  sudo apt install libglib2.0-dev

  # vulkan
  # sudo apt install glslc
  sudo apt install libglm-dev
  
  # skia 编译依赖
  sudo apt install libfreetype6-dev
  sudo apt install libfontconfig1-dev
  sudo apt install libgl1-mesa-dev  # 包含 OpenGL 和 GLX 头文件
```

设置PATH环境变量

windows:

```sh
> call script/ent.bat
```

linux/mac：

```sh
$ source script/ent.sh
```



### 编译skia

https://skia.org/docs/user/build/

```sh
# 使用depot tools中的gclient更新skia依赖库
$ cd third_party/skia/
$ gclient config --unmanaged --name=src https://github.com/google/skia.git
# 耗时比较久
$ gclient sync
```

错误处理：
1. gclient sync提示src/third_party/externals/piet-gpu error

piet-gpu仓库已过期，找到 skia/src/DEPS文件，替换"third_party/externals/piet-gpu"对应的仓库地址为：https://github.com/linebender/vello.git

2. gn gen 报错： gn.py: Could not find gn executable at

deps中没有添加gn的依赖，可以直接拷贝一个gn文件到 skia/src/buildtools/linux64/gn 或者 skia/src/buildtools/mac/gn 位置即可。

```sh
#windows
copy build_tools\windows\gn.exe third_party\skia\src\buildtools\win\gn.exe
#linux
cp build_tools/linux/gn third_party/skia/src/buildtools/linux/gn
#mac
cp build_tools/mac/gn third_party/skia/src/buildtools/mac/gn
```



各平台对应的args.gn配置：

* windows static lib debug:

```sh
skia_use_system_libjpeg = false
is_debug = true
symbol_level = 2
```

* windows shared lib debug:

```sh
skia_use_system_libjpeg = false
is_component_build = true
is_debug = true
symbol_level = 2
```



* mac dylib debug:

```sh
is_official_build = false
is_component_build = true
is_debug = true
symbol_level = 2
target_cpu="x64"
skia_use_system_expat = false
skia_use_system_harfbuzz = false
skia_use_system_icu = false
skia_use_system_libjpeg_turbo = false
skia_use_system_libpng = false
skia_use_system_libwebp = false
skia_use_system_zlib = false
skia_enable_skottie = true
```

* mac dylib release:
   
```js
is_official_build = true
is_component_build = true
is_debug = false
target_cpu="x64"
skia_use_system_expat = false
skia_use_system_harfbuzz = false
skia_use_system_icu = false
skia_use_system_libjpeg_turbo = false
skia_use_system_libpng = false
skia_use_system_libwebp = false
skia_use_system_zlib = false
skia_enable_tools = true
```

Mac TBD:     skia_use_metal=true


* linux debug： 

```sh
$ cd third_party/skia/src
$ mkdir -p out/debug
$ cd out/debug
$ vi args.gn
$ gn gen .
$ ninja skia
```



### 编译swiftshader

TODO: skia external中自带了swiftshader，可以考虑直接复用。

```sh
cd third_party/swiftshader

cd build
cmake ..
cmake --build . --parallel

./vk-unittests

```



### ui

下载GNU gcc g++: https://github.com/niXman/mingw-builds-binaries/releases
找到 x86_64-win32-seh 链接下载到7z包，解压开，将bin配置到系统PATH即可。

```sh
$ cd out/debug
$ gn gen . --args="is_debug=true"
$ ninja
```


### Windows下头文件依赖问题
在windows下，有时修改头文件或者某个cpp时，不能很好的触发编译。

1. 添加/showIncludes参数：

gn/ninja使用msvc工具链编译时，为了获取每个cpp文件依赖的头文件，会给cl.exe命令添加参数：/showIncludes
在编译过程中会输出这个文件依赖的所有头文件：

```log
注意： 包含文件：include file path
```

ninja会拦截这个输出进行过滤，得到依赖项。

2. 资源替换

目前的ninja只支持过滤英文的输出，即：
```log
Note: including file:%s%s\n
```
所以在编译时拦截失败，导致输出了大量的 "注意： 包含文件："。
目前没有较好的办法强制输出英文，当前采用的方法是直接替换原始文件：
将
`C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\1033\clui.dll`
替换到
`C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\2052\clui.dll`


## submodule

配置git pull时，默认不更新submodule:
```sh
git config --global submodule.update none
```

### depot_tools

depot_tools 用于编译skia

```sh
$ git submodule add https://chromium.googlesource.com/chromium/tools/depot_tools third_party/depot_tools
```

### mini_chromium

base库 和 gn编译工具链

```sh
# 需要翻墙
$ git submodule add https://chromium.googlesource.com/chromium/mini_chromium third_party/mini_chromium
```

### skia

2D渲染库

```sh
# 创建
$ git submodule add https://github.com/google/skia.git ui/3rd/skia/src

# 其他电脑
$ git submodule update --init --force

# 分支
$ git checkout -b m108 origin/chrome/m108
```

~~### swifter shader~~
~~CPU实现的Vulkan~~
TODO: Skia中自带了swiftshader.

```sh
# 需要翻墙
$ git submodule add https://swiftshader.googlesource.com/SwiftShader third_party/swiftshader
```

### 其它

手动修改.gitmodules文件后，需要执行git submodule sync命令才能生效。



## 开发环境配置

1. VSCode
2. 语法扩展：clangd 
   用于实现语法提示
   Clangd: Fallback Flags添加：
  ```
  --std=c++17
  -I/<替换为本地的工程目录>/ui
  ```
  重启vscode

3. 调试扩展：lldb




## 调试
1. VSCode 扩展CodeLLDB。
   调试配置示例：
  ```json
   #.vscode/launch.json
   {
    "configurations": [
      {
        "type": "lldb",
        "request": "launch",
        "name": "sample_svg",
        "program": "${workspaceFolder}/out/debug/sample_svg",
        "args": [],
        "cwd": "${workspaceFolder}/out/debug"
      }
    ]
  }
  ```
2. Windows下直接使用Visual Studio加载对应的.exe进行调试即可。
3. 不推荐使用VSCode c/c++扩展，经常出现cpu占满的情况。

