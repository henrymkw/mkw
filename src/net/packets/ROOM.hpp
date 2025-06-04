#pragma once

#include <rk_types.h>

namespace Net {

enum RoomRole {
  ROOM_ROLE_HOST = 0x0,
  ROOM_ROLE_GUEST = 0x1,
};

// Credits: https://wiki.tockdom.com/wiki/Network_Protocol/ROOM
#pragma options align=packed
  struct ROOMPacket {
  u8 messageType;
  u16 param_1;
  u8 param_2;
};
#pragma options align=reset
static_assert(sizeof(ROOMPacket) == 0x4);



class ROOMHandler {

public:
  static ROOMHandler *getInstance() {
    return spInstance;
  }
  
  void init(RoomRole role);

  void initAndZero();

private:
  u8 _00[0x04 - 0x00];
  u32 m_role;
  ROOMPacket m_packetsToSend[12];
  ROOMPacket m_packetsRecieved[12];
  u8 m_aidLastSentTo;
  u8 _68[0x70 - 0x69];
  u64 m_lastSentTime;
  u8 _78[0x80 - 0x78];
  static ROOMHandler *spInstance;
};
static_assert(sizeof(ROOMHandler) == 0x80);

}
