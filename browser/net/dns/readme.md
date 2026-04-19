## 当配置了HTTP代理时，DNS的请求也会由代理进行解析。

HttpProxyConnectJob 会将 params_.destination_ 设置为代理的地址，例如
`{port_=7897 host_="127.0.0.1"}`，这也就会直接跳过DNS的解析

```cpp
 	net.dll!net::TransportConnectJob::TransportConnectJob 行 134	C++
 	net.dll!std::__Cr::make_unique<...> 行 756	C++
>	net.dll!net::HttpProxyConnectJob::DoTransportConnect 行 499	C++
 	net.dll!net::HttpProxyConnectJob::DoLoop 行 429	C++
 	net.dll!net::HttpProxyConnectJob::ConnectInternal 行 389	C++
 	net.dll!net::ConnectJob::Connect 行 130	C++
 	net.dll!net::SSLConnectJob::DoTunnelConnect 行 311	C++
 	net.dll!net::SSLConnectJob::DoLoop 行 216	C++
 	net.dll!net::SSLConnectJob::ConnectInternal 行 544	C++
 	net.dll!net::ConnectJob::Connect 行 130	C++
 	net.dll!net::TransportClientSocketPool::RequestSocketInternal 行 463	C++
 	net.dll!net::TransportClientSocketPool::RequestSockets 行 348	C++
 	net.dll!net::`anonymous namespace'::InitSocketPoolHelper 行 126	C++
```



## DNS Resolve 逻辑：

HostResolverManager::CreateTaskSequence 会初始化 DNS 请求队列，例如：
deque<net::HostResolverManager::TaskType>
		[0]	CACHE_LOOKUP (4)
		[1]	HOSTS (9)
		[2]	DNS (1)
		[3]	SYSTEM (0)

但是，是否用 启用DNS 类型，取决于 API DnsIsZtEnabled 

> DnsIsZtEnabled 
> 获取一个值，该值指定是否在当前设备上启用零信任 DNS （ZTDNS）。
> 命令行执行可进行查看：netsh ztdns show help
>
> 在86/108核中，features::kAsyncDns (built-in DNS resolver) 是默认关闭， 145核中默认开启。(从109 Windows默认开启的)
>
> 629950a2fe003db7d96667c2cb8e902faea63217
> Tsuyoshi Horo* 2022/11/2 5:32 Enable AsyncDns on Windows by default
> MAJOR=109
> MINOR=0
> BUILD=5396
> PATCH=0
>

```cpp
bool StubResolverConfigReader::GetInsecureStubResolverEnabled() {
  return local_state_->GetBoolean(prefs::kBuiltInDnsClientEnabled);
}

// Check the AsyncDns field trial and return true if it should be enabled. On
// Android this includes checking the Android version in the field trial.
bool ShouldEnableAsyncDns() {
#if BUILDFLAG(IS_WIN)
  // On Windows if Zero Trust DNS is enabled on current device,
  // we should not use built-in resolver (async dns). It should
  // always use system (OS) resolver.
  if (IsZTDNSEnabled()) {
    return false;
  }
#endif
  bool feature_can_be_enabled = true;
  return feature_can_be_enabled &&
         base::FeatureList::IsEnabled(net::features::kAsyncDns);
}


// Defines the base::Feature for controlling the ZTDNS check.
BASE_FEATURE(kZeroTrustDNS, base::FEATURE_ENABLED_BY_DEFAULT);

// DnsIsZtEnabled returns a BOOL value that specifies whether Zero
// Trust DNS (ZTDNS) is enabled on the current device.
using DnsIsZtEnabledFunc = BOOL (*)();

