#pragma once

#include <rk_types.h>

namespace Net {

struct ItemRecord {
  u8 _0[0x8 - 0x0];
};
static_assert(sizeof(ItemRecord) == 0x8);

class ItemHandler {
public:
  static void createStaticInstance();

  void calc();

  static ItemHandler* Instance() { return spInstance; }

private:
  u8 _000[0x184 - 0x000];

  static ItemHandler* spInstance;
};
static_assert(sizeof(ItemHandler) == 0x184);

} // namespace Net
