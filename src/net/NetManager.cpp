#include "NetManager.hpp"

#include "egg/core/eggHeap.hpp"
#include "gamespy/GP/gp.h"
#include "net/FriendInfo.hpp"
#include "net/FriendRosterManager.hpp"
#include "net/GPCallbacks.hpp"
#include "net/MiscPacketHandler.hpp"
#include "net/records/Room.hpp"

#include "host_system/SystemManager.hpp"
#include "host_system/RKSystem.hpp"
#include "system/RaceConfig.hpp"

#include <dwc/common/dwc_error.h>
#include <dwc/common/dwc_init.h>
#include <dwc/core/dwc_friend.h>
#include <dwc/core/dwc_main.h>
#include <dwc/core/dwc_match.h>
#include <dwc/core/dwc_transport.h>

#include <rvl/net.h>

namespace Net {

void NetManager::scheduleShutdown() { m_shutdownScheduled = true; }

void NetManager::startWWVSSearch(u8 localPlayerCount) {
  ConnectionState connState = getConnectionState();

  // were online, start search/mm
  if (connState == CONNECTION_STATE_IDLE) {
    initMMInfos();
    m_roomType = ROOM_TYPE_VS_WW;
    m_matchMakingInfos[0].localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].localPlayerCount = localPlayerCount;
    // allows the logic in RACEHEADER1Handler_calc() to run
    RH1Handler::Instance()->setPrepared();
  }
}

void NetManager::startRegionalVSSearch(u8 localPlayerCount) {
  ConnectionState connState = getConnectionState();

  if (connState == CONNECTION_STATE_IDLE) {
    initMMInfos();
    m_roomType = ROOM_TYPE_VS_REGIONAL;
    m_matchMakingInfos[0].localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].localPlayerCount = localPlayerCount;
    RH1Handler::Instance()->setPrepared();
  }
}

void NetManager::startWWBattleSearch(u8 localPlayerCount) {
  ConnectionState connState = getConnectionState();

  if (connState == CONNECTION_STATE_IDLE) {
    initMMInfos();
    m_matchMakingInfos[0].localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].localPlayerCount = localPlayerCount;
    m_roomType = ROOM_TYPE_BT_WW;
    RH1Handler::Instance()->setPrepared();
  }
}

void NetManager::startRegionalBattleSearch(u8 localPlayerCount) {
  ConnectionState connState = getConnectionState();

  if (connState == CONNECTION_STATE_IDLE) {
    initMMInfos();
    m_matchMakingInfos[0].localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].localPlayerCount = localPlayerCount;
    m_roomType = ROOM_TYPE_BT_REGIONAL;
    RH1Handler::Instance()->setPrepared();
  }
}

void NetManager::joinFriendPublicVS(u32 friendIdx, u8 localPlayerCount) {
  ConnectionState connState = getConnectionState();

  if (connState == CONNECTION_STATE_IDLE) {
    FriendJoinableStatus status = getFriendJoinableStatus(friendIdx);
    switch (status) {
    case STATUS_WW_VS:
      m_roomType = ROOM_TYPE_JOINING_FRIEND_VS_WW;
      break;
    case STATUS_JOINABLE_REGIONAL_VS:
      m_roomType = ROOM_TYPE_JOINING_FRIEND_VS_REGIONAL;
      break;
    default:
      OSLockMutex(&m_mutex);

      if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
        m_disconnectInfo.type = DISCONNECT_TYPE_CANT_JOIN_FRIEND;
        m_disconnectInfo.code = 0;
      }

      OSUnlockMutex(&m_mutex);
      return;
    }
    initMMInfos();
    m_matchMakingInfos[0].hostFriendId = friendIdx;
    m_matchMakingInfos[1].hostFriendId = friendIdx;
    m_matchMakingInfos[0].localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].localPlayerCount = localPlayerCount;
    RH1Handler::Instance()->setPrepared();
  }
}

