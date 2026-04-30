#pragma once

#include <rk_types.h>

#include <rvl/os/osMutex.h>
#include <dwc/core/dwc_account.h>
#include <dwc/core/dwc_context.h>

#include <gamespy/GP/gp.h>

namespace Net {

class FriendRosterManager {
public:
  bool isBuddyFriend(u32 friendId);

  inline void resetProfileIds() {
    for (u32 i = 0; i < 30; i++)
      m_friendsGSProfileIds[i] = 0;
  }

  void* vtable;
  OSMutex m_mutex;                          // 0x004
  DWCAccUserData m_dwcUserData[2];          // 0x01c, 0x05c
  DWCAccFriendData m_dwcAccFriendData[30];  // 0x9c
  DWCAccFriendData m_dwcAccFriendData2[30]; // 0x204
  GPProfile m_friendsGSProfileIds[30];      // 0x36c
  u64* m_pFriendCodes;                      // 0x3e4
  static FriendRosterManager* Instance() { return spInstance; }
  static FriendRosterManager* spInstance;
};
static_assert(sizeof(FriendRosterManager) == 0x3e8);

} // namespace Net
