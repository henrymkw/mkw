#pragma once

// Credits: Melg and CLF, used as reference for names.
// See bottom of file for the licenses
// https://github.com/MelgMKW/Pulsar/blob/main/GameSource/MarioKartWii/RKNet/RKNetController.hpp
// https://github.com/CLF78/OpenPayload/blob/master/payload/game/net/RKNetController.hpp

#include <rk_types.h>
#include <decomp.h>

#include "net/DisconnectInfo.hpp"
#include "net/FriendInfo.hpp"
#include "net/records/RacePacketHeader.hpp"
#include "net/records/RH1.hpp"
#include "net/records/User.hpp"

#include <egg/core/eggExpHeap.hpp>
#include <egg/core/eggTaskThread.hpp>
#include <dwc/core/dwc_friend.h>
#include <dwc/core/dwc_match.h>

#include <gamespy/GP/gp.h>

namespace Net {

enum RecordId {
  HEADER_RECORD,
  RH1_RECORD,
  RH2_RECORD,
  SELECT_RECORD,
  RACE_DATA_RECORD,
  USER_RECORD,
  ITEM_RECORD,
  EVENT_RECORD
};
// Packet ID constants
static const u32 HEADERPacketId = 0;
static const u32 RACEHEADER1PacketId = 1;
static const u32 RACEHEADER2PacketId = 2;
static const u32 SELECTPacketId = 3;
static const u32 RACEDATAPacketId = 4;
static const u32 USERPacketId = 5;
static const u32 ITEMPacketId = 6;
static const u32 EVENTPacketId = 7;

class RecordHolder {
public:
  RecordHolder(u32 bufferSize);
  ~RecordHolder();

  void reset();

  void copy(void* src, u32 len);

  void append(void* src, u32 len);

  template <typename T> T* getPacket() {
    return reinterpret_cast<T*>(m_packet);
  }
  u32 getBufferSize() { return m_bufferSize; }
  u32 getPacketSize() { return m_packetSize; }

  void* m_packet;
  u32 m_bufferSize;
  u32 m_packetSize;
};

class RacePacketHolder {
public:
  RecordHolder* header() { return m_records[HEADERPacketId]; }

  RecordHolder* rh1() { return m_records[RACEHEADER1PacketId]; }

  RecordHolder* rh2() { return m_records[RACEHEADER2PacketId]; }

  RecordHolder* select() { return m_records[SELECTPacketId]; }

  RecordHolder* raceData() { return m_records[RACEDATAPacketId]; }

  RecordHolder* user() { return m_records[USERPacketId]; }

  RecordHolder* item() { return m_records[ITEMPacketId]; }

  RecordHolder* event() { return m_records[EVENTPacketId]; }

  RecordHolder* holder(u32 idx) { return m_records[idx]; }

  inline void clear();

  RecordHolder* m_records[8];

  // 0x8065a3dc
  RacePacketHolder();
  // 0x8065a474
  ~RacePacketHolder();
};
static_assert(sizeof(RacePacketHolder) == 0x20);

class NetManager {
public:
  enum ConnectionState {
    CONNECTION_STATE_SHUTDOWN = 0x0, // offline
    CONNECTION_STATE_BEGIN_LOGIN = 0x1,
    CONNECTION_STATE_CHECK_PROFANITY = 0x2,
    CONNECTION_STATE_SOMETHING_GP_FRIENDS =
        0x3,                             // Friend related, needs better name
    CONNECTION_STATE_SYNC_FRIENDS = 0x4, // checks if a friend has added back
    CONNECTION_STATE_IDLE = 0x5,         // online but not doing anything
    CONNECTION_STATE_IN_MM = 0x6,        // searching or in a room
    CONNECTION_STATE_ERROR = 0x7,        // non SAKE errors
    CONNECTION_STATE_SAKE_ERROR = 0x8,   // set when the EC is 4xxxx or 98xxx
  };