// Applicable to Windows OS.
// Returns true if Zero Trust DNS is enabled at the OS level.
// Returns false if Zero Trust DNS is either not enabled or unsupported.
bool IsZTDNSEnabled() {
  if (!base::FeatureList::IsEnabled(kZeroTrustDNS)) {
    return false;
  }

  // DnsIsZtEnabled returns a BOOL value that specifies whether Zero
  // Trust DNS (ZTDNS) is enabled on the current device.
  // There is no import library for this function, thus using native
  // dnsapi.dll library.
  const wchar_t* dll_name = L"dnsapi.dll";
  const char* function_name = "DnsIsZtEnabled";
  auto dns_api_dll = base::ScopedNativeLibrary(base::FilePath(dll_name));

  if (!dns_api_dll.is_valid()) {
    return false;
  }

  auto dns_is_zt_enabled_func = reinterpret_cast<DnsIsZtEnabledFunc>(
      dns_api_dll.GetFunctionPointer(function_name));

  if (!dns_is_zt_enabled_func) {
    return false;
  }
  return dns_is_zt_enabled_func();
}
```

调试发现，86核中，某个时机会通知network进程放开 insecure_resolver，这样就会启用内置的DNS解析。
触发时机：DNS解析失败时触发
```cpp
options.insecure_dns_client_enabled = true;
net进程：NetworkContext::CreateHostResolver
// ....
chrome.dll!chrome_browser_net::DnsProbeRunner::CreateHostResolver
//...
chrome.dll!chrome_browser_net::NetErrorTabHelper::StartDnsProbe()
chrome.dll!chrome_browser_net::NetErrorTabHelper::OnMainFrameDnsError()
```
会临时使用一次内置DNS。但后续的其它页面请求还是走系统DNS。


1. CACHE_LOOKUP

>	net.dll!net::HostCache::LookupInternal 行 847	C++
 	net.dll!net::HostCache::LookupInternalIgnoringFields 行 832	C++
 	net.dll!net::HostCache::Lookup 行 745	C++
 	net.dll!net::HostResolverManager::MaybeServeFromCache 行 1052	C++
 	net.dll!net::HostResolverManager::ResolveLocally 行 895	C++
 	net.dll!net::HostResolverManager::RequestImpl::DoResolveLocally 行 331	C++
>	net.dll!net::HostResolverManager::RequestImpl::DoLoop 行 252	C++
 	net.dll!net::HostResolverManager::RequestImpl::Start 行 96	C++
 	services_network_network_service.dll!network::ResolveHostRequest::Start 行 90	C++

HostCache::Lookup 读取到缓存后，再进一步判断 expires(TTL)，
如果获取到缓存则：HostResolverManager::RequestImpl::next_state_ = STATE_FINISH_REQUEST;

2. HOSTS

>	net.dll!net::HostResolverManager::ServeFromHosts 行 1113	C++
 	net.dll!net::HostResolverManager::ResolveLocally 行 921	C++
 	net.dll!net::HostResolverManager::ServiceEndpointRequestImpl::DoResolveLocally 行 349	C++
 	net.dll!net::HostResolverManager::ServiceEndpointRequestImpl::DoLoop 行 280	C++
 	net.dll!net::HostResolverManager::ServiceEndpointRequestImpl::Start 行 95	C++

session_->config()->hosts中加载了本地 hosts 文件中的内容，以及主机名。

如果获取失败，则 next_state_ = State::kStartJob;

3. DNS

>	net.dll!net::UDPSocketWin::SendToOrWrite 行 465	C++
 	net.dll!net::UDPSocketWin::Write 行 443	C++
 	net.dll!net::UDPClientSocket::Write 行 198	C++
 	services_network_network_service.dll!network::BrokeredUdpClientSocket::Write 行 236	C++
 	net.dll!net::`anonymous namespace'::DnsUDPAttempt::DoSendQuery 行 341	C++
 	net.dll!net::`anonymous namespace'::DnsUDPAttempt::DoLoop 行 301	C++
 	net.dll!net::`anonymous namespace'::DnsUDPAttempt::Start 行 258	C++
 	net.dll!net::`anonymous namespace'::DnsTransactionImpl::MakeUdpAttempt 行 1466	C++
 	net.dll!net::`anonymous namespace'::DnsTransactionImpl::MakeClassicDnsAttempt 行 1424	C++
 	net.dll!net::`anonymous namespace'::DnsTransactionImpl::MakeAttempt 行 1400	C++
 	net.dll!net::`anonymous namespace'::DnsTransactionImpl::StartQuery 行 1596	C++
 	net.dll!net::`anonymous namespace'::DnsTransactionImpl::Start 行 1272	C++
 	net.dll!net::HostResolverDnsTask::CreateAndStartTransaction 行 456	C++
 	net.dll!net::HostResolverDnsTask::StartNextTransaction 行 312	C++
 	net.dll!net::HostResolverManager::Job::StartDnsTask 行 722	C++
 	net.dll!net::HostResolverManager::Job::RunNextTask 行 457	C++
 	net.dll!net::HostResolverManager::Job::Start 行 611	C++
 	net.dll!net::PrioritizedDispatcher::Add 行 35	C++
 	net.dll!net::HostResolverManager::Job::Schedule 行 211	C++
 	net.dll!net::HostResolverManager::Job::RunNextTask 行 431	C++
 	net.dll!net::HostResolverManager::CreateAndStartJobForServiceEndpointRequest 行 996	C++
 	net.dll!net::HostResolverManager::ServiceEndpointRequestImpl::DoStartJob 行 421	C++
 	net.dll!net::HostResolverManager::ServiceEndpointRequestImpl::DoLoop 行 283	C++
 	net.dll!net::HostResolverManager::ServiceEndpointRequestImpl::Start 行 95	C++


系统配置了多 DNS 时，会同时发起多个 Attempt, session->config().nameservers是本地配置的DNS列表，例如：
fd00:ec60:730b:fda7:ee60:73ff:fe0b:fda7:53
192.168.18.1:53
192.168.0.1:53

net\dns\dns_server_iterator.cc

 HTTPS/IPv4 要查这两种 QueryType

> 为什么有时要向所有name servers发起请求，有时只向其中一个发请求。
> 
> 通过dns_server_iterator.cc查询到一个nameserver后，会开启一个定时器(1010ms?)：
> ```cpp
>  timer_.Start(FROM_HERE, fallback_period, this,
>               &DnsTransactionImpl::OnFallbackPeriodExpired);
> ```
> 当一个Attemp完成时（OnAttemptComplete）会将timer_.Stop();
> 即：只有当第一台DNS服务器在1秒内完全没有响应（例如服务器宕机、网络不通、丢包严重）时，才会自动转向第二台DNS服务器进行查询
>

>	net.dll!net::ClassicDnsServerIterator::GetNextAttemptIndex 行 132	C++
 	net.dll!net::`anonymous namespace'::DnsTransactionImpl::MakeClassicDnsAttempt 行 1416	C++
 	net.dll!net::`anonymous namespace'::DnsTransactionImpl::MakeAttempt 行 1400	C++
 	net.dll!net::`anonymous namespace'::DnsTransactionImpl::ProcessAttemptResult 行 1684	C++
 	net.dll!net::`anonymous namespace'::DnsTransactionImpl::OnFallbackPeriodExpired 行 1770	C++

> 还有一种可能性，就是走的 SYSTEM （PROC）类型的DNS解析。
>
> 使用 getaddrinfo api验证发现，确实是会出现同时发并请求多个DNS的现象，但再次请求时就只请求一个，内部原理未知。
>
> 
> 组策略-计算机配置-管理模板-网络-DNS客户端-禁用智能多宿主名称解析-勾选已禁用/未配置
> reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Tcpip6\Parameters" /v DisabledComponents /t REG_DWORD /d 0x20 /f
> #reg add "HKEY_LOCAL_MACHINE\Software\Policies\Microsoft\Windows NT\DNSClient" /v DisableSmartNameResolution /t REG_DWORD /d > 0x1 /f
> reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Dnscache\Parameters" /v DisableParallelAandAAAA /t REG_DWORD /d 0x1 /f

```cpp
// net\dns\dns_transaction.cc

