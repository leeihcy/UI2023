
## DirectX SDK下载

https://download.microsoft.com/download/A/E/7/AE743F1F-632B-4809-87A9-AA1BB3458E31/DXSDK_Jun10.exe

### Error Code: S1023
DirectX SDK 安装失败。

**错误原因**

DirectX SDK 安装程序尝试安装较低版本的 Microsoft Visual C++ 2010 Redistributable（如 10.0.30319），但系统中已存在更高版本（如 10.0.40219）。因此，安装冲突导致错误。

**解决方法**

1. 卸载冲突的 Visual C++ Redistributable

打开 控制面板，进入 程序和功能，找到以下两个程序并卸载：

Microsoft Visual C++ 2010 x86 Redistributable
Microsoft Visual C++ 2010 x64 Redistributable

2. 安装 DirectX SDK

卸载完成后，重新运行 DirectX SDK 的安装程序（如 DXSDK_Jun10.exe）。此时应能正常完成安装。
