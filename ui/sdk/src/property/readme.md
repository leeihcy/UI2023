# Property

## 目标

减小每个Object对象的size，用于支撑窗口/页面中分配使用大量的object。

1. 每个属性要有一个唯一数字编码id，用于快速定位，减少使用map，能用数组定位就用数组定位。

2. 所有属性的默认值静态存储，共享。

3. 支持多优先级架构。 XML配置 > 继承 > 默认值

4. 可继承父结点属性，如字体属性。

5. 支持跨模块使用，每个模块可以定义自己的属性。


### 目标2 -- CSS

> https://developer.mozilla.org/en-US/docs/Web/CSS/Guides/Cascade/Introduction

**来源（origin types）**: 

* User-agent stylesheets

  浏览器内置的样式表

* Author stylesheets

  页面使用的样式表

* User stylesheets

  扩展、用户自定义的样式表。

优先级： Author stylesheets > User stylesheets > User-agent stylesheets


| Precedence Order (low to high) | Origin | Importance |
| - | - | - |
| 1  | user-agent (browser)	normal
| 2 | user | normal |
| 3 | author (developer) | normal |
| 4 | CSS keyframe animations| | 
| 5 | author (developer) | !important |
| 6 | user | !important |
| 7 | user-agent (browser) | !important |
| 8 | CSS transitions|  |

> Note: Transitions and animations
>
> Property values set by animation @keyframes are more important than all normal styles (those with no !important set).
>
> Property values being set in a transition take precedence over all other values set, even those marked with !important.

**特异性（Specificity）**

特异性是CSS给选择器打的一个权重分数，用于决定同一来源层叠级别中哪个规则胜出。

想象特异性是一个 4位数：[A, B, C, D]，逐步计算，不进位

|   部分   | 对应内容 | 常见例子 |
| A（千位）| 行内样式	|  style="..."  |
| B（百位）| ID选择器的数量	|  #header  |
| C（十位）| 类、属性、伪类的数量	| .class, [type="text"], :hover |
| D（个位）| 元素、伪元素的数量	| p, div, ::before  |

```css
/* 例子1 */
p {}                     /* 特异性 = 0,0,0,1 → 001 */
/* 例子2 */
.text p {}               /* 0,0,1,1 → 011 */
/* 例子3 */
#header .text a:hover {} /* 0,1,2,1 → 0121 */
/* 例子4 */
style="color:red"        /* 1,0,0,0 → 1000 (行内样式) */
```

**特异性 vs 层叠**

顺序：

. 重要性（!important）
. 来源（作者 > 用户 > 浏览器默认）
. 特异性（分数比较）
. 源代码顺序（后定义覆盖前定义）
. 只有当来源相同时，才比较特异性。

## 术语

* attribute

  是 xml/html 结点中的key/value键值对，是字符串类型

* property
  
  是attribute解析后的对象属性，可以是任意类型



## 演进

1. DefaultValue/InitializeValue （done）

2. 模板对象如何实现。多个对象共用一套样式，如列表中的所有行的按钮等。

3. 