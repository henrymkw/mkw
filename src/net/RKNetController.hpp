#pragma once

#include <egg/core/eggExpHeap.hpp>
#include <dwc/common/dwc_memfunc.h>
#include <rvl/os/osMutex.h>

#include <rk_types.h>
#include <rk_common.h>

#include <decomp.h>

// Credits:
// https://github.com/MelgMKW/Pulsar/blob/main/GameSource/MarioKartWii/RKNet/RKNetController.hpp was used as reference for naming
// https://github.com/CLF78/OpenPayload/blob/master/payload/game/net/RKNetController.hpp

namespace RKNet {

class RKNetController {
public:
  static void createStaticInstance(EGG::Heap *heap);
  static void destroyStaticInstance();

  // Called when starting anything online related
  void resetStateInitiateOnlineSession(u8 localPlayerCount);

  void scheduleShutdown();

  // All are called after VS/Battle button is pressed
  void startWWRaceSearch(u8 localPlayerCount);
  void startRegionalRaceSearch(u8 localPlayerCount);
  void startWWBattleSearch(u8 localPlayerCount);
  void startRegionalBattleSearch(u8 localPlayerCount);

  // TODO: Name and add 0x806564a4 and 0x806565d4

  void joinFriendRoom(u32 friendIdToJoin, u8 localPlayerCount);
  void createFriendRoom(u8 localPlayerCount);

  void resetRH1andROOM();

  void FUN806568F0();
  
  void FUN80656908();

  void initSubs();

  RKNetController(EGG::Heap* heap);
  ~RKNetController();

  // Called by DWC libs
  static void* alloc(DWCAllocType name, u32 size, int align);
  static void free(DWCAllocType name, void* ptr, u32 size);

  // Called by SO libs
  static void* alloc(u32 name, u32 size);
  static void free(u32 name, void* ptr);

  enum WifiDisconnectCategory {
    ERROR_CODE = 0x1,
    MII_INVALID = 0x2,
    GENERAL_DISCONNECT = 0x4,
    UNRECOVERABLE_DISCONNECT = 0x5,
  };

  struct WifiDisconnectInfo {
    WifiDisconnectCategory errorCategory;
    s32 errorCode;
  };

private:
   class ControllerSub {
    public:
    u8 _00[0x08 - 0x00];
    u32 m_connectionCount;
    u32 m_playerCount;
    u32 m_availableAids;
    u32 m_aidBitmap;
    u32 m_groupId;
    u8 _1c[0x20 - 0x1c];
    u8 m_localPlayerCount;
    u8 m_myAid;
    u8 m_myServerAid;
    u8 _23[0x58 - 0x23];
  };
  static_assert(sizeof(ControllerSub) == 0x58);

  static RKNetController* spInstance;

  // Source: Ghidra
  u8 _0000[0x0008 - 0x0000];
  OSMutex m_mutex;
  EGG::ExpHeap* m_heap;
  u8 _0024[0x0028 - 0x0024];
  u32 m_connectionState; // TODO: Make enum
  WifiDisconnectInfo m_errorInfo;
  u8 _0034[0x0038 - 0x0034];
  ControllerSub m_subs[2];
  u32 m_joinType;
  s32 m_00ec; 
  u8 *m_splitSendRACEPackets[2][12];
  u8 *m_splitRecvRACEPackets[2][12];
  u8 *m_fullSendPackets[12];
  u64 m_lastRACESendTimes[12];
  u64 m_lastRACERecvTimes[12];
  u64 m_RACESendTimesTaken[12];
  u64 m_RACERecvTimesTaken[12];
  u8 m_lastRACESendAid;
  u8 m_fullRecvPackets[12][736];
  u8 _25e1[0x2754 - 0x25e1];
  u8 m_2754;
  u8 m_shutdownScheduled;
  u8 m_2756;
  u8 m_2757;
  u8 _2758[0x2764 - 0x2758];
  s32 m_vr;
  s32 m_br;
  s32 m_lastSendBufferUsed[12];
  s32 m_lastRecvBufferUsed[12][8];
  s32 m_currentSub;
  u8 m_aidsBelongingToPlayerIds[12];
  u32 m_disconnectedAids;
  u32 m_disconnectedPlayerIds;
  u8 _2934[0x29c8 - 0x2934];
};
static_assert(sizeof(RKNetController) == 0x29c8);

}
