#pragma once

#include <rk_types.h>

namespace Net {

struct UserRecord {
  u8 _00[0xc0 - 0x00];

  inline void reset();
};
static_assert(sizeof(UserRecord) == 0xc0);

class UserHandler {
public:
  void prepareToSend();

  void update();

  void importRecords();

  static UserHandler* Instance() { return spInstance; }

private:
  bool m_prepared;
  u8 _001[0x008 - 0x001];
  UserRecord m_recordToSend;
  UserRecord m_recvRecords[12];
  u8 _9c8[0x9d8 - 0x9c8];
  void* m_rflPackets; // Not a void
  BOOL _9dc;
  u32 _9e0;
  u32 m_aidsThatSentUser;
  u32 _9e8;
  u8 _9ec[0x9f0 - 0x9ec];

  static UserHandler* spInstance;
};
static_assert(sizeof(UserHandler) == 0x9f0);

} // namespace Net
