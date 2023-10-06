> https://zhuanlan.zhihu.com/p/430397192
>
在Vulkan里，没有任何API允许你直接的，立即的像GPU发出任何命令。所有的命令，包括渲染的Draw Call，计算的调用，甚至内存的操作例如资源的拷贝，都需要通过App自己创建的Command Buffer。

另一个值得注意的是，为了让驱动能更加简易的优化这些Command的调用，没有任何渲染状态会在Command Buffer之间继承下来。每一个Command Buffer都需要显式的绑定它所需要的所有渲染状态，Shader，和Descriptor Set等等。这和传统API中，只要你不改某个状态，某个状态就一直不会变，这一点很不一样.

Queue - 是Vulkan中唯一给GPU递交任务的渠道.