#pragma once

#include <rk_types.h>

#include "net/NetManager.hpp" // for RatingGroups

namespace Net {

class Nwc24Manager {
public:
  bool ratingRelated8067686c(RatingGroups* groups);
  static Nwc24Manager* Instance() { return spInstance; }

private:
  u8 _0000[0xa008 - 0x0000];

  static Nwc24Manager* spInstance;
};
static_assert(sizeof(Nwc24Manager) == 0xa008);

} // namespace Net