void NetManager::joinFriendPublicBT(u32 friendIdx, u8 localPlayerCount) {
  ConnectionState connState = getConnectionState();

  if (connState == CONNECTION_STATE_IDLE) {
    FriendJoinableStatus status = getFriendJoinableStatus(friendIdx);
    switch (status) {
    case STATUS_WW_BT:
      m_roomType = ROOM_TYPE_JOINING_FRIEND_BT_WW;
      break;
    case STATUS_JOINABLE_REGIONAL_BT:
      m_roomType = ROOM_TYPE_JOINING_FRIEND_BT_REGIONAL;
      break;
    default:
      OSLockMutex(&m_mutex);

      if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
        m_disconnectInfo.type = DISCONNECT_TYPE_CANT_JOIN_FRIEND;
        m_disconnectInfo.code = 0;
      }

      OSUnlockMutex(&m_mutex);
      return;
    }
    initMMInfos();
    m_matchMakingInfos[0].hostFriendId = friendIdx;
    m_matchMakingInfos[1].hostFriendId = friendIdx;
    m_matchMakingInfos[0].localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].localPlayerCount = localPlayerCount;
    RH1Handler::Instance()->setPrepared();
  }
}

void NetManager::joinFriendRoom(u32 friendIdx, u8 localPlayerCount) {
  ConnectionState connState = getConnectionState();

  if (connState == CONNECTION_STATE_IDLE) {
    initMMInfos();
    m_matchMakingInfos[0].hostFriendId = friendIdx;
    m_matchMakingInfos[1].hostFriendId = friendIdx;
    m_roomType = ROOM_TYPE_NONHOST_PRIVATE;
    m_matchMakingInfos[0].localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].localPlayerCount = localPlayerCount;
    RoomHandler::Instance()->init(ROLE_GUEST);
  }
}

void NetManager::createFriendRoom(u8 localPlayerCount) {
  ConnectionState connState = getConnectionState();

  if (connState == CONNECTION_STATE_IDLE) {
    initMMInfos();
    m_roomType = ROOM_TYPE_HOST_PRIVATE;
    m_matchMakingInfos[0].localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].localPlayerCount = localPlayerCount;
    RoomHandler::Instance()->init(ROLE_HOST);
  }
}

void NetManager::resetRH1andROOM() {
  if (RH1Handler::Instance() != nullptr) {
    RH1Handler::Instance()->reset();
  }
  if (RoomHandler::Instance() != nullptr) {
    RoomHandler::Instance()->reset();
  }
  m_voteMMSuspension = VOTE_MM_DISCONNECTED;
}

void NetManager::setVoteMatchMakingSuspend() {
  if (m_voteMMSuspension == VOTE_MM_DISCONNECTED) {
    return;
  }
  m_voteMMSuspension = VOTE_MM_SUSPEND;
}

void NetManager::setVoteMatchMakingUnsuspend() {
  if (m_voteMMSuspension == VOTE_MM_DISCONNECTED) {
    return;
  }
  m_voteMMSuspension = VOTE_MM_UNSUSPEND;
}

void NetManager::setDisconnectInfo(DisconnectType dcType, s32 errorCode) {
  OSLockMutex(&m_mutex);

  if (dcType == DISCONNECT_TYPE_DISK_EJECTED) {
    m_hasEjectedDisk = true;
  } else if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
    m_disconnectInfo.type = dcType;
    m_disconnectInfo.code = errorCode;
    switch (dcType) {
    case DISCONNECT_TYPE_ERROR_CODE:
    case DISCONNECT_TYPE_BAD_MII_NAME:
    case DISCONNECT_TYPE_UNRECOVERABLE_ERROR:
      m_connectionState = CONNECTION_STATE_ERROR;
      break;
    }
  }
  OSUnlockMutex(&m_mutex);
}

DisconnectInfo NetManager::getDisconnectInfo() {
  DisconnectInfo dcInfo;

  OSLockMutex(&m_mutex);

  if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR &&
      m_hasEjectedDisk) {
    dcInfo.type = DISCONNECT_TYPE_DISK_EJECTED;
    dcInfo.code = 0;

  } else {
    dcInfo.type = m_disconnectInfo.type;
    dcInfo.code = m_disconnectInfo.code;
  }
  OSUnlockMutex(&m_mutex);
  return dcInfo;
}

void NetManager::resetDisconnectInfo() {
  OSLockMutex(&m_mutex);
  if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
    m_disconnectInfo.type = DISCONNECT_TYPE_NONE;
    m_disconnectInfo.code = 0;
  }
  OSUnlockMutex(&m_mutex);
}

void NetManager::resetErrors() {
  m_hasEjectedDisk = false;
  resetDisconnectInfo();
}

