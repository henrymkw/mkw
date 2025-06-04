// Credits: Used as reference for naming, see bottom of file for the licenses
// https://github.com/MelgMKW/Pulsar/blob/main/GameSource/MarioKartWii/RKNet/RKNetController.hpp
// https://github.com/CLF78/OpenPayload/blob/master/payload/game/net/RKNetController.hpp

#pragma once

#include "net/packets/RACEPacketHolder.hpp"

#include <egg/core/eggExpHeap.hpp>
#include <egg/core/eggTaskThread.hpp>
#include <rvl/os/osMutex.h>

#include <rk_types.h>

#include <decomp.h>

namespace Net {

enum SearchType {
  SEARCH_TYPE_UNK_1 = 0x1,
  SEARCH_TYPE_UNK_2 = 0x2,
  SEARCH_TYPE_OPEN_ROOM = 0x3,
  SEARCH_TYPE_PLAYING_WITH_FRIENDS = 0x4,
  SEARCH_TYPE_VS_WW = 0x5,
  SEARCH_TYPE_VS_REGIONAL = 0x6,
  SEARCH_TYPE_UNK_7 = 0x7,
  SEARCH_TYPE_BT_WW = 0x8,
  SEARCH_TYPE_BT_REGIONAL = 0x9,
  SEARCH_TYPE_UNK_A = 0xA,
};

enum FriendStatus {
  FRIEND_STATUS_INACTIVE = 0x0,
  FRIEND_STATUS_IDLE = 0x2,         // online, but not doing anything
  FRIEND_STATUS_HOSTING_ROOM = 0x3, //
  FRIEND_STATUS_GUEST = 0x4,
  FRIEND_STATUS_PUBLIC_VS = 0x5,
  FRIEND_STATUS_PUBLIC_BATTLE = 0x8,
  FRIEND_STATUS_PRIVATE_VS_HOST = 0xb,
  FRIEND_STATUS_PRIVATE_VS_GUEST = 0xc,
  FRIEND_STATUS_PRIVATE_BT_HOST = 0xd,
  FRIEND_STATUS_PRIVATE_BT_GUEST = 0xe,
};

enum ConnectionState {
  CONNECTION_STATE_OFFLINE = 0x0,
  CONNECTION_STATE_BEGIN_LOGIN = 0x1,
  CONNECTION_STATE_2 = 0x2, // Needs profanity check maybe?
  CONNECTION_STATE_3 = 0x3, // Passed profanity check, needs to sync friends?
  CONNECTION_STATE_SOMETHING_FRIENDS = 0x4,
  CONNECTION_STATE_IDLE = 0x5,
  CONNECTION_STATE_IN_MM = 0x6,
  CONNECTION_STATE_ERROR = 0x7, // All errors except for the last one??
  CONNECTION_STATE_SAKE_ERROR =
      0x8, // Set when the EC is 4xxxx or 98xxx, which is checked for a lot
};

enum DisconnectType {
  DISCONNECT_TYPE_NONE = 0x0,
  DISCONNECT_TYPE_ERROR_CODE = 0x1,
  DISCONNECT_TYPE_BAD_MII_NAME = 0x2,
  DISCONNECT_TYPE_CANT_JOIN_FRIEND = 0x3,
  DISCONNECT_TYPE_DISK_EJECTED =
      0x4, // used when disk is ejected during connection
  DISCONNECT_TYPE_UNRECOVERABLE_ERROR = 0x5,
};

enum RoomType {
  ROOMTYPE_NONE = 0x0,
  ROOMTYPE_VS_WW = 0x1,
  ROOMTYPE_VS_REGIONAL = 0x2,
  ROOMTYPE_BATTLE_WW = 0x3,
  ROOMTYPE_BATTLE_REGIONAL = 0x4,
  ROOMTYPE_HOST_PRIVATE = 0x5,
  ROOMTYPE_NONHOST_PRIVATE = 0x6,
  ROOMTYPE_JOINING_FRIEND_WW = 0x7,
  ROOMTYPE_JOINING_FRIEND_REGIONAL = 0x8,
};

enum ErrorType {
  ERRORTYPE_NONE = 0x0,
  ERRORTYPE_CANTJOIN = 0x3,
};

enum UnkEC {
  NONE = 0x0,
  DISCONNECT_RACE = 0x2,
};

enum TransitionType {
  TRANSITIONTYPE_NONE = 0x0,
  TRANSITIONTYPE_WW_VS = 0x1,
  TRANSITIONTYPE_PRIVATE = 0x2,
  TRANSITIONTYPE_UNK = 0x3,
};

struct DisconnectInfo {
  DisconnectType type;
  s32 code;
};

// Name is preliminary.
class NetManager {
public:
  static NetManager* createStaticInstance(EGG::ExpHeap* heap);

