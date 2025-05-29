#include "RKNetController.hpp"

#include <dwc/common/dwc_init.h>
#include <dwc/common/dwc_error.h>

#include <cstddef>

// Temporary, no idea where this should live
extern const char* s_mariokartwii = "mariokartwii";

namespace RKNet {

void RKNetController::createStaticInstance(EGG::Heap *heap) {
  if (!spInstance) {
    spInstance = new (heap, 0x4) RKNetController(heap);

    // doesn't feel like the right way to get the gamecode.
    DWC_Init(DWC_SVR_RELEASE, s_mariokartwii, 0x524d434a, alloc, free);
  }
}

void RKNetController::destroyStaticInstance() {
  if (spInstance) {
    DWC_Shutdown();
    if (spInstance) {
      delete spInstance;
    }
  }
}

void resetStateInitiateOnlineSession(u8 localPlayerCount) {

}

void RKNetController::scheduleShutdown() {
  m_shutdownScheduled = true;
}

void RKNetController::startWWRaceSearch(u8 localPlayerCount) {
  s32 connectionState = 0;
  WifiDisconnectInfo error;

  // There must be a gap of understanding here, this matches until the last cmplwi
  // but the call doesn't make any sense (parameters are flipped)
  DWC_GetLastErrorEx(reinterpret_cast<int*>(&error.errorCategory), reinterpret_cast<int*>(&error.errorCode));

  
  if (error.errorCategory / 10000 == 4 || error.errorCategory / 1000 == 0x62) {
    connectionState = 8;
  }
  else {
    connectionState = m_connectionState;
  }

  if (connectionState == 5) {
    initSubs();
    m_joinType = 1;

    m_subs[0].m_localPlayerCount = localPlayerCount;
    m_subs[1].m_localPlayerCount = localPlayerCount;
    // Calls a function from another class, will add later.
  }
}

void RKNetController::startRegionalRaceSearch(u8 localPlayerCount) {

}

void RKNetController::startWWBattleSearch(u8 localPlayerCount) {

}

void RKNetController::startRegionalBattleSearch(u8 localPlayerCount) {

}

void RKNetController::joinFriendRoom(u32 friendIdToJoin, u8 localPlayerCount) {

}

void RKNetController::createFriendRoom(u8 localPlayerCount) {

}

void RKNetController::resetRH1andROOM() {

}

void RKNetController::FUN806568F0() {
  if (m_00ec == 1) {
    return;
  }
  m_00ec = 2;
}

void RKNetController::FUN80656908() {
  if (m_00ec == 1) {
    return;
  }
  m_00ec = 3;
}

/* RKNetController::RKNetController(EGG::Heap *heap) {
  OSInitMutex(&m_mutex);
  m_connectionState = 0;
  m_disconnectedAids = 0;
  m_disconnectedPlayerIds = 0;

  m_heap = EGG::ExpHeap::create(0x9b00, heap, 0);

}*/

}