  enum RoomType {
    ROOM_TYPE_NONE = 0x0,
    ROOM_TYPE_VS_WW = 0x1,
    ROOM_TYPE_VS_REGIONAL = 0x2,
    ROOM_TYPE_BT_WW = 0x3,
    ROOM_TYPE_BT_REGIONAL = 0x4,
    ROOM_TYPE_HOST_PRIVATE = 0x5,
    ROOM_TYPE_NONHOST_PRIVATE = 0x6,
    ROOM_TYPE_JOINING_FRIEND_VS_WW = 0x7,
    ROOM_TYPE_JOINING_FRIEND_VS_REGIONAL = 0x8,
    ROOM_TYPE_JOINING_FRIEND_BT_WW = 0x9,
    ROOM_TYPE_JOINING_FRIEND_BT_REGIONAL = 0xA,
  };

  // Only applies when m_connectionState == CONNECTION_STATE_IN_MM.
  // MatchMakingInfo::isMatchMakingSuspended controls whether others can join a
  // room. To change its value, clients vote for the room to be
  // suspended/unsuspended. The host sets the flag by unanimus vote.
  enum VoteMatchMakingSuspended {
    VOTE_MM_NONE = 0x0,
    VOTE_MM_DISCONNECTED = 0x1, // Triggers a dc
    VOTE_MM_SUSPEND =
        0x2, // Set when ending a public race or starting a private room
    VOTE_MM_UNSUSPEND = 0x3, // Set when private room ends
  };

  NetManager(EGG::ExpHeap* heap);

  ~NetManager();

  void scheduleShutdown();

  void startWWVSSearch(u8 localPlayerCount);

  void startRegionalVSSearch(u8 localPlayerCount);

  void startWWBattleSearch(u8 localPlayerCount);

  void startRegionalBattleSearch(u8 localPlayerCount);

  void joinFriendPublicVS(u32 friendIdx, u8 localPlayerCount);

  void joinFriendPublicBT(u32 friendIdx, u8 localPlayerCount);

  void joinFriendRoom(u32 friendIdx, u8 localPlayerCount);

  void createFriendRoom(u8 localPlayerCount);

  void resetRH1andROOM();

  void setDisconnectInfo(DisconnectType dcType, s32 errorCode);

  void setVoteMatchMakingSuspend();

  void setVoteMatchMakingUnsuspend();

  DisconnectInfo getDisconnectInfo();

  // Inline reset function for DisconnectInfo. Used in resetErrors() and init()
  inline void resetDisconnectInfo();

  // Resets m_hasEjectedDisk and m_disconnectInfo
  void resetErrors();

  s32 matchMakingElapsedSeconds();

  // Note, this gets inlined in a few places
  void updateDWCServersAsnycPassProfanity();

  void updateDWCServersAsync();

  void updateFriendsProfiles();

  void updateFriendsHasAddedBack(GPProfile* pidsThatAddedBack, u32 numPids);

  void resetFriendData(u32 friendIdx);

  bool isConnectionStateIdleOrInMM() const;

  bool isTaskThreadIdle();

  bool isConnectionStateIdle() const;

  bool hasFoundMatch() const;

  void clearRACEPacketPointers();

  void setConnectionStateIdle();

  void construct(EGG::ExpHeap* heap);

  void sendRaceUpdateUserPackets();

  void formRacePacket();

  void sendRacePacket();

  bool sendAidRacePacket(u8 aid);

  u32 getRACEPacketSize(u8 aid);

  void setConnectionState(ConnectionState connState);

  ConnectionState getConnectionState() const;

  void handleError();

  void* alloc(u32 size, s32 alignment);

  void free(void* block);

  // namesake of the alloc/free functions is the lib they're called by
  static void* SOAlloc(u32 unk, u32 size);

  static void SOFree(u32 unk, void* block);

  static void* DWCAlloc(u32 unk, u32 size, s32 alignment);

  static void DWCFree(u32 unk, void* block);

  static void loginCallback(u32 r3, u32 r4, NetManager* netManager);

  static void connectionCleanupCallback();

  inline static s32 totalPlayersHelper();

