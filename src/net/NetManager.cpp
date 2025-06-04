#include "NetManager.hpp"

#include "net/packets/RACEHEADER1.hpp"
#include "net/packets/ROOM.hpp"

#include "host_system/SystemManager.hpp"

#include <dwc/common/dwc_error.h>

namespace Net {

void NetManager::scheduleShutdown() { m_shutdownScheduled = true; }

void NetManager::startWWVSSearch(u8 localPlayerCount) {
  DisconnectInfo dcInfo;
  int code;
  DWC_GetLastErrorEx((int*)&code, (int*)&dcInfo.type);

  ConnectionState connState;

  // 4xxxx and 98xxx are sake erorrs, otherwise set connectionState
  if ((code / 10000) == 4 || (code / 1000) == 98) {
    connState = CONNECTION_STATE_SAKE_ERROR;
  } else {
    connState = m_connectionState;
  }

  // were online, start the match making
  if (connState == CONNECTION_STATE_IDLE) {
    initMMInfo();
    m_roomType = ROOMTYPE_VS_WW;
    m_matchMakingInfos[0].m_localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].m_localPlayerCount = localPlayerCount;
    // allows the logic in RACEHEADER1Handler_calc() to run
    RACEHEADER1Handler::getInstance()->setPrepared();
  }
}

void NetManager::startRegionalVSSearch(u8 localPlayerCount) {
  DisconnectInfo dcInfo;
  int code;
  DWC_GetLastErrorEx((int*)&code, (int*)&dcInfo.type);

  ConnectionState connState;

  if ((code / 10000) == 4 || (code / 1000) == 98) {
    connState = CONNECTION_STATE_SAKE_ERROR;
  } else {
    connState = m_connectionState;
  }

  if (connState == CONNECTION_STATE_IDLE) {
    this->initMMInfo();
    m_roomType = ROOMTYPE_VS_REGIONAL;
    m_matchMakingInfos[0].m_localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].m_localPlayerCount = localPlayerCount;
    RACEHEADER1Handler::getInstance()->setPrepared();
  }
}

void NetManager::startWWBattleSearch(u8 localPlayerCount) {
  DisconnectInfo dcInfo;
  int code;
  DWC_GetLastErrorEx((int*)&code, (int*)&dcInfo.type);

  ConnectionState connState;

  if ((code / 10000) == 4 || (code / 1000) == 98) {
    connState = CONNECTION_STATE_SAKE_ERROR;
  } else {
    connState = m_connectionState;
  }

  if (connState == CONNECTION_STATE_IDLE) {
    initMMInfo();
    m_matchMakingInfos[0].m_localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].m_localPlayerCount = localPlayerCount;
    m_roomType = ROOMTYPE_BATTLE_WW;
    RACEHEADER1Handler::getInstance()->setPrepared();
  }
}

void NetManager::startRegionalBattleSearch(u8 localPlayerCount) {
  DisconnectInfo dcInfo;
  s32 code;
  DWC_GetLastErrorEx((int*)&code, (int*)&dcInfo.type);

  ConnectionState connState;

  if ((code / 10000) == 4 || (code / 1000) == 98) {
    connState = CONNECTION_STATE_SAKE_ERROR;
  } else {
    connState = m_connectionState;
  }

  if (connState == CONNECTION_STATE_IDLE) {
    initMMInfo();
    m_matchMakingInfos[0].m_localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].m_localPlayerCount = localPlayerCount;
    m_roomType = ROOMTYPE_BATTLE_REGIONAL;
    RACEHEADER1Handler::getInstance()->setPrepared();
  }
}