  static void destroyStaticInstance();

  void init(u8 localPlayerCount);

  void scheduleShutdown();

  void startWWVSSearch(u8 localPlayerCount);

  void startRegionalVSSearch(u8 localPlayerCount);

  void startWWBattleSearch(u8 localPlayerCount);

  void startRegionalBattleSearch(u8 localPlayerCount);

  void joinFriendPublicVS(u32 friendRosterId, u8 localPlayerCount);

  void joinFriendPublicBT(u32 friendRosterId, u8 localPlayerCount);

  void joinFriendRoom(u32 friendRosterId, u8 localPlayerCount);

  void createFriendRoom(u8 localPlayerCount);

  void mainNetworkLoop();

  void initMMInfo();

  SearchType getFriendSearchType(u32 friendRosterId) const;

  void resetRH1andROOM();

  void setToPrivateRoomTransitionType();

  void setToUnkRoomTransitionType();

  void setDisconnectInfo(DisconnectType dcType, s32 errorCode);

  DisconnectInfo getDisconnectInfo();

  void resetDisconnectInfo();

  s32 getTimeDiff();

  void updateDWCServersAysnc();

  struct recvPacketBuffer { // TODO: figure this struct out
    u8 recv[0x2e0];
  };

  struct StatusData {
    u32 hostDWCGroupId;
    u8 regionId;
    u8 status;
    u8 playerCount;
    u8 currRace;
  };

  struct Friend {
    StatusData status;
    u8 _8;
    bool hasAddedBack;
    u8 _a[0xc - 0xa];
  };

  struct RatingRangeMaker {
    u16 groupRanges[2][5][2];
  };

private:
  class MatchMakingInfo {
  public:
    struct numPlayersPerConsole {
      u8 numPlayers; // needed since its at
      u8 _1[3];
    };

    u64 m_timeOfMatchMaking;    // unsure but gets set upon match making
    u32 m_numConnectedConsoles; // number of non guest players
    u32 m_playerCount;          // players in room (includes guests)
    u32 m_fullAidBitmap;        // # bits is equal to num consoles, all 1
    u32 m_availableAidBitmap;   // will equal the full bitmap by end of mm
    u32 m_roomId;
    s32 m_hostFriendId; // -1 if host isn't a friend
    u8 m_localPlayerCount;
    u8 m_myAid;
    u8 m_hostAid;
    numPlayersPerConsole m_numPlayersConsoleArr[12];
    bool m_concludedMatchmaking;
    u8 _54[0x58 - 0x54];
  };
  static_assert(sizeof(MatchMakingInfo) == 0x58);

  u8 _0000[0x0008 - 0x0000]; // vtable and something else
  OSMutex m_mutex;
  EGG::ExpHeap* m_heap;
  EGG::TaskThread* m_taskThread;
  ConnectionState m_connectionState;
  DisconnectInfo m_disconnectInfo;
  u8 _0034[0x0038 - 0x0034];
  MatchMakingInfo m_matchMakingInfos[2];
  RoomType m_roomType;
  TransitionType m_transitionType; // unknown purpose but set when starting mm
                                   // then flips to 0
  RACEPacketHolder* m_sentRACEPackets[2][12];
  RACEPacketHolder* m_recvRACEPackets[2][12];
  PacketHolder<void*>* m_fullSendPackets[12];
  u64 m_timeOfLastSentRACE[12];
  u64 m_timeOfLastRecvRACE[12];
  u64 m_timeSinceLastSentToPlayer[12];
  u64 m_timeSinceLastRecvFromPlayer[12];
  u8 m_aidLastSentTo; // Aid of last player we sent to
  recvPacketBuffer m_RecvPackets[12];
  u8 _25e1[0x25e4 - 0x25e1];
  StatusData m_localStatusData;
  Friend m_friends[30];
  bool m_friendStatusChanged; // set when a friend adds back
  bool m_shutdownScheduled;   // set when logging off
  bool m_shouldGetFriendStatus;
  bool m_hasEjectedDisk; // Error/Disconnect related
  u8 _2759[0x275c - 0x2759];
  s32 m_numBadWords;  // set in the profanity check, credits to Pulsar
  u32 m_searchParamP; // some search parameter
  s32 m_vr;
  s32 m_br;
  s32 m_lastSendBufferUsed[12];
  s32 m_lastRecvBufferUsed[12][8];
  s32 m_currentSub; // current sub class used
  u8 m_aidsBelongingToPlayerIds[12];
  u32 m_disconnectedAids;
  u32 m_disconnectedPlayerIds;
  RatingRangeMaker m_rangeMaker;
  u8 _295c[0x2960 - 0x295c];
  u64 m_selectTimeRatios[12];
  u32 m_biggestRH1Timer;
  u8 _29c4[0x29c8 - 0x29c4];

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
