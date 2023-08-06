#include "guid.h"
#include <assert.h>

using namespace ui;

class Test 
{
public:
    UI_DEFINE_CLASS_GUID("5C36801E-5929-4512-A998-F9719DCC6903");
};

void guid_unittest() 
{
  const ui::Guid& guid = __guid(Test);
  assert(guid.data1 == 0x5C36801E);
  assert(guid.data2 == 0x5929);
  assert(guid.data3 == 0x4512);
  assert(guid.data4[0] == 0xA9);
  assert(guid.data4[1] == 0x98);
  assert(guid.data4[2] == 0xF9);
  assert(guid.data4[3] == 0x71);
  assert(guid.data4[4] == 0x9D);
  assert(guid.data4[5] == 0xCC);
  assert(guid.data4[6] == 0x69);
  assert(guid.data4[7] == 0x03);
  assert(guid == __guid(Test));

  assert(__guid(Test) == ui::Guid::Build("5C36801E-5929-4512-A998-F9719DCC6903"));
}