  static bool isTotalPlayersValid(DWCConnectionUserData* playerCountPtr,
                                  NetManager* self);

  static void updateFriendStatusCallback(u32 r3, u32 r4, void* self);

  static void setFriendRosterChangedCallback(u32 r3, u32 r4,
                                             NetManager* netManager);

  static void BLR_80658918();

  static void DWCSetBuddyFriendCallback(u32 r3, NetManager* netManager);

  void initRecvPacketBuffer();

  void handleAidDisconnect(u8 aid);

  void connect();

  void initMMInfos();

  void resetFriends();

  // unused ?
  void buildHeader(u8 aid);

  void calcOutgoingCRC32(u8 aid);

  void processRacePacket(u8 aid, RacePacketHeader* header, u32 size);

  void updateAidMapping();

  void resetPlayerIdToAidMap();

  inline s32 getLocalId(u32 hudId) const;

  inline bool myAidInRoom() const;

  s32 getLocalPlayerId(u32 hudId) const;

  FriendJoinableStatus getFriendJoinableStatus(u32 friendIdx) const;

  void updateStatusDatas();

  struct MatchMakingInfo;

  inline MatchMakingInfo* getMMInfo() {
    return &m_matchMakingInfos[m_currMMInfo];
  }

  inline u8 getMyAid() { return getMMInfo()->myAid; }

  inline bool hasDisconnected(u32 playerId);

  inline RacePacketHolder* lastRecvRacePacket(u8 aid, u32 type) {
    return m_recvRacePackets[m_lastRecvIdx[aid][type]][aid];
  }

  inline RecordHolder* getRecvRH1PacketHolder(u8 aid) {

    return lastRecvRacePacket(aid, RH1_RECORD)->rh1();
    /*
    RacePacketHolder** row =
        (RacePacketHolder**)
            netManager->m_recvRacePackets[netManager->m_lastRecvIdx[aid][1]];
    return row[aid]->m_records[1];
    */
  }

  RecordHolder* getSendRH2PacketHolder(u8 aid) {
    return m_sendRacePackets[m_lastSendIdx[aid]][aid]->rh2();
  }

  inline RecordHolder* getSendSelectPacketHolder(u8 aid) {
    return m_sendRacePackets[m_lastSendIdx[aid]][aid]->select();
  }

  inline RecordHolder* getRecvSelectPacketHolder(u8 aid) {
    return m_recvRacePackets[m_lastRecvIdx[aid][SELECTPacketId]][aid]->select();
  }

  inline RecordHolder* getSendRACEDATAPacketHolder(u8 aid) {
    return m_sendRacePackets[m_lastSendIdx[aid]][aid]->raceData();
  }

  static NetManager* Instance() { return spInstance; }
  // reason this exists is since the local player count must be in the highest
  // byte to be passed off to DWC functions

  struct MatchMakingInfo {       // 0x0038
    OSTime matchMakingStartTime; // gets set upon match making 0x0 / 0x0038
    u32 numConnectedConsoles;    // number of non guest players 0x8  / 0x0040
    u32 playerCount;   // players in room (includes guests) 0xC / 0x0044
    u32 availableAids; // # bits is equal to num consoles, all 1 0x10 / 0x0048
    u32 directConnectedAidBitmap; // Aids I'm connected to. It will fill up to
                                  // equal availableAids by the end of MM as
                                  // I connect to other users. 0x14 / 0x004c
    u32 roomId;                   // Also known as groupId by DWC 0x18 / 0x0050
    s32 hostFriendId;             // -1 if host isn't a friend. 0x1C / 0x0054
    u8 localPlayerCount;          // 0x20 / 0x0058
    u8 myAid;                     // 0x21 / 0x0059
    u8 hostAid; // value returned by DWC_GetServerAid() 0x22 / 0x005a
    DWCConnectionUserData localPlayerCounts[MAX_PLAYER_COUNT]; // 0x23 / 0x005b
    // When matching is suspended, friends aren't able to join your room.
    // This gets set to true during the voting screen in public rooms
    // and transitioning to opening a private room, both cases friends can't
    // join.
    bool isMatchMakingSuspended; // 0x53 / 0x008b
    u8 _54[0x58 - 0x54];
  };
  static_assert(sizeof(MatchMakingInfo) == 0x58);