AttemptResult MakeUdpAttempt(size_t server_index,
                             std::unique_ptr<DnsQuery> query) {
 const DnsConfig& config = session_->config();

 attempts_.push_back(std::make_unique<DnsUDPAttempt>(
     server_index, std::move(socket), config.nameservers[server_index],
     std::move(query), session_->udp_tracker()));
 ++attempts_count_;

 DnsAttempt* attempt = attempts_.back().get();

  int rv = attempt->Start(base::BindOnce(
    &DnsTransactionImpl::OnAttemptComplete, base::Unretained(this),
    attempt_number, true /* record_rtt */, base::TimeTicks::Now()));
  return AttemptResult(rv, attempt);
}
```

响应数据解析：

>	net.dll!net::`anonymous namespace'::ExtractResponseRecords 行 218	C++
 	net.dll!net::`anonymous namespace'::ExtractAddressResults 行 325	C++
 	net.dll!net::DnsResponseResultExtractor::ExtractDnsResults 行 652	C++
 	net.dll!net::HostResolverDnsTask::OnDnsTransactionComplete 行 561	C++
  // ...
 	net.dll!net::`anonymous namespace'::DnsTransactionImpl::DoCallback 行 1381	C++
	net.dll!net::`anonymous namespace'::DnsTransactionImpl::OnAttemptComplete 行 1615	C++
  // ...
 	net.dll!net::`anonymous namespace'::DnsUDPAttempt::OnIOComplete 行 396	C++


