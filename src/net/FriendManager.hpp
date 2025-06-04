#pragma once

#include <rk_types.h>

namespace Net {

class FriendManager {
public:
  static FriendManager *getInstance() {
    return spInstance;
  }

private:
  u8 [0x3e8 - 0x000];

  static FriendManager *spInstance;
};
static_assert(sizeof(FriendManager) == 0x3e8);

}
