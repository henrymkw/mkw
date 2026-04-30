#pragma once

#include <rk_types.h>

namespace Net {

struct RacePacketHeader {
  u32 magic;
  u32 crc32;
  u8 packetSizes[8];
};
static_assert(sizeof(RacePacketHeader) == 0x10);

} // namespace Net