多个Attempt的结果是如何合并处理的？（内置DNS解析不会有多个Attempt并发的！）


4. SYSTEM

什么时候走系统API？
1. 在udp请求失败时，HostResolverManager将继续调用System Task继续解析。
2. 没有启用内置DNS Resolver的时候

```cpp
  ::getaddrinfo(host.c_str(), nullptr, hints, &ai);

>	net.dll!net::AddrInfoGetter::getaddrinfo 行 201	C++
 	net.dll!net::AddressInfo::Get 行 64	C++
 	net.dll!net::SystemHostResolverCall 行 612	C++
 	net.dll!net::`anonymous namespace'::ResolveOnWorkerThread 行 117	C++

  // ...

 	net.dll!net::HostResolverSystemTask::StartLookupAttempt 行 387	C++
 	net.dll!net::HostResolverSystemTask::Start 行 326	C++
 	net.dll!net::HostResolverManager::Job::StartSystemTask 行 637	C++
>	net.dll!net::HostResolverManager::Job::RunNextTask 行 454	C++    // case TaskType::SYSTEM:
 	net.dll!net::HostResolverManager::Job::OnDnsTaskFailure 行 774	C++
 	net.dll!net::HostResolverManager::Job::OnDnsTaskComplete 行 799	C++
 	net.dll!net::HostResolverDnsTask::OnSuccess 行 1040	C++
 	net.dll!net::HostResolverDnsTask::OnTransactionsFinished 行 917	C++
 	net.dll!net::HostResolverDnsTask::HandleTransactionResults 行 880	C++
 	net.dll!net::HostResolverDnsTask::SortTransactionAndHandleResults 行 754	C++
 	net.dll!net::HostResolverDnsTask::OnDnsTransactionComplete 行 653	C++

```

响应结果处理：

```cpp
void HostResolverManager::Job::OnSystemTaskComplete() {
  base::TimeDelta ttl = base::Seconds(kNegativeCacheEntryTTLSeconds);
  if (net_error == OK) {
    ttl = base::Seconds(kCacheEntryTTLSeconds);
  }

  // Source unknown because the system resolver could have gotten it from a
  // hosts file, its own cache, a DNS lookup or somewhere else.
  // Don't store the |ttl| in cache since it's not obtained from the server.
  CompleteRequests(
      HostCache::Entry(
          net_error,
          net_error == OK ? addr_list.endpoints() : std::vector<IPEndPoint>(),
          std::move(aliases), HostCache::Entry::SOURCE_UNKNOWN),
      ttl, /*allow_cache=*/true, /*secure=*/false, TaskType::SYSTEM);
}
```

对于 SYSTEM 调用系统API的结果，由于拿不到 ttl，代码里使用了默认的60s：
```cpp
// Default TTL for successful resolutions with HostResolverSystemTask.
const unsigned kCacheEntryTTLSeconds = 60;
```
但即使是这样，也不会有很大的问题。
chrome net内部，虽然每60s会导致cache失效，导致多次调用getadrinfo。然而windows api getaddrinfo内部会做缓存，在缓存期间多次调用 getaddrinfo 并不会重复发起DNS请求。从抓包层 

使用 ipconfig  /flushdns 可清除系统缓存。