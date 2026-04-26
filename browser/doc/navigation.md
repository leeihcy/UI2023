## NavigationEntry

● FrameNavigationEntry 和 NavigationEntry 的关系

  基于这两个类的定义，它们的关系可以概括为：

  1. 容器与元素的关系

  ┌──────────────────────┬───────────────────────────────────┬────────────────────┐
  │          类          │               作用                 │        类比        │
  ├──────────────────────┼───────────────────────────────────┼────────────────────┤
  │ NavigationEntry      │ 表示整个页面/标签页的一次导航状态    │ 一个文件夹          │
  ├──────────────────────┼───────────────────────────────────┼────────────────────┤
  │ FrameNavigationEntry │ 表示单个 iframe/frame的导航历史项   │ 文件夹里的一个文件  │
  └──────────────────────┴───────────────────────────────────┴────────────────────┘

  2. 树形结构

  NavigationEntry (整个页面的导航条目)
  ├── FrameNavigationEntry (主帧 frame)
  ├── FrameNavigationEntry (frame1)
  └── FrameNavigationEntry (frame2)

  正如注释所说：

  ▎ "There is a tree of FrameNavigationEntries in each NavigationEntry, one per frame."

  每个 NavigationEntry 中包含一棵 FrameNavigationEntry 的树，每个 frame 对应一个。


## NavigationController

每个 WebContents 对应一个 NavigationController，严格说，是WebContents的primary frame tree对应一个 NavigationController。

生命周期：

```cpp
const NavigationControllerImpl& WebContentsImpl::GetController() const {
  return primary_frame_tree_.controller();
}
```

Primary vs non-primary frame trees：
Primary Frame Tree：当前显示在地址栏的那个页面对应的FrameTree。
Primary page 会随着 Navigation 改变，如加载新页面，或者 Back/Forward。
可以理解为一个 WebContents 所持有的、用于代表当前正在标签页中活跃显示的那个“主页面”的树。
它是一个特定指向的概念，明确了在当前 WebContents 持有的可能多个 Frame Tree（例如用于预渲染或存在“后退/前进缓存”的页面）中，
只有被标记为 Primary 的树才是用户正在交互的

>
> frame trees doc:
>
> src\docs\frame_trees.md
>

**Relationships between core classes in content**
A WebContents represents a tab. A WebContents owns a FrameTree, the "primary frame tree," for what is being displayed in the tab. A WebContents may indirectly own additional FrameTrees for features such as prerendering.

A FrameTree consists of FrameTreeNodes. A FrameTreeNode contains a RenderFrameHost. FrameTreeNodes reflect the frame structure in the renderer. RenderFrameHosts represent documents loaded in a frame (roughly, see footnote 3). As a frame navigates its RenderFrameHost may change, but its FrameTreeNode stays the same.

In the case of nested frame trees, the RenderFrameHost corresponding to the hosting document owns the inner FrameTree (possibly through an intermediate object, as is the case for content::FencedFrame).

┌─────────────────────────────────────────────────────────────────────────────┐
│                              WebContents                                     │
│                                                                             │
│  ┌──────────────────────────────────────────────────────────────────────┐  │
│  │                         NavigationController                          │  │
│  │                      (负责管理导航和会话历史)                           │  │
│  │                                                                       │  │
│  │   entries_: vector<NavigationEntry>                                   │  │
│  │   ┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐        │  │
│  │   │ NavigationEntry │ │ NavigationEntry │ │ NavigationEntry │ ...    │  │
│  │   │ (index 0)       │ │ (index 1)       │ │ (index 2)       │        │  │
│  │   │  * url          │ │  * url          │ │  * url          │        │  │
│  │   │  * title        │ │  * title        │ │  * title        │        │  │
│  │   │  * page_state   │──┼──→ (序列化的    │ │  * timestamp    │        │  │
│  │   │  * timestamp    │ │    FrameTree    │ └─────────────────┘        │  │
│  │   └─────────────────┘ │    状态)         │                            │  │
│  │         ▲             └─────────────────┘                            │  │
│  │         │ last_committed_entry_index = 2 (当前页面)                   │  │
│  │         │ pending_entry_index (正在加载的页面)                         │  │
│  │         │                                                            │  │
│  └─────────┼────────────────────────────────────────────────────────────┘  │
│            │                                                               │
│            │ 当前显示的页面对应哪个 NavigationEntry？                         │
│            │                                                               │
│  ┌─────────▼────────────────────────────────────────────────────────────┐  │
│  │                    Primary Frame Tree (当前活动)                       │  │
│  │                                                                       │  │
│  │   ┌─────────────────────────────────────────────────────────────┐    │  │
│  │   │ FrameTreeNode A (main frame, frame_tree_node_id = 1)        │    │  │
│  │   │     │                                                        │    │  │
│  │   │     ├── FrameTreeNode B (iframe, frame_tree_node_id = 2)     │    │  │
│  │   │     │                                                        │    │  │
│  │   │     └── FrameTreeNode C (iframe, frame_tree_node_id = 3)     │    │  │
│  │   └─────────────────────────────────────────────────────────────┘    │  │
│  │                                                                       │  │
│  │   每个 FrameTreeNode 当前包含一个 RenderFrameHost                      │  │
│  └──────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
│  ┌──────────────────────────────────────────────────────────────────────┐  │
│  │                 Additional Frame Trees (非 Primary)                   │  │
│  │                                                                       │  │
│  │   ┌─────────────────────┐  ┌─────────────────────┐                   │  │
│  │   │ Prerendering Tree   │  │ BackForwardCache    │                   │  │
│  │   │ (预渲染页面)         │  │ Tree (缓存页面)      │                   │  │
│  │   │ 对应 NavigationEntry?│  │ 对应 NavigationEntry│                   │  │
│  │   │ (尚未提交)          │  │ (已缓存的历史页面)    │                   │  │
│  │   └─────────────────────┘  └─────────────────────┘                   │  │
│  └──────────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────────┘

