#include "FriendRosterManager.hpp"

namespace Net {

bool FriendRosterManager::isBuddyFriend(u32 friendId) {
  return DWC_IsBuddyFriendData(&m_dwcAccFriendData2[friendId]);
}

} // namespace Net
