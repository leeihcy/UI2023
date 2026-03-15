HTTP请示发起堆栈

```cpp
 	services_network_network_service.dll!network::BrokeredTcpClientSocket::Connect 行 91	C++
 	net.dll!net::TransportConnectSubJob::DoEndpointLockComplete 行 240	C++
 	net.dll!net::TransportConnectSubJob::DoLoop 行 177	C++
 	net.dll!net::TransportConnectSubJob::Start 行 128	C++
 	net.dll!net::TransportConnectJob::DoTransportConnect 行 397	C++
 	net.dll!net::TransportConnectJob::DoLoop 行 231	C++
 	net.dll!net::TransportConnectJob::ConnectInternal 行 495	C++
 	net.dll!net::ConnectJob::Connect 行 130	C++
 	net.dll!net::HttpProxyConnectJob::DoTransportConnect 行 521	C++
 	net.dll!net::HttpProxyConnectJob::DoLoop 行 429	C++
 	net.dll!net::HttpProxyConnectJob::ConnectInternal 行 389	C++
 	net.dll!net::ConnectJob::Connect 行 130	C++
 	net.dll!net::SSLConnectJob::DoTunnelConnect 行 311	C++
 	net.dll!net::SSLConnectJob::DoLoop 行 216	C++
 	net.dll!net::SSLConnectJob::ConnectInternal 行 544	C++
 	net.dll!net::ConnectJob::Connect 行 130	C++
 	net.dll!net::TransportClientSocketPool::RequestSocketInternal 行 463	C++
 	net.dll!net::TransportClientSocketPool::RequestSocket 行 273	C++
 	net.dll!net::ClientSocketHandle::Init 行 51	C++
 	net.dll!net::`anonymous namespace'::InitSocketPoolHelper 行 131	C++
 	net.dll!net::InitSocketHandleForHttpRequest 行 226	C++
 	net.dll!net::HttpStreamFactory::Job::DoInitConnectionImpl 行 823	C++
 	net.dll!net::HttpStreamFactory::Job::DoInitConnection 行 702	C++
 	net.dll!net::HttpStreamFactory::Job::DoLoop 行 630	C++
 	net.dll!net::HttpStreamFactory::Job::RunLoop 行 531	C++
 	net.dll!net::HttpStreamFactory::Job::StartInternal 行 656	C++
 	net.dll!net::HttpStreamFactory::Job::Start 行 246	C++
 	net.dll!net::HttpStreamFactory::JobController::DoCreateJobs 行 1028	C++
 	net.dll!net::HttpStreamFactory::JobController::DoLoop 行 807	C++
 	net.dll!net::HttpStreamFactory::JobController::RunLoop 行 765	C++
 	net.dll!net::HttpStreamFactory::JobController::Start 行 235	C++
 	net.dll!net::HttpStreamFactory::RequestStreamInternal 行 235	C++
 	net.dll!net::HttpStreamFactory::RequestStream 行 163	C++
 	net.dll!net::HttpNetworkTransaction::DoCreateStream 行 1132	C++
 	net.dll!net::HttpNetworkTransaction::DoLoop 行 1005	C++
 	net.dll!net::HttpNetworkTransaction::Start 行 422	C++
 	services_network_network_service.dll!network::ThrottlingNetworkTransaction::Start 行 137	C++
 	net.dll!net::SharedDictionaryNetworkTransaction::Start 行 114	C++
 	net.dll!net::HttpCache::Transaction::DoSendRequest 行 1921	C++
 	net.dll!net::HttpCache::Transaction::DoLoop 行 910	C++
 	net.dll!net::HttpCache::Transaction::Start 行 213	C++
 	net.dll!net::URLRequestHttpJob::StartTransactionInternal 行 782	C++
 	net.dll!net::URLRequestHttpJob::MaybeStartTransactionInternal 行 714	C++
 	net.dll!net::URLRequestHttpJob::StartTransaction 行 692	C++
 	net.dll!net::URLRequestHttpJob::SetCookieHeaderAndStart 行 998	C++
 	net.dll!base::internal::DecayedFunctorTraits<...>::Invoke<...> 行 740	C++
 	net.dll!base::internal::InvokeHelper<...>::MakeItSo<...> 行 956	C++
 	net.dll!base::internal::Invoker<...>::RunImpl<...> 行 1069	C++
 	net.dll!base::internal::Invoker<...>::RunOnce 行 982	C++
 	net.dll!base::OnceCallback<...>::Run 行 156	C++
 	net.dll!`anonymous namespace'::MaybeRunCookieCallback<...> 行 160	C++
 	net.dll!net::CookieMonster::GetCookieListWithOptions 行 823	C++
 	net.dll!base::internal::DecayedFunctorTraits<...>::Invoke<...> 行 740	C++
 	net.dll!base::internal::InvokeHelper<...>::MakeItSo<...> 行 932	C++
 	net.dll!base::internal::Invoker<...>::RunImpl<...> 行 1069	C++
 	net.dll!base::internal::Invoker<...>::RunOnce 行 982	C++
 	net.dll!base::OnceCallback<...>::Run 行 156	C++
 	net.dll!net::CookieMonster::DoCookieCallbackForHostOrDomain 行 2986	C++
 	net.dll!net::CookieMonster::DoCookieCallbackForURL 行 2946	C++
 	net.dll!net::CookieMonster::GetCookieListWithOptionsAsync 行 582	C++
 	net.dll!net::URLRequestHttpJob::AddCookieHeaderAndStart 行 847	C++
 	net.dll!net::URLRequestHttpJob::OnGotFirstPartySetMetadata 行 556	C++
 	net.dll!net::URLRequestHttpJob::Start 行 494	C++
 	net.dll!net::URLRequest::StartJob 行 762	C++
 	net.dll!net::URLRequest::BeforeRequestComplete 行 672	C++
 	net.dll!net::URLRequest::Start 行 622	C++
 	services_network_network_service.dll!network::URLLoader::ScheduleStart 行 762	C++
>	services_network_network_service.dll!network::URLLoader::ProcessOutboundSharedStorageInterceptor 行 743	C++
 	services_network_network_service.dll!network::URLLoader::ProcessOutboundTrustTokenInterceptor 行 672	C++
 	services_network_network_service.dll!network::URLLoader::URLLoader 行 562	C++
 	services_network_network_service.dll!std::__Cr::make_unique<...> 行 756	C++
 	services_network_network_service.dll!network::URLLoaderFactory::CreateLoaderAndStartWithSyncClient 行 393	C++
 	services_network_network_service.dll!network::cors::CorsURLLoader::StartNetworkRequest 行 1025	C++
 	services_network_network_service.dll!network::cors::CorsURLLoader::StartRequest 行 913	C++
 	services_network_network_service.dll!network::cors::CorsURLLoader::Start 行 405	C++
 	services_network_network_service.dll!network::cors::CorsURLLoaderFactory::CreateLoaderAndStart 行 493	C++
 	services_network_network_service.dll!network::PrefetchMatchingURLLoaderFactory::CreateLoaderAndStart 行 107	C++
 	services_network_network_service.dll!network::mojom::URLLoaderFactoryStubDispatch::Accept 行 359	C++
 	services_network_network_service.dll!network::mojom::URLLoaderFactoryStub<...>::Accept 行 139	C++
 	mojo_public_cpp_bindings.dll!mojo::InterfaceEndpointClient::HandleValidatedMessage 行 1085	C++
 	mojo_public_cpp_bindings.dll!mojo::InterfaceEndpointClient::HandleIncomingMessageThunk::Accept 行 383	C++
 	mojo_public_cpp_bindings.dll!mojo::MessageDispatcher::Accept 行 51	C++
 	mojo_public_cpp_bindings.dll!mojo::InterfaceEndpointClient::HandleIncomingMessage 行 747	C++
 	mojo_public_cpp_bindings.dll!mojo::internal::MultiplexRouter::ProcessIncomingMessage 行 1203	C++
 	mojo_public_cpp_bindings.dll!mojo::internal::MultiplexRouter::Accept 行 788	C++
 	mojo_public_cpp_bindings.dll!mojo::MessageDispatcher::Accept 行 44	C++
 	mojo_public_cpp_bindings.dll!mojo::Connector::DispatchMessageW 行 567	C++
 	mojo_public_cpp_bindings.dll!mojo::Connector::ReadAllAvailableMessages 行 629	C++
 	mojo_public_cpp_bindings.dll!mojo::Connector::OnHandleReadyInternal 行 456	C++
 	mojo_public_cpp_bindings.dll!mojo::Connector::OnWatcherHandleReady 行 420	C++
 	mojo_public_cpp_bindings.dll!base::internal::DecayedFunctorTraits<...>::Invoke<...> 行 740	C++
 	mojo_public_cpp_bindings.dll!base::internal::InvokeHelper<...>::MakeItSo<...> 行 932	C++
 	mojo_public_cpp_bindings.dll!base::internal::Invoker<...>::RunImpl<...> 行 1069	C++
 	mojo_public_cpp_bindings.dll!base::internal::Invoker<...>::Run 行 989	C++
 	mojo_public_cpp_bindings.dll!base::RepeatingCallback<...>::Run 行 344	C++
 	mojo_public_cpp_bindings.dll!mojo::SimpleWatcher::DiscardReadyState 行 192	C++
 	mojo_public_cpp_bindings.dll!base::internal::DecayedFunctorTraits<...>::Invoke<...> 行 673	C++
 	mojo_public_cpp_bindings.dll!base::internal::InvokeHelper<...>::MakeItSo<...> 行 932	C++
 	mojo_public_cpp_bindings.dll!base::internal::Invoker<...>::RunImpl<...> 行 1069	C++
 	mojo_public_cpp_bindings.dll!base::internal::Invoker<...>::Run 行 989	C++
 	mojo_public_system_cpp.dll!base::RepeatingCallback<...>::Run 行 344	C++
 	mojo_public_system_cpp.dll!mojo::SimpleWatcher::OnHandleReady 行 287	C++
 	mojo_public_system_cpp.dll!mojo::SimpleWatcher::Context::Notify 行 98	C++
 	mojo_public_system_cpp.dll!mojo::SimpleWatcher::Context::CallNotify 行 65	C++
 	mojo_core_embedder_internal.dll!mojo::core::ipcz_driver::MojoTrap::DispatchEvent 行 606	C++
 	mojo_core_embedder_internal.dll!mojo::core::ipcz_driver::MojoTrap::DispatchOrQueueEvent 行 582	C++
 	mojo_core_embedder_internal.dll!mojo::core::ipcz_driver::MojoTrap::HandleEvent 行 460	C++
 	mojo_core_embedder_internal.dll!mojo::core::ipcz_driver::MojoTrap::TrapEventHandler 行 393	C++
 	mojo_core_embedder_internal.dll!ipcz::TrapEventDispatcher::DispatchAll 行 23	C++
 	mojo_core_embedder_internal.dll!ipcz::TrapEventDispatcher::~TrapEventDispatcher 行 12	C++
 	mojo_core_embedder_internal.dll!ipcz::Router::AcceptInboundParcel 行 272	C++
 	mojo_core_embedder_internal.dll!ipcz::NodeLink::AcceptCompleteParcel 行 1082	C++
 	mojo_core_embedder_internal.dll!ipcz::NodeLink::OnAcceptParcel 行 666	C++
 	mojo_core_embedder_internal.dll!ipcz::msg::NodeMessageListener::DispatchMessage 行 914	C++
 	mojo_core_embedder_internal.dll!ipcz::msg::NodeMessageListener::OnMessage 行 611	C++
 	mojo_core_embedder_internal.dll!ipcz::msg::NodeMessageListener::OnTransportMessage 行 746	C++
 	mojo_core_embedder_internal.dll!ipcz::DriverTransport::Notify 行 129	C++
 	mojo_core_embedder_internal.dll!ipcz::`anonymous namespace'::NotifyTransport 行 47	C++
 	mojo_core_embedder_internal.dll!mojo::core::ipcz_driver::Transport::OnChannelMessage 行 744	C++
 	mojo_core_embedder_internal.dll!mojo::core::Channel::TryDispatchMessage 行 1213	C++
 	mojo_core_embedder_internal.dll!mojo::core::Channel::TryDispatchMessage 行 1122	C++
 	mojo_core_embedder_internal.dll!mojo::core::Channel::OnReadComplete 行 1094	C++
 	mojo_core_embedder_internal.dll!mojo::core::`anonymous namespace'::ChannelWin::OnReadDone 行 267	C++
 	mojo_core_embedder_internal.dll!mojo::core::`anonymous namespace'::ChannelWin::OnIOCompleted 行 254	C++
 	base.dll!base::MessagePumpForIO::WaitForIOCompletion 行 906	C++
 	base.dll!base::MessagePumpForIO::DoRunLoop 行 843	C++
 	base.dll!base::MessagePumpWin::Run 行 88	C++
 	base.dll!base::sequence_manager::internal::ThreadControllerWithMessagePumpImpl::Run 行 650	C++
 	base.dll!base::RunLoop::Run 行 135	C++
 	base.dll!base::Thread::Run 行 361	C++
 	content.dll!content::`anonymous namespace'::ChildIOThread::Run 行 70	C++
 	base.dll!base::Thread::ThreadMain 行 436	C++
 	base.dll!base::`anonymous namespace'::ThreadFunc 行 114	C++
 	kernel32.dll!BaseThreadInitThunk()	未知
 	ntdll.dll!00007ff9db1cc48c()	未知
```
