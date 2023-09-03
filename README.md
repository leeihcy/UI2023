# UI2023
an ui framework can cross platform 


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

## compile 

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

### ui
下载GNU gcc g++: https://github.com/niXman/mingw-builds-binaries/releases
找到 x86_64-win32-seh 链接下载到7z包，解压开，将bin配置到系统PATH即可。

gn gen out/debug --args="is_debug=true"
ninja