  // Two vtables
  void* m_vtable1; // offset 0xc is NetManager's dtor
  void* m_vtable2; // unk dtor at 0xc, also present in FriendManager vtable
  OSMutex m_mutex;
  EGG::ExpHeap* m_heap;
  EGG::TaskThread* m_taskThread; // runs the mainLoop
  ConnectionState m_connectionState;
  DisconnectInfo m_disconnectInfo;
  u8 _0034[0x0038 - 0x0034];             // padding?
  MatchMakingInfo m_matchMakingInfos[2]; // 0x0038 - 0x00e8
  RoomType m_roomType;
  VoteMatchMakingSuspended m_voteMMSuspension;
  // points to RACE packets to be sent, two per aid / 0xf0
  RacePacketHolder* m_sendRacePackets[2][MAX_PLAYER_COUNT];
  // points to RACE packets to be recieved, two per aid / 0x150
  RacePacketHolder* m_recvRacePackets[2][MAX_PLAYER_COUNT];
  // The RACE packet to be sent, formed from m_sendRacePackets, one per aid /
  // 0x1b0
  RecordHolder* m_outgoingRACEPacket[MAX_PLAYER_COUNT];
  OSTime m_timeOfLastSentRACE[MAX_PLAYER_COUNT]; // 0x1e0
  OSTime m_timeOfLastRecvRACE[MAX_PLAYER_COUNT]; // 0x240
  OSTime
      m_timeBetweenSendingPackets[MAX_PLAYER_COUNT]; // time bewteen sent
                                                     // packets per aid / 0x2a0
  OSTime m_timeBetweenRecvPackets[MAX_PLAYER_COUNT]; // time between recieved
                                                     // packets per aid / 0x300
  u8 m_aidLastSentTo; // Aid of last player we sent to / 0x360
  u8 m_recvRACEPacketBuffer[MAX_PLAYER_COUNT][0x2e0]; // 0x361
  u8 _25e1[0x25e4 - 0x25e1];                          // padding
  StatusData m_myStatusData;                          // 0x25e4
  FriendInfo m_friends[MAX_FRIEND_COUNT];
  bool m_friendRosterChanged;      // set when a friend adds back 0x2753
  bool m_shutdownScheduled;        // set when logging off // 0x2755
  bool m_shouldUpdateFriendStatus; // 0x2756
  bool m_hasEjectedDisk;           // triggers a dc screen 0x2757
  bool m_profanityCheckFailed;     // 0x2758
  u8 _2759[0x275c - 0x2759];
  s32 m_badWordsNum; // number of bad words found in the profanity check
  u32 m_disconnectPenalty;
  s32 m_vr;
  s32 m_br;
  u32 m_lastSendIdx[MAX_PLAYER_COUNT]; // idx of m_sendRacePackets last sent per
                                       // aid
  // idx of m_recvRacePackets last recvieved per packet per aid
  u32 m_lastRecvIdx[MAX_PLAYER_COUNT][8];      // 0x279c
  u32 m_currMMInfo;                            // Current MM info used 0x291c
  u8 m_playerIdToAidMapping[MAX_PLAYER_COUNT]; // 0x2920
  u32 m_disconnectedAids;      // disconnected if 1 << aid is 1 // 0x292c
  u32 m_disconnectedPlayerIds; // disconnected if 1 << pid is 1 // 0x2930
  u8 _2934[0x295c - 0x2934];   // elo based MM struct
  u8 _295c[0x29c8 - 0x295c];   // some timers

  static NetManager* spInstance;
};
static_assert(sizeof(NetManager) == 0x29c8);
} // namespace Net

// MIT License

// Copyright (c) 2023 MelgMKW

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// MIT License

// Copyright (c) 2024 CLF78

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
