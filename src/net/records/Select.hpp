#pragma once

#include <rk_types.h>

namespace Net {

struct SelectRecord {
  u8 _00[0x28 - 0x00];
  u8 m_playerIdToAidMapping[12];
  u8 _34[0x38 - 0x34];
};
static_assert(sizeof(SelectRecord) == 0x38);

class SelectHandler {
public:
  const u8* getPlayerIdToAidMapping() const;

  static SelectHandler* Instance() { return spInstance; }

private:
  u8 _000[0x008 - 0x000];
  SelectRecord m_sendPacket;
  SelectRecord m_recvPackets[12];
  u8 _2e0[0x3f8 - 0x2e0];

  static SelectHandler* spInstance;
};
static_assert(sizeof(SelectHandler) == 0x3f8);

} // namespace Net