s32 NetManager::matchMakingElapsedSeconds() {
  s32 time = m_matchMakingInfos[m_currMMInfo].matchMakingStartTime;

  // has to do u64 comparison
  if (m_matchMakingInfos[m_currMMInfo].matchMakingStartTime == 0) {
    return 0;
  }

  OSTime currTime = OSGetTime();
  return OSTicksToSeconds((s32)currTime - time);
}

void NetManager::updateDWCServersAsnycPassProfanity() {
  updateDWCServersAsync();
  m_connectionState = CONNECTION_STATE_SOMETHING_GP_FRIENDS;
}

void NetManager::updateDWCServersAsync() {
  DWC_UpdateServersAsync(0, updateFriendStatusCallback, this, BLR_80658918,
                         this, 0, 0);
}

void NetManager::updateFriendsProfiles() {
  FriendRosterManager* friendRosterManager = FriendRosterManager::Instance();
  int numTargets = 0;

  // Reset friend's profileIds
  friendRosterManager->resetProfileIds();

  DWCstAccFriendData* friendData = friendRosterManager->m_dwcAccFriendData;

  // This loop repopulates the friendProfileId array
  for (u32 i = 0; i < MAX_FRIEND_COUNT; i++) {
    if (DWC_GetGsProfileId(friendRosterManager->m_dwcUserData, friendData) >
        0) {
      friendRosterManager->m_friendsGSProfileIds[numTargets++] =
          DWC_GetGsProfileId(friendRosterManager->m_dwcUserData, friendData);
    }
    friendData++;
  }

  // This sets up getting friend profileIds.
  // The callback updates friend structs upon gettomg a responce
  GPResult result = gpGetReversBuddiesList(
      DWCi_GetMatchCntExt()->connection,
      friendRosterManager->m_friendsGSProfileIds, numTargets, GP_NON_BLOCKING,
      GPReversBuddiesListCallback, // the cb updates friend structs upon getting
                                   // a responce.
      this);

  // update connection state if there's no error
  if (result == GP_NO_ERROR) {
    m_connectionState =
        CONNECTION_STATE_SYNC_FRIENDS; // Think this name is fine?
  } else {
    SetGPError(result);
  }
}

void NetManager::updateFriendsHasAddedBack(GPProfile* pidsThatAddedBack,
                                           u32 numPids) {
  for (u32 friendIdx = 0; friendIdx < MAX_FRIEND_COUNT; friendIdx++) {
    u32 i = 0;
    for (i = 0; i < numPids; i++) {
      if (DWC_GetGsProfileId(FriendRosterManager::Instance()->m_dwcUserData,
                             &FriendRosterManager::Instance()
                                  ->m_dwcAccFriendData[friendIdx]) ==
          pidsThatAddedBack[i]) {
        m_friends[friendIdx].addedBack = true;
        break;
      }
    }
    if (i == numPids) {
      if (DWC_GetGsProfileId(FriendRosterManager::Instance()->m_dwcUserData,
                             &FriendRosterManager::Instance()
                                  ->m_dwcAccFriendData[friendIdx]) == 0) {
        m_friends[friendIdx].addedBack = true;
      } else {
        m_friends[friendIdx].addedBack = false;
      }
    }
  }
}

void NetManager::resetFriendData(u32 friendIdx) {
  m_friends[friendIdx].statusData.roomId = 0;
  m_friends[friendIdx].statusData.regionId = 0;
  m_friends[friendIdx].statusData.status = 0;
  m_friends[friendIdx].statusData.playerCount = 0;
  m_friends[friendIdx].statusData.currRace = 0;
  m_friends[friendIdx].dwcFriendStatus = 0;
  m_friends[friendIdx].addedBack = true;
}

bool NetManager::isConnectionStateIdleOrInMM() const {
  bool idleOrMM = false;

  switch (getConnectionState()) {
  case CONNECTION_STATE_IDLE:
  case CONNECTION_STATE_IN_MM:
    idleOrMM = true;
    break;
  }

  return idleOrMM;
}

bool NetManager::isTaskThreadIdle() { return !m_taskThread->isTaskExist(); }

bool NetManager::isConnectionStateIdle() const {
  return getConnectionState() == CONNECTION_STATE_IDLE;
}

bool NetManager::hasFoundMatch() const {
  bool inMatch = false;

  bool isMyAidInMatch = (1 << m_matchMakingInfos[m_currMMInfo].myAid) &
                        m_matchMakingInfos[m_currMMInfo].availableAids;
  // were in a match if my aid is in the room and we have connected to another
  // console
  if (isMyAidInMatch &&
      m_matchMakingInfos[m_currMMInfo].numConnectedConsoles > 1) {
    inMatch = true;
  }
  return inMatch;
}

