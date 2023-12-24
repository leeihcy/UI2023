
## git submodule

### skia

```sh
# 创建
$ git submodule add git@github.com:google/skia.git ui/3rd/skia/src
# 其他电脑
$ git submodule update --init --force

# 分支
$ git checkout -b m108 origin/chrome/m108
```

## 编译 

### gn_toolchain

引用了开源gn_toolchain https://github.com/Streamlet/gn_toolchain
```sh
# 创建
$ git submodule add git@github.com:Streamlet/gn_toolchain.git build
# 其他电脑
$ git submodule update --init --force

```

基于 https://github.com/Streamlet/gn_toolchain 搭建的 ninja toolchain

### 编译skia
https://skia.org/docs/user/build/

windows: 
skia_use_system_libjpeg = false

mac dylib debug:
```js
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

mac  dylib release:
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

编译命令： ninja skia

### ui
下载GNU gcc g++: https://github.com/niXman/mingw-builds-binaries/releases
找到 x86_64-win32-seh 链接下载到7z包，解压开，将bin配置到系统PATH即可。

```sh
cd out/debug
gn gen . --args="is_debug=true"
ninja
```

## 开发环境配置

1. VSCode
2. 扩展：clangd 
   用于实现语法提示
   Clangd: Fallback Flags添加：
  ```
  --std=c++17
  -I/<替换为本地的工程目录>/ui
  ```
  重启vscode

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

