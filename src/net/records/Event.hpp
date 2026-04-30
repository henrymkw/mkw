#pragma once

#include <rk_types.h>

namespace Net {

struct EventRecord {
  u8 _00[0xf8 - 0x00];
};
static_assert(sizeof(EventRecord) == 0xf8);

class EventHandler {
public:
  static void createStaticInstance();

  void calc();

  bool unk8065b8d4();

  static EventHandler* Instance() { return spInstance; }

  u8 _0000[0x2b84 - 0x0000];
  u32 _unk2b88;

  static EventHandler* spInstance;
};
static_assert(sizeof(EventHandler) == 0x2b88);

} // namespace Net