void NetManager::setConnectionStateIdle() {
  m_connectionState = CONNECTION_STATE_IDLE;
}

void NetManager::sendRaceUpdateUserPackets() {
  formRacePacket();
  sendRacePacket();
  UserHandler::Instance()->update();
}

void NetManager::sendRacePacket() {
  u8 lastAid = m_aidLastSentTo;

  // get the time we last sent a packet (via the last aid we sent to)
  u32 timeSinceLastSend;
  if (lastAid != 0xff) {
    OSTime aidLastSentTime = m_timeOfLastSentRACE[lastAid];
    if (aidLastSentTime == 0) {
      timeSinceLastSend = -1;
    } else {
      OSTime currentTime = OSGetTime();
      OSTime delta = OSTicksToMilliseconds(currentTime - aidLastSentTime);

      timeSinceLastSend = delta;
    }
  } else {
    timeSinceLastSend = -1;
  }

  // dont send if we sent within 17 ms
  // (this function gets ran sometimes
  // every frame, and sometimes every other frame (dunno when or why). this
  // would rate limit in the case where its called each frame, but does nothing
  // every other frame)
  if (timeSinceLastSend < 17) {
    return;
  }

  lastAid = m_aidLastSentTo + 1;
  for (u8 aid = 0; aid < 12; aid++) {
    if (lastAid >= 12) {
      lastAid = 0;
    }

    MatchMakingInfo* mmInfo = getMMInfo();

    // skip if the aid is used or the aid is mine
    if ((1 << lastAid & mmInfo->availableAids) == 0) {
      lastAid++;
      continue;
    }
    if (lastAid == mmInfo->myAid) {
      lastAid++;
      continue;
    }

    bool sentSuccessfully = sendAidRacePacket(lastAid);

    if (sentSuccessfully) {
      return;
    }
    lastAid++;
  }
}

bool NetManager::sendAidRacePacket(u8 lastAid) {
  BOOL sentSuccessfully = FALSE;
  if (m_outgoingRACEPacket[lastAid]->m_packetSize != 0) {

    ((RacePacketHeader*)m_outgoingRACEPacket[lastAid]->m_packet)->crc32 =
        NETCalcCRC32(m_outgoingRACEPacket[lastAid]->m_packet,
                     m_outgoingRACEPacket[lastAid]->m_packetSize);

    sentSuccessfully =
        DWC_SendUnreliable(lastAid, m_outgoingRACEPacket[lastAid]->m_packet,
                           m_outgoingRACEPacket[lastAid]->m_packetSize);

    if (sentSuccessfully) {
      OSTime lastSentTime = m_timeOfLastSentRACE[lastAid];

      if (lastSentTime != 0) {
        m_timeBetweenSendingPackets[lastAid] = OSGetTime() - lastSentTime;
      }

      m_aidLastSentTo = lastAid;
      m_timeOfLastSentRACE[lastAid] = OSGetTime();
    }

    m_outgoingRACEPacket[lastAid]->reset();
  }
  return sentSuccessfully == TRUE;
}

u32 NetManager::getRACEPacketSize(u8 aid) {
  u32 size = 0;
  RacePacketHolder* holder = m_sendRacePackets[m_lastSendIdx[aid]][aid];
  // this could be inlined
  for (u32 i = 0; i < 8; i++) {
    size += holder->holder(i)->getPacketSize();
  }
  return size;
}

void NetManager::setConnectionState(ConnectionState connState) {
  m_connectionState = connState;
}

NetManager::ConnectionState NetManager::getConnectionState() const {
  s32 code;
  DWCErrorType type;
  DWC_GetLastErrorEx(&code, &type);

  ConnectionState connState;

  // 4xxxx and 98xxx are sake erorrs, otherwise set connectionState
  if ((code / 10000) == 4 || (code / 1000) == 98) {
    connState = CONNECTION_STATE_SAKE_ERROR;
  } else {
    connState = m_connectionState;
  }

  return connState;
}