WebContents
    │
    ├── 拥有 0~N 个 FrameTree (主要 + 额外)
    │       │
    │       └── FrameTree 由 FrameTreeNode 组成
    │               │
    │               └── 每个 FrameTreeNode 包含一个 RenderFrameHost
    │                       │
    │                       └── 导航时 RenderFrameHost 可替换，FrameTreeNode 不变
    │
    └── 嵌套情况：外层 RenderFrameHost 拥有内层整个 FrameTree


核心概念对比表
概念	是什么	生命周期	是否可变
WebContents	一个浏览器标签页	从标签页创建到关闭	——
FrameTree	一棵帧树，描述一个页面的完整帧结构	随页面创建/销毁	整棵树可替换
FrameTreeNode	帧树中的一个节点，代表一个“帧槽位”	随 FrameTree 创建，永不改变	❌ 不可变
RenderFrameHost	实际加载在帧中的文档	随导航变化（同站点复用，跨站点新建）	✅ 可变

通俗理解：
FrameTreeNode = 酒店的房间号（固定不变）
RenderFrameHost = 住进去的客人（客人可以换）

## NavigationHandle 

NavigationHandle 是 Chromium 中用于跟踪和管理单次导航过程的核心接口。可以把它理解为一次导航的“通行证”或“生命周期管理器”。
一句话：NavigationHandle 代表一次正在进行的导航，包含了这次导航的所有上下文信息。




## SiteInstance

┌─────────────────────────────────────────────────────────────────┐
│                      BrowsingInstance                           │
│                                                                 │
│   ┌─────────────────┐  ┌─────────────────┐  ┌───────────────┐  │
│   │  SiteInstance   │  │  SiteInstance   │  │  SiteInstance │  │
│   │  (a.com)        │  │  (b.com)        │  │  (c.com)      │  │
│   │                 │  │                 │  │               │  │
│   │ 绑定到同一个     │  │ 绑定到同一个     │  │ 绑定到同一个   │  │
│   │ BrowsingInstance│  │ BrowsingInstance│  │ BrowsingInstance│
│   └─────────────────┘  └─────────────────┘  └───────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
关键理解：SiteInstance 和 BrowsingInstance 之间是多对一的关系：

一个 BrowsingInstance 包含多个 SiteInstance

一个 SiteInstance 只属于一个 BrowsingInstance

给定一个 SiteInstance，你可以唯一确定它属于哪个 BrowsingInstance



## did_create_new_entry

```cpp
  // "Standard" commits from Blink create new NavigationEntries. We also treat
  // main frame "inert" commits as creating new NavigationEntries if they
  // replace the current entry on a cross-document navigation (e.g., client
  // redirects, location.replace, navigation to same URL), since this will
  // replace all the subframes and could go cross-origin. We don't want to rely
  // on updating the existing NavigationEntry in this case, since it could leave
  // stale state around.
  params->did_create_new_entry =
      (commit_type == blink::kWebStandardCommit) ||
      (commit_type == blink::kWebHistoryInertCommit && !frame_->Parent() &&
       document_loader->ReplacesCurrentHistoryItem() &&
       !navigation_state->WasWithinSameDocument());
```

```cpp 
 
  // renderer process:
>	content.dll!content::RenderFrameImpl::MakeDidCommitProvisionalLoadParams 行 4953	C++
 	content.dll!content::RenderFrameImpl::DidCommitNavigationInternal 行 5238	C++
 	content.dll!content::RenderFrameImpl::DidCommitNavigation 行 4023	C++
 	blink_core.dll!blink::LocalFrameClientImpl::DispatchDidCommitLoad 行 555	C++
 	blink_core.dll!blink::DocumentLoader::CommitNavigation 行 3150	C++
 	blink_core.dll!blink::FrameLoader::CommitDocumentLoader 行 1452	C++
 	blink_core.dll!blink::FrameLoader::CommitNavigation 行 1262	C++
 	blink_core.dll!blink::WebLocalFrameImpl::CommitNavigation 行 2815	C++
 	content.dll!content::RenderFrameImpl::CommitNavigationWithParams 行 3004	C++
  ...
 	content.dll!content::RenderFrameImpl::CommitNavigation 行 2861	C++
 	content.dll!content::NavigationClient::CommitNavigation 行 87	C++

  // -----------------------------------
  // browser process
  content.dll!content::RenderFrameHostImpl::CommitNavigation 行 9741	C++
 	content.dll!content::NavigationRequest::CommitNavigationForIE 行 5139	C++
 	content.dll!content::NavigationRequest::BeginNavigationImpl 行 2313	C++
 	content.dll!content::NavigationRequest::BeginNavigation 行 2112	C++
```