void NetManager::joinFriendPublicVS(u32 friendRosterId, u8 localPlayerCount) {
  // DisconnectInfo dcInfo;
  s32 type;
  s32 code;
  DWC_GetLastErrorEx((int*)&code, (int*)&type);

  ConnectionState connState;

  if ((code / 10000) == 4 || (code / 1000) == 98) {
    connState = CONNECTION_STATE_SAKE_ERROR;
  } else {
    connState = m_connectionState;
  }

  if (connState == CONNECTION_STATE_IDLE) {
    SearchType searchType = getFriendSearchType(friendRosterId);

    // This really feels like an else if, but I can't get that to match at
    // all... this worked better:
    /*
     *  if (searchType != SEARCH_TYPE_VS_WW) {
     *    if (searchType != SEARCH_TYPE_VS_REGIONAL) {
     *      m_roomType = ROOMTYPE_JOINING_FRIEND_WW;
     *    }
     *  }
     */
    if (searchType == SEARCH_TYPE_VS_WW) {
      m_roomType = ROOMTYPE_JOINING_FRIEND_WW;

    } else if (searchType == SEARCH_TYPE_VS_REGIONAL) {
      m_roomType = ROOMTYPE_JOINING_FRIEND_REGIONAL;
    } else {
      OSLockMutex(&m_mutex);

      if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
        m_disconnectInfo.type = DISCONNECT_TYPE_CANT_JOIN_FRIEND;
        m_disconnectInfo.code = 0;
      }

      OSUnlockMutex(&m_mutex);
      return;
    }

    initMMInfo();
    m_matchMakingInfos[0].m_hostFriendId = friendRosterId;
    m_matchMakingInfos[1].m_hostFriendId = friendRosterId;
    m_matchMakingInfos[0].m_localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].m_localPlayerCount = localPlayerCount;
    RACEHEADER1Handler::getInstance()->setPrepared();
  }
}

void NetManager::joinFriendPublicBT(u32 friendRosterId, u8 localPlayerCount) {
  s32 type;
  s32 code;
  DWC_GetLastErrorEx((int*)&code, (int*)&type);

  ConnectionState connState;

  if ((code / 10000) == 4 || (code / 1000) == 98) {
    connState = CONNECTION_STATE_SAKE_ERROR;
  } else {
    connState = m_connectionState;
  }

  if (connState == CONNECTION_STATE_IDLE) {
    SearchType searchType = getFriendSearchType(friendRosterId);

    if (searchType == SEARCH_TYPE_BT_WW) {
      m_roomType = static_cast<RoomType>(9);
    } else if (searchType == SEARCH_TYPE_BT_REGIONAL) {
      m_roomType = static_cast<RoomType>(10);
    } else {
      OSLockMutex(&m_mutex);
      if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
        m_disconnectInfo.type = DISCONNECT_TYPE_CANT_JOIN_FRIEND;
        m_disconnectInfo.code = 0;
      }

      OSUnlockMutex(&m_mutex);
      return;
    }

    initMMInfo();
    m_matchMakingInfos[0].m_hostFriendId = friendRosterId;
    m_matchMakingInfos[1].m_hostFriendId = friendRosterId;
    m_matchMakingInfos[0].m_localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].m_localPlayerCount = localPlayerCount;
    RACEHEADER1Handler::getInstance()->setPrepared();
  }
}

void NetManager::joinFriendRoom(u32 friendRosterId, u8 localPlayerCount) {
  s32 type;
  s32 code;
  DWC_GetLastErrorEx((int*)&code, (int*)&type);

  ConnectionState connState;

  if ((code / 10000) == 4 || (code / 1000) == 98) {
    connState = CONNECTION_STATE_SAKE_ERROR;
  } else {
    connState = m_connectionState;
  }

  if (connState == CONNECTION_STATE_IDLE) {
    initMMInfo();
    m_matchMakingInfos[0].m_hostFriendId = friendRosterId;
    m_matchMakingInfos[1].m_hostFriendId = friendRosterId;
    m_roomType = ROOMTYPE_NONHOST_PRIVATE;
    m_matchMakingInfos[0].m_localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].m_localPlayerCount = localPlayerCount;
    ROOMHandler::getInstance()->init(ROOM_ROLE_GUEST);
  }
}

void NetManager::createFriendRoom(u8 localPlayerCount) {
  s32 type;
  s32 code;
  DWC_GetLastErrorEx((int*)&code, (int*)&type);

  ConnectionState connState;

  if ((code / 10000) == 4 || (code / 1000) == 98) {
    connState = CONNECTION_STATE_SAKE_ERROR;
  } else {
    connState = m_connectionState;
  }

  if (connState == CONNECTION_STATE_IDLE) {
    initMMInfo();
    m_roomType = ROOMTYPE_NONHOST_PRIVATE;
    m_matchMakingInfos[0].m_localPlayerCount = localPlayerCount;
    m_matchMakingInfos[1].m_localPlayerCount = localPlayerCount;
    ROOMHandler::getInstance()->init(ROOM_ROLE_HOST);
  }
}