void NetManager::handleError() {
  s32 code;
  DWCErrorType type;

  if (DWC_GetLastErrorEx(&code, &type)) {
    // this is functionally equal to code = -code; but this was need to match
    // dwc error codes are negative values, but mkw uses positive error codes.
    // we just need to negate code
    code = code - (code * 2);
    if ((code / 10000) == 4 || (code / 1000) == 98) {
      // return early for sake errors
      return;
    }

    DWC_ClearError();

    u32 errorCode; // this variable was needed for matching purposes

    switch (type) {
    case DWC_ERROR_TYPE_1:
    case DWC_ERROR_TYPE_2:

      errorCode = code; // this is used for matching purposes
      OSLockMutex(&m_mutex);
      if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
        m_disconnectInfo.type = DISCONNECT_TYPE_CANT_JOIN_FRIEND;
        // code getting set here is somewhat interesting since errors
        // when joining friends dont bring you to the error code screen in game
        m_disconnectInfo.code = errorCode;
      }
      OSUnlockMutex(&m_mutex);
      break;

    case DWC_ERROR_TYPE_3:
    case DWC_ERROR_TYPE_4:
    case DWC_ERROR_TYPE_5:
    case DWC_ERROR_TYPE_6:
      errorCode = code;
      OSLockMutex(&m_mutex);
      if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
        m_disconnectInfo.type = DISCONNECT_TYPE_ERROR_CODE;
        m_disconnectInfo.code = errorCode;
        m_connectionState = CONNECTION_STATE_ERROR;
      }
      OSUnlockMutex(&m_mutex);
      break;

    case DWC_ERROR_TYPE_7:
      OSLockMutex(&m_mutex);
      if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
        m_disconnectInfo.type = DISCONNECT_TYPE_UNRECOVERABLE_ERROR;
        m_disconnectInfo.code = 0;
        m_connectionState = CONNECTION_STATE_ERROR;
      }
      OSUnlockMutex(&m_mutex);
      break;
    }
  }
}

void* NetManager::alloc(u32 size, s32 alignment) {
  void* block = nullptr;
  if (size != 0) {
    OSLockMutex(&m_mutex);
    block = m_heap->alloc(size, alignment);
    OSUnlockMutex(&m_mutex);
  }
  return block;
}

void NetManager::free(void* block) {
  if (block != nullptr) {
    OSLockMutex(&m_mutex);
    m_heap->free(block);
    OSUnlockMutex(&m_mutex);
  }
}

void* NetManager::SOAlloc(u32 unk, u32 size) {
  void* block = nullptr;
  NetManager* netManager = spInstance;
  if (size != 0) {
    OSLockMutex(&netManager->m_mutex);
    block = netManager->m_heap->alloc(size, 0x20);
    OSUnlockMutex(&netManager->m_mutex);
  }
  return block;
}

void NetManager::SOFree(u32 unk, void* block) {
  NetManager* netManager = spInstance;
  if (block != nullptr) {
    OSLockMutex(&netManager->m_mutex);
    netManager->m_heap->free(block);
    OSUnlockMutex(&netManager->m_mutex);
  }
}

void* NetManager::DWCAlloc(u32 unk, u32 size, s32 alignment) {
  void* block = nullptr;
  NetManager* netManager = spInstance;
  if (size != 0) {
    OSLockMutex(&netManager->m_mutex);
    block = netManager->m_heap->alloc(size, alignment);
    OSUnlockMutex(&netManager->m_mutex);
  }
  return block;
}

void NetManager::DWCFree(u32 unk, void* block) {
  NetManager* netManager = spInstance;
  if (block != nullptr) {
    OSLockMutex(&netManager->m_mutex);
    netManager->m_heap->free(block);
    OSUnlockMutex(&netManager->m_mutex);
  }
}

void NetManager::connectionCleanupCallback() {
  u32 zeros;
  for (u8 i = 0; i < 12; i++) {
    zeros = 0;

    // GetConnectionUserData is the function name in ghidra, maybe its set
    // instead of get?
    DWC_GetConnectionUserData(i, &zeros);
  }
}

s32 NetManager::totalPlayersHelper() {
  s32 totalRemotePlayers = 0;
  for (u8 aid = 0; aid < 12; aid++) {
    u32 userData = 0; //
    if (DWC_GetConnectionUserData(aid, (u32*)&userData) != 0) {
      totalRemotePlayers += userData;
    }
  }
  return totalRemotePlayers;
}

bool NetManager::isTotalPlayersValid(DWCConnectionUserData* playerCountPtr,
                                     NetManager* self) {

  bool isValid =
      ((NetManager::totalPlayersHelper() + playerCountPtr->_0) <= 12);

  NetManager::connectionCleanupCallback();

  return isValid;
}

