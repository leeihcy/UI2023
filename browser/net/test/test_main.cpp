
namespace net {
void NetUnitTest();
}

int main() {
#if defined(_DEBUG)
  net::NetUnitTest();
#endif
  return 0;
}