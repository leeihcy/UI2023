#include "uuid.h"
#include <assert.h>

using namespace ui;

void uuid_unittest() 
{
  const ui::Uuid& uuid = ui::Uuid("5C36801E-5929-4512-A998-F9719DCC6903");
  const ui::Uuid& uuid2 = {0x5C36801E, 0x5929, 0x4512, {0xA9, 0x98, 0xF9, 0x71, 0x9D, 0xCC, 0x69, 0x03}};
  assert(uuid.data1 == 0x5C36801E);
  assert(uuid.data2 == 0x5929);
  assert(uuid.data3 == 0x4512);
  assert(uuid.data4[0] == 0xA9);
  assert(uuid.data4[1] == 0x98);
  assert(uuid.data4[2] == 0xF9);
  assert(uuid.data4[3] == 0x71);
  assert(uuid.data4[4] == 0x9D);
  assert(uuid.data4[5] == 0xCC);
  assert(uuid.data4[6] == 0x69);
  assert(uuid.data4[7] == 0x03);

  assert(uuid2.data4[0] == 0xA9);
  assert(uuid == uuid2);

  ui::Uuid uuid3 = uuid2;
  assert(uuid3 == uuid2);

}