void NetManager::updateFriendStatusCallback(u32 r3, u32 r4, void* self) {
  if (r3 != 0) {
    return;
  }
  NetManager* netManager = reinterpret_cast<NetManager*>(self);
  if (r4 != 0) {
    netManager->m_friendRosterChanged = true;
  }
  netManager->updateFriendsProfiles();
}

void NetManager::setFriendRosterChangedCallback(u32 r3, u32 r4,
                                                NetManager* netManager) {
  if (r3 != 0) {
    return;
  }
  if (r4 == 0) {
    return;
  }
  netManager->m_friendRosterChanged = true;
}

void NetManager::BLR_80658918() {}

void NetManager::DWCSetBuddyFriendCallback(u32 r3, NetManager* netManager) {
  netManager->m_friendRosterChanged = true;
}

void NetManager::initMMInfos() {
  for (u32 i = 0; i < ARRAY_SIZE(m_matchMakingInfos); i++) {
    m_matchMakingInfos[i].matchMakingStartTime = 0;
    m_matchMakingInfos[i].numConnectedConsoles = 0;
    m_matchMakingInfos[i].playerCount = 0;
    m_matchMakingInfos[i].availableAids = 0;
    m_matchMakingInfos[i].directConnectedAidBitmap = 0;
    m_matchMakingInfos[i].roomId = 0;
    m_matchMakingInfos[i].hostFriendId = -1;
    m_matchMakingInfos[i].myAid = -1;
    m_matchMakingInfos[i].hostAid = -1;
    m_matchMakingInfos[i].isMatchMakingSuspended = false;
    for (u32 j = 0; j < MAX_PLAYER_COUNT; j++) {
      memset(&m_matchMakingInfos[i].localPlayerCounts[j], 0, 4);
    }
  }
}

void NetManager::resetFriends() {
  for (u32 i = 0; i < ARRAY_SIZE(m_friends); i++) {
    resetFriendData(i);
  }
}

void NetManager::buildHeader(u8 aid) {
  RacePacketHeader header;
  memset(&header, 0, sizeof(RacePacketHeader));

  for (u32 i = 0; i < 8; i++) {
    if (i == HEADERPacketId) {
      header.packetSizes[i] = sizeof(RacePacketHeader);
      continue;
    }
    header.packetSizes[i] =
        m_sendRacePackets[m_lastSendIdx[aid] ^ 1][aid]->holder(i)->m_packetSize;
  }
  m_sendRacePackets[m_lastSendIdx[aid] ^ 1][aid]->header()->copy(
      &header, sizeof(RacePacketHeader));
}

void NetManager::calcOutgoingCRC32(u8 aid) {
  u32 crc32 = NETCalcCRC32(m_outgoingRACEPacket[aid]->m_packet,
                           m_outgoingRACEPacket[aid]->getPacketSize());
  reinterpret_cast<RacePacketHeader*>(m_outgoingRACEPacket[aid]->m_packet)
      ->crc32 = crc32;
}

void NetManager::processRacePacket(u8 aid, RacePacketHeader* header, u32 size) {
  u32 origCrc32 = header->crc32;
  header->crc32 = 0;
  u32 calcCrc32 = NETCalcCRC32(header, size);

  // make sure the packet isn't corrupted
  if (origCrc32 == calcCrc32) {
    // update time based structs
    OSTime aidLastRecvTime = m_timeOfLastRecvRACE[aid];
    if (aidLastRecvTime != 0) {
      m_timeBetweenRecvPackets[aid] = OSGetTime() - aidLastRecvTime;
    }
    m_timeOfLastRecvRACE[aid] = OSGetTime();

    // data for other packet is immedately after the header, so add the
    // packet[i] size to this to get a specific offset
    u8* dataPacketPtr = reinterpret_cast<u8*>(header);
    for (u32 i = 0; i < ARRAY_SIZE(header->packetSizes); i++) {
      if (header->packetSizes[i] != 0) {
        // reset and copy the recieved packet into recv structs
        m_recvRacePackets[m_lastRecvIdx[aid][i] ^ 1][aid]->holder(i)->reset();
        m_recvRacePackets[m_lastRecvIdx[aid][i] ^ 1][aid]->holder(i)->copy(
            dataPacketPtr, header->packetSizes[i]);

        // increment the data pointer to the next packet offset
        dataPacketPtr += header->packetSizes[i];

        // flip the last recieved buffer idx
        m_lastRecvIdx[aid][i] ^= 1;
      }
    }
  }
}