void NetManager::resetRH1andROOM() {
  if (RACEHEADER1Handler::getInstance() != nullptr) {
    RACEHEADER1Handler::getInstance()->reset();
  }
  if (ROOMHandler::getInstance() != nullptr) {
    ROOMHandler::getInstance()->initAndZero();
  }
  m_transitionType = TRANSITIONTYPE_WW_VS;
}
void NetManager::setToPrivateRoomTransitionType() {
  if (m_transitionType == TRANSITIONTYPE_WW_VS) {
    return;
  }
  m_transitionType = TRANSITIONTYPE_PRIVATE;
}

void NetManager::setToUnkRoomTransitionType() {
  if (m_transitionType == TRANSITIONTYPE_WW_VS) {
    return;
  }
  m_transitionType = TRANSITIONTYPE_UNK;
}

void NetManager::setDisconnectInfo(DisconnectType dcType, s32 errorCode) {
  OSLockMutex(&m_mutex);

  if (dcType == DISCONNECT_TYPE_DISK_EJECTED) {
    m_hasEjectedDisk = true;
  } else if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
    m_disconnectInfo.type = dcType;
    m_disconnectInfo.code = errorCode;
    if (static_cast<u32>(dcType - 1) <= DISCONNECT_TYPE_ERROR_CODE ||
        dcType == DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
      m_connectionState = CONNECTION_STATE_ERROR;
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
  m_hasEjectedDisk = false;
  OSLockMutex(&m_mutex);
  if (m_disconnectInfo.type != DISCONNECT_TYPE_UNRECOVERABLE_ERROR) {
    m_disconnectInfo.type = DISCONNECT_TYPE_NONE;
    m_disconnectInfo.code = 0;
  }
  OSUnlockMutex(&m_mutex);
}

s32 NetManager::getTimeDiff() {

  u32 time = (s32) (m_matchMakingInfos[m_currentSub].m_timeOfMatchMaking);
  if (m_matchMakingInfos[m_currentSub].m_timeOfMatchMaking == 0) {
    return 0;
  } else {
    OSTime currTime = OSGetTime();
    return ((s32)currTime - time) / (__OSBusClock / 4);
  }
}

void NetManager::updateDWCServersAysnc() {

}


SearchType NetManager::getFriendSearchType(u32 friendRosterId) const {

  // If the friend hasn't added back, return early
  if (m_friends[friendRosterId].hasAddedBack == false ||
      m_friends[friendRosterId]._8 == 0) {
    return SEARCH_TYPE_UNK_1;
  }
  FriendStatus friendStatus =
      static_cast<FriendStatus>(m_friends[friendRosterId].status.status);

  if (friendStatus != FRIEND_STATUS_INACTIVE) {

    // If the player is in a froom (host, guest, battle, race)
    if (friendStatus < FRIEND_STATUS_PRIVATE_VS_HOST ||
        friendStatus > FRIEND_STATUS_PRIVATE_BT_GUEST) {

      //
      if (friendStatus < FRIEND_STATUS_IDLE ||
          friendStatus > FRIEND_STATUS_GUEST) {

        switch (friendStatus) {
        case FRIEND_STATUS_PUBLIC_VS: {
          //
          if (m_searchParamP != 0) {
            return SEARCH_TYPE_UNK_2;
          }
          s8 regionId = m_friends[friendRosterId].status.regionId;
          if (regionId != -1) {
            System::SystemManager* systemManager =
                System::SystemManager::sInstance;
            SearchType searchType = SEARCH_TYPE_UNK_7;
            if (regionId != systemManager->mMatchingArea) {
              return searchType;
            }
            return SEARCH_TYPE_VS_REGIONAL;
          }
          return SEARCH_TYPE_VS_WW;
        }
        case FRIEND_STATUS_PUBLIC_BATTLE: {
          if (m_searchParamP != 0) {
            return SEARCH_TYPE_UNK_2;
          }
          s8 regionId = m_friends[friendRosterId].status.regionId;
          if (regionId == -1) {
            System::SystemManager* systemManager =
                System::SystemManager::sInstance;
            SearchType searchType = SEARCH_TYPE_UNK_A;
            if (regionId != systemManager->mMatchingArea) {
              return searchType;
            }
            return SEARCH_TYPE_BT_REGIONAL;
          }
          return SEARCH_TYPE_BT_WW;
        }
        }
      }
      return static_cast<SearchType>(friendStatus);
    }
    return static_cast<SearchType>(friendStatus);
  }
  return SEARCH_TYPE_UNK_2;
}

} // namespace Net
