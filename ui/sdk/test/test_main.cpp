#include <iostream>
using namespace std;
#include "include/macro/uidefine.h"
#include "include/uiapi.h"

int main() {
#if defined(_DEBUG)
  ui::UIUnitTest();
#endif
  return 0;
}