void NetManager::resetPlayerIdToAidMap() {
  for (u32 i = 0; i < ARRAY_SIZE(m_playerIdToAidMapping); i++)
    m_playerIdToAidMapping[i] = -1;
}

void NetManager::updateAidMapping() {
  m_disconnectedPlayerIds = 0;
  m_disconnectedAids = 0;

  if (RH1Handler::Instance()) {
    const u8* RH1AidMapping = RH1Handler::Instance()->getPlayerIdToAidMapping();
    for (u32 i = 0; i < ARRAY_SIZE(m_playerIdToAidMapping); i++)
      m_playerIdToAidMapping[i] = RH1AidMapping[i];
  } else if (SelectHandler::Instance()) {
    const u8* selectAidMapping =
        SelectHandler::Instance()->getPlayerIdToAidMapping();
    for (u32 i = 0; i < ARRAY_SIZE(m_playerIdToAidMapping); i++)
      m_playerIdToAidMapping[i] = selectAidMapping[i];
  } else {
    resetPlayerIdToAidMap();
  }
}

s32 NetManager::getLocalId(u32 hudId) const {
  u8 myAid = m_matchMakingInfos[m_currMMInfo].myAid;
  s32 count = -1;
  for (s32 i = 0; i < ARRAY_SIZE(m_playerIdToAidMapping); i++) {
    if (m_playerIdToAidMapping[i] == myAid) {
      count++;
      if (count == hudId)
        return i;
    }
  }
  return -1;
}

bool NetManager::myAidInRoom() const {
  u32 fullMap = m_matchMakingInfos[m_currMMInfo].availableAids;
  u8 myAid = m_matchMakingInfos[m_currMMInfo].myAid;
  return (1 << myAid & fullMap);
}

s32 NetManager::getLocalPlayerId(u32 hudId) const {
  if (MiscPacketHandler::Instance()) {
    System::RaceConfig::Player* players =
        System::RaceConfig::spInstance->mRaceScenario.mPlayers;
    s32 count = 0;
    // this currently just exists to prevent regswaps
    u8 playerId = 0;
    // loop thru player ids, check if player[i] is local
    for (s32 i = 0; i < MAX_PLAYER_COUNT; i++) {

      if (players[playerId].mPlayerType ==
          System::RaceConfig::Player::TYPE_REAL_LOCAL) {

        // there can be at most two local players for online and the guest's
        // player id will be higher than player 1's.
        if (count == hudId) {
          return i;
        }
        count++;
      }
      playerId++;
    }

    return -1;
  }

  if (myAidInRoom()) {
    return getLocalId(hudId);
  }
  return -1;
}

// https://decomp.me/scratch/l8u6Q
FriendJoinableStatus NetManager::getFriendJoinableStatus(u32 friendIdx) const {
  if (!m_friends[friendIdx].addedBack ||
      m_friends[friendIdx].dwcFriendStatus == 0) {
    return STATUS_OFFLINE;
  }

  FriendStatus friendStatus =
      static_cast<FriendStatus>(m_friends[friendIdx].statusData.status);
  s8 regionId;

  // if the friend isn't online but not doing anything
  if (friendStatus != FRIEND_STATUS_IDLE) {
    switch (friendStatus) {

    // maybe the following two cases are an inlined function? its quite
    // repetitive
    case FRIEND_STATUS_PUBLIC_VS:
      if (m_disconnectPenalty != 0) {
        // if im penalized for dc-ing too much show my status as online for
        // others
        return STATUS_ONLINE;
      }

      regionId = m_friends[friendIdx].statusData.regionId;

      if (regionId != -1) {
        // i have the same region as my friend, so i can join their regional
        // room
        if (regionId ==
            static_cast<s32>(System::SystemManager::sInstance->mMatchingArea)) {
          return STATUS_JOINABLE_REGIONAL_VS;
        }
        // otherwise i cant
        return STATUS_UNJOINABLE_REGIONAL_VS;
      }
      return STATUS_WW_VS;

    case FRIEND_STATUS_PUBLIC_BT:
      if (m_disconnectPenalty != 0) {
        return STATUS_ONLINE;
      }

      regionId = m_friends[friendIdx].statusData.regionId;

      if (regionId != -1) {
        if (regionId ==
            static_cast<s32>(System::SystemManager::sInstance->mMatchingArea)) {
          return STATUS_JOINABLE_REGIONAL_BT;
        }
        return STATUS_UNJOINABLE_REGIONAL_BT;
      }
      return STATUS_WW_BT;

    // just return the friendStatus for all other cases
    case FRIEND_STATUS_FROOM_VS_HOST:
    case FRIEND_STATUS_FROOM_BATTLE_HOST:
    case FRIEND_STATUS_FROOM_VS_NON_HOST:
    case FRIEND_STATUS_FROOM_BATTLE_NON_HOST:
    case FRIEND_STATUS_ONLINE:
    case FRIEND_STATUS_OPEN_ROOM:
    case FRIEND_STATUS_PLAYING_WITH_FRIENDS:
      return static_cast<FriendJoinableStatus>(friendStatus);
      break;
    default:
      return static_cast<FriendJoinableStatus>(friendStatus);
    }
  }
  return STATUS_ONLINE;
}

