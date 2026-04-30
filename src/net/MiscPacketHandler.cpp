#include "MiscPacketHandler.hpp"

#include "net/NetManager.hpp"

#include "net/records/Event.hpp"
#include "net/records/Item.hpp"

#include "system/RaceConfig.hpp"
#include "system/RaceManager.hpp"

namespace Net {

void MiscPacketHandler::createInstance() {
  if (!spInstance) {
    spInstance = new MiscPacketHandler;
  }
}

void MiscPacketHandler::setRH2Record(RH2Record* packet) {
  NetManager* netManager = NetManager::Instance();
  NetManager::MatchMakingInfo* mmInfo = netManager->getMMInfo();

  /*
  for (u8 i = 0; i < MAX_PLAYER_COUNT; i++) {
    if (((1 << i) & mmInfo->m_fullAidBitmap) && (i != mmInfo->m_myAid)) {
      netManager->m_sendRACEPackets[netManager->m_lastSendIdx[i]][i]
          ->getRaceHeader1Packet()->copy(packet, 0x28);
    }
  }
  */
}

void MiscPacketHandler::resetSendRACEDATAPackets() {
  for (u8 aid = 0; aid < MAX_PLAYER_COUNT; aid++) {
    NetManager::Instance()->getSendRACEDATAPacketHolder(aid)->reset();
  }
}

void MiscPacketHandler::resetSendRACEHEADER2Packets() {
  for (u8 aid = 0; aid < MAX_PLAYER_COUNT; aid++) {
    NetManager::Instance()->getSendRH2PacketHolder(aid)->reset();
  }
}

void MiscPacketHandler::clearAidFromUnkBitfield(u32 aid) {
  m_aidsWithRH1Seed &= ~(1 << aid);
}

MiscPacketHandler::MiscPacketHandler()
    : m_isPrepared(0), scheduleDisconnect(false), m_aidsWithRH1Seed(0),
      m_aidsLastSentRoomOrSelect(0), m_aidsShouldStop(0), m_myLagFrames(0),
      m_countdownTime(3000) {
  EventHandler::createStaticInstance();
  ItemHandler::createStaticInstance();
}

u8 MiscPacketHandler::getAidFromPlayerId(s32 playerId) const {
  NetManager* NetManager = NetManager::Instance();
  if (playerId >= 0 && playerId < 12) {
    return NetManager->m_playerIdToAidMapping[playerId];
  }
  return 0xff;
}

void MiscPacketHandler::updateAsSpectator() {
  createAndCopyRH1andUSERPackets();
  updateBitfields();

  NetManager* netManager = NetManager::Instance();

  if (netManager->getMMInfo()->isMatchMakingSuspended == false) {
    netManager->setVoteMatchMakingSuspend();
  }

  EventHandler::Instance()->calc();
  ItemHandler::Instance()->calc();

  for (u32 playerId = 0;
       playerId < System::RaceConfig::spInstance->mRaceScenario.mPlayerCount;
       playerId++) {
    u8 aid = getAidFromPlayerId(playerId);

    // call RaceManager::stopPlayer(playerId) if any of the following are true:
    // 1. The playerId dced (by checking disconnectedPlayerIds bitmap)
    // 2. The aid is my aid (since were spectating)
    // 3. The aid isnt in fullRoomBitmap (meaning they don't exist)
    // 4. The aid is set in m_aidsShouldStop

    if (isPlayerDisconnected(playerId) ||
        (aid == NetManager::Instance()->getMyAid()) ||
        (isAidNotInRoomNoHelper(aid)) || ((m_aidsShouldStop & 1 << aid) != 0)) {
      System::RaceManager::spInstance->stopPlayer(playerId);
    }
  }
}

// Scratch: https://decomp.me/scratch/dDr6n
bool MiscPacketHandler::isEveryoneInRace() const {
  if (!NetManager::Instance()->hasFoundMatch()) {
    return true;
  }

  System::RaceManager* raceManager = System::RaceManager::spInstance;
  NetManager* netMgr = NetManager::Instance();

  u32 bitmap2 = m_aidsWithRH1Seed;
  bitmap2 |= 1 << netMgr->getMMInfo()->myAid;

  u32 bitmap = 0;

  u8 playerCount = System::RaceConfig::spInstance->mRaceScenario.mPlayerCount;
  for (u32 playerId = 0; playerId < playerCount;
       playerId++) { // This loop is inlined LOL
    if (!isPlayerDisconnected(playerId) &&
        (raceManager->getPlayer(playerId)->flags & System::DISCONNECTED) == 0) {
      u8 aid = getAidFromPlayerId(playerId);
      u32 aidSlot = 1 << aid;
      if ((aidSlot & netMgr->getMMInfo()->availableAids)) {
        bitmap |= aidSlot;
      }
    }
  }

  return (bitmap & bitmap2) == bitmap;
}

void MiscPacketHandler::update() {}

void MiscPacketHandler::updateAsRacer() {}

void MiscPacketHandler::createExportRH1ExportRaceData() {}

void MiscPacketHandler::processRecvRH1Records() {}

void MiscPacketHandler::processLagFrames() {}

// Scratch: https://decomp.me/scratch/zBfVq
u32 MiscPacketHandler::getRH1Timer(u32 playerId) {
  NetManager* netManager = NetManager::Instance();

  u8 aid = getAidFromPlayerId(playerId);
  if (isAidInRoom(aid)) {
    // u32 lastRecvIdx = netManager->m_lastRecvIdx[aid][1];
    u32 lastRecvIdx = netManager->m_lastRecvIdx[aid][1];

    // NetManager::RACEPacketHolder *holder =
    // netManager->m_recvRACEPackets[lastRecvIdx][aid];
    NetManager::RACEPacketHolder* holder =
        netManager->m_recvRACEPackets[netManager->m_lastRecvIdx[aid][1]][aid];

    NetManager::PacketHolder* RH1Holder = holder->getRaceHeader1PacketHolder();
    if (RH1Holder->getPacketSize() != 0) {
      RH1Record* packet = RH1Holder->getPacket<RH1Record>();
      if (System::RaceConfig::spInstance->mRaceScenario.mSettings.mCameraMode !=
              System::RaceConfig::Settings::CAMERA_MODE_LIVE_VIEW &&
          (600 <=
           packet->elapsedTimeSinceRaceStart - m_aidsTimeSinceRaceStart[aid])) {
        return m_aidsTimeSinceRaceStart[aid];
      }
      return packet->elapsedTimeSinceRaceStart;
    }
  }
  return 0;
}

u16 MiscPacketHandler::unk80654568(u8 aid) {
  if (isAidInRoom(aid)) {
    if (aid == NetManager::Instance()->getMMInfo()->myAid) {
      return m_myLagFrames;
    }
    // next two lines are equal, ones an inline helper function for the latter
    // line
    return NetManager::Instance()
        ->getRecvRH1PacketHolder(aid)
        ->getPacket<RH1Record>()
        ->lagFrames;
    // return
    // NetManager::getInstance()->m_recvRACEPackets[NetManager::getInstance()->m_lastRecvIdx[(u8)aid][1]][(u8)aid]->m_raceHeader1Packet->packet->_unkC;
  }
  return 0;
}

bool MiscPacketHandler::isEVENTfun8065b8d4() {
  return EventHandler::Instance()->unk8065b8d4();
}

u32 MiscPacketHandler::getEVENTUnk2b88() {
  return EventHandler::Instance()->_unk2b88;
}

bool MiscPacketHandler::isPlayerIdInRoom(u32 playerId) {
  // When the aid's bit is set, they're in the room (TODO: Does this update with
  // disconnections?)
  return ((1 << getAidFromPlayerId(playerId) &
           NetManager::Instance()
               ->m_matchMakingInfos[NetManager::Instance()->m_currMMInfo]
               .availableAids));
}

bool MiscPacketHandler::isAidNotInRoom(u32 aid) {
  // When the aid's bit is set, they're in the room (TODO: Does this update with
  // disconnections?)
  return ((1 << aid & NetManager::Instance()->getMMInfo()->availableAids) == 0);
}

bool MiscPacketHandler::isAidInRoom(u32 aid) const {
  // When the aid's bit is set, they're in the room (TODO: Does this update with
  // disconnections?)
  return ((1 << aid & NetManager::Instance()->getMMInfo()->availableAids) != 0);
}

bool MiscPacketHandler::isAidInRoomNoHelper(u32 aid) const {
  // When the aid's bit is set, they're in the room (TODO: Does this update with
  // disconnections?)
  return (
      (1 << aid & NetManager::Instance()
                      ->m_matchMakingInfos[NetManager::Instance()->m_currMMInfo]
                      .availableAids) != 0);
}

bool MiscPacketHandler::isAidNotInRoomNoHelper(u32 aid) const {
  // When the aid's bit is set, they're in the room (TODO: Does this update with
  // disconnections?)
  return (
      (1 << aid & NetManager::Instance()
                      ->m_matchMakingInfos[NetManager::Instance()->m_currMMInfo]
                      .availableAids) == 0);
}

bool MiscPacketHandler::isAidSlotInRoom(u32 aidSlot) const {
  // When the aid's bit is set, they're in the room (TODO: Does this update with
  // disconnections?)
  return (
      (aidSlot & NetManager::Instance()
                     ->m_matchMakingInfos[NetManager::Instance()->m_currMMInfo]
                     .availableAids) != 0);
}

bool MiscPacketHandler::isPlayerDisconnected(u32 playerId) const {
  // When playerId bit is set, they were in the room but disconnected. When its
  // 0, they aren't disconnected.
  return (1 << playerId & NetManager::Instance()->m_disconnectedPlayerIds);
}

bool MiscPacketHandler::isPlayerConnected(u32 playerId) {
  // Ensure playerId is valid (less than the playerCount)
  if (playerId < System::RaceConfig::spInstance->mRaceScenario.mPlayerCount) {
    // Player is connected if their aid is in the room, and they haven't
    // disconnected.
    return (isPlayerIdInRoom(playerId) && !isPlayerDisconnected(playerId));
  }
  return false;
}

bool MiscPacketHandler::isPlayerLocal(u32 playerId) {
  bool isPlayerIdLocal = false;
  if (playerId == NetManager::Instance()->getLocalPlayerId(0) ||
      playerId == NetManager::Instance()->getLocalPlayerId(1)) {
    isPlayerIdLocal = true;
  }
  return isPlayerIdLocal;
}

u32 MiscPacketHandler::getHudSlotId(u32 playerId) {
  for (u32 hudId = 0; hudId < 2; hudId++) {
    u32 localPlayerId = NetManager::Instance()->getLocalPlayerId(hudId);
    if (playerId == localPlayerId)
      return hudId;
  }
  return -1;
}

void MiscPacketHandler::stopDisconnectedPlayers() {
  for (u32 playerId = 0;
       playerId < System::RaceConfig::spInstance->mRaceScenario.mPlayerCount;
       playerId++) {
    // Stop a player locally if they have disconnected or they're not in the
    // full bitmap
    u8 aid = getAidFromPlayerId(playerId);
    if (isPlayerDisconnected(playerId) || isAidNotInRoom(aid)) {
      System::RaceManager::spInstance->stopPlayer(playerId);
    }
  }
}

void MiscPacketHandler::stopDisconnectPlayersField_C() {
  for (u32 playerId = 0;
       playerId < System::RaceConfig::spInstance->mRaceScenario.mPlayerCount;
       playerId++) {
    u8 aid = getAidFromPlayerId(playerId);
    if (isPlayerDisconnected(playerId) ||
        aid == NetManager::Instance()->getMMInfo()->myAid ||
        isAidNotInRoom(aid) || (m_aidsShouldStop & 1 << aid) != 0) {
      System::RaceManager::spInstance->stopPlayer(playerId);
    }
  }
}

void MiscPacketHandler::updateBitfields() {
  for (u32 aid = 0; aid < 12; aid++) { // is t
    NetManager* netManager = NetManager::Instance();

    u32 aidSlot = 1 << (u8)aid;

    if (isAidSlotInRoom(aidSlot) && (u32)aid != netManager->getMyAid()) {
      // NetManager::RACEPacketHolder **row =
      // (NetManager::RACEPacketHolder**)netManager->m_recvRACEPackets[netManager->m_lastRecvIdx[aid][1]];
      // NetManager::PacketHolder *holder = row[aid]->m_packets[1];
      NetManager::PacketHolder* holder =
          netManager->getRecvRH1PacketHolder(aid);

      if (holder->getPacketSize() != 0) {
        RH1Record* rh1Packet = reinterpret_cast<RH1Record*>(holder->m_packet);
        if (rh1Packet->seed != 0) {
          m_aidsWithRH1Seed |= aidSlot;
        }
        if (rh1Packet->unk17 != 0) {
          m_aidsShouldStop |= aidSlot;
        }
      }

      // Using the netManager variable doesn't work
      if (NetManager::Instance()
              ->getRecvSelectPacketHolder(aid)
              ->getPacketSize() != 0) {
        m_aidsLastSentRoomOrSelect |= aidSlot;
      }
    }
  }
}

} // namespace Net
