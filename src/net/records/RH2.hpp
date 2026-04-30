#pragma once

#include <rk_types.h>

namespace Net {

struct RH2Record {
  u8 _00[0x28 - 0x00];
};
static_assert(sizeof(RH2Record) == 0x28);

} // namespace Net