void NetManager::updateStatusDatas() {
  if (m_myStatusData.status != FRIEND_STATUS_IDLE) {
    OSLockMutex(&m_mutex);
    m_myStatusData.roomId = DWC_GetGroupId();
    DWC_SetOwnStatusData(&m_myStatusData, 8);
    m_myStatusData.status = FRIEND_STATUS_IDLE;
    m_myStatusData.playerCount = 0;
    m_myStatusData.currRace = 0;
    m_myStatusData.regionId = -1;
    OSUnlockMutex(&m_mutex);
  }

  if (isConnectionStateIdleOrInMM() && m_shouldUpdateFriendStatus) {
    for (u32 i = 0; i < MAX_FRIEND_COUNT; i++) {
      if (FriendRosterManager::Instance()->isBuddyFriend(i)) {
        FriendInfo friendInfo;
        u32 r5[2]; // TODO: Document this structure.
        u8 friendStatusData = DWC_GetFriendStatusData(
            &FriendRosterManager::Instance()->m_dwcAccFriendData2[i],
            &friendInfo, &r5[0]);
        m_friends[i].dwcFriendStatus = friendStatusData;

        if (r5[0] == 8) {
          m_friends[i].statusData.roomId = friendInfo.statusData.roomId;
          m_friends[i].statusData.regionId = friendInfo.statusData.regionId;
          m_friends[i].statusData.status = friendInfo.statusData.status;
          m_friends[i].statusData.playerCount =
              friendInfo.statusData.playerCount;
          m_friends[i].statusData.currRace = friendInfo.statusData.currRace;
        } else {
          m_friends[i].statusData.roomId = 0;
          m_friends[i].statusData.status = 0;
        }
      }
    }
    m_shouldUpdateFriendStatus = false;
  }
}

bool NetManager::hasDisconnected(u32 playerId) {
  return (1 << playerId & m_disconnectedPlayerIds) != 0;
}

RecordHolder::RecordHolder(u32 bufferSize) {
  m_packet = nullptr;
  m_bufferSize = bufferSize;
  m_packetSize = 0;
  m_packet = operator new[](bufferSize);
  reset();
}

RecordHolder::~RecordHolder() {
  delete m_packet;
  m_packet = 0;
}

void RecordHolder::reset() {
  memset(m_packet, 0, m_bufferSize);
  m_packetSize = 0;
}

void RecordHolder::copy(void* src, u32 len) {
  memcpy(m_packet, src, len);
  m_packetSize = len;
}

void RecordHolder::append(void* src, u32 len) {
  memcpy((void*)((u32)m_packet + m_packetSize), src, len);
  m_packetSize += len;
}

const u32 recordSizes[9] = {0x10, 0x28, 0x28, 0x38, 0x80,
                            0xc0, 0x10, 0xf8, 0x2e0};

RacePacketHolder::RacePacketHolder() {
  for (u32 i = 0; i < ARRAY_SIZE(m_records); i++) {
    m_records[i] = new RecordHolder(recordSizes[i]);
  }
}

RacePacketHolder::~RacePacketHolder() {
  for (u32 i = 0; i < ARRAY_SIZE(m_records); i++) {
    delete m_records[i];
  }
}

void RacePacketHolder::clear() {
  for (u32 i = 0; i < 8; i++) {
    holder(i)->reset();
  }
}

} // namespace Net
