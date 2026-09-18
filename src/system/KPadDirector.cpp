#include "KPadDirector.hpp"

namespace System {

void KPadDirector::resetPlayersActivityStatus() {
  for (u8 i = 0; i < 4; i++) {
    mPlayers[i].mInactivityFrames = 0;
    mPlayers[i].mDisconnectedFrames = 0;
  }
}

void KPadDirector::resetMasterController() {
  mMaster.setController(&mDummyController, nullptr);
}

void KPadDirector::resetPlayerController(u8 playerIdx) {
  mPlayers[playerIdx].setController(&mDummyController, nullptr);
}

void KPadDirector::resetPlayerControllers() {
  for (u8 i = 0; i < 4; i++) {
    mPlayers[i].setController(&mDummyController, nullptr);
  }
}

} // namespace System
