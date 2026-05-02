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

void MiscPacketHandler::resetSendRaceDataRecords() {
  for (u8 aid = 0; aid < MAX_PLAYER_COUNT; aid++) {
    NetManager::Instance()->recvRaceData(aid)->reset();
  }
}

void MiscPacketHandler::resetSendRH2Records() {
  for (u8 aid = 0; aid < MAX_PLAYER_COUNT; aid++) {
    NetManager::Instance()->sentRH2(aid)->reset();
  }
}

MiscPacketHandler::MiscPacketHandler()
    : m_isPrepared(0), scheduleDisconnect(false), m_aidsLoadedIntoRace(0),
      m_aidsLastSentRoomOrSelect(0), m_aidsShouldStop(0), m_myLagFrames(0),
      m_countdownTime(3000) {
  EventHandler::createInstance();
  ItemHandler::createInstance();
}

u8 MiscPacketHandler::getAidFromPlayerId(s32 playerId) const {
  NetManager* NetManager = NetManager::Instance();
  if (playerId >= 0 && playerId < 12) {
    return NetManager->playerIdToAid(playerId);
  }
  return 0xff;
}

void MiscPacketHandler::updateAsSpectator() {
  exportAsSpectator();
  updateBitfields();

  NetManager* netManager = NetManager::Instance();

  if (netManager->isMatchMakingSuspended() == false) {
    netManager->setVoteMatchMakingSuspend();
  }

  EventHandler::Instance()->update();
  ItemHandler::Instance()->update();

  for (u32 playerId = 0;
       playerId < System::RaceConfig::spInstance->mRaceScenario.mPlayerCount;
       playerId++) {
    u8 aid = getAidFromPlayerId(playerId);

    // call RaceManager::stopPlayer(playerId) if any of the following are true:
    // 1. The playerId dced (by checking disconnectedPlayerIds bitmap)
    // 2. The aid is my aid (since were spectating)
    // 3. The aid isnt in fullRoomBitmap (meaning they don't exist)
    // 4. The aid is set in m_aidsShouldStop

    if (NetManager::Instance()->isPlayerDisconnected(playerId) ||
        (aid == NetManager::Instance()->myAid()) ||
        (!NetManager::Instance()->isAidUsed(aid)) ||
        ((m_aidsShouldStop.on(aid)) != 0)) {
      System::RaceManager::spInstance->stopPlayer(playerId);
    }
  }
}

// Scratch: https://decomp.me/scratch/dDr6n
bool MiscPacketHandler::isEveryoneInRace() const {
  if (!NetManager::Instance()->hasFoundMatch()) {
    return true;
  }

  AidBitmap<u8> aidsInRace = m_aidsLoadedIntoRace;
  aidsInRace.set(NetManager::Instance()->myAid());
  AidBitmap<u8> availableAids;

  u8 playerCount = System::RaceConfig::spInstance->mRaceScenario.mPlayerCount;
  for (u32 playerId = 0; playerId < playerCount; playerId++) {
    if (NetManager::Instance()->isPlayerDisconnected(playerId)) {
      continue;
    }

    if ((System::RaceManager::spInstance->getPlayer(playerId)->flags &
         System::DISCONNECTED)) {
      continue;
    }
    u32 aid = getAidFromPlayerId(playerId);
    if (NetManager::Instance()->isAidUsed(aid)) {
      availableAids.set(aid);
    }
  }

  // TODO: Fix & hack
  return (availableAids & aidsInRace) == availableAids;
}

// Scratch: https://decomp.me/scratch/zBfVq
u32 MiscPacketHandler::getPlayerElapsedTimeSinceRaceStart(u32 playerId) {
  NetManager* netManager = NetManager::Instance();

  u8 aid = getAidFromPlayerId(playerId);
  if (NetManager::Instance()->isAidUsed(aid)) {

    RecordHolder* RH1Holder = netManager->recvRH1(aid);
    if (RH1Holder->recordSize() != 0) {
      RH1Record* packet = RH1Holder->record<RH1Record>();
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

u16 MiscPacketHandler::getAidLagFrames(u8 aid) {
  if (NetManager::Instance()->isAidUsed(aid)) {
    if (aid == NetManager::Instance()->mmInfo()->myAid) {
      return m_myLagFrames;
    }
    // next two lines are equal, ones an inline helper function for the latter
    // line
    return NetManager::Instance()->recvRH1(aid)->record<RH1Record>()->lagFrames;
  }
  return 0;
}

bool MiscPacketHandler::hasFreeEventEntries() {
  return EventHandler::Instance()->hasFreeEntries();
}

u32 MiscPacketHandler::getEventFreeSpace() {
  return EventHandler::Instance()->freeSpaceInSendBuffer();
}

bool MiscPacketHandler::isPlayerConnected(u32 playerId) {
  // Ensure playerId is valid (less than the playerCount)
  if (playerId < System::RaceConfig::spInstance->mRaceScenario.mPlayerCount) {
    // Player is connected if their aid is in the room, and they haven't
    // disconnected.

    return (NetManager::Instance()->isAidUsed(getAidFromPlayerId(playerId)) &&
            !NetManager::Instance()->isPlayerDisconnected(playerId));
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

u32 MiscPacketHandler::getLocalPlayerId(u32 playerId) {
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
    if (NetManager::Instance()->isPlayerDisconnected(playerId) ||
        !NetManager::Instance()->isAidUsed(aid)) {
      System::RaceManager::spInstance->stopPlayer(playerId);
    }
  }
}

void MiscPacketHandler::stopPlayersAsSpectator() {
  for (u32 playerId = 0;
       playerId < System::RaceConfig::spInstance->mRaceScenario.mPlayerCount;
       playerId++) {
    u8 aid = getAidFromPlayerId(playerId);
    if (NetManager::Instance()->isPlayerDisconnected(playerId) ||
        aid == NetManager::Instance()->myAid() ||
        !NetManager::Instance()->isAidUsed(aid) || m_aidsShouldStop.on(aid)) {
      System::RaceManager::spInstance->stopPlayer(playerId);
    }
  }
}

void MiscPacketHandler::updateBitfields() {
  for (u32 aid = 0; aid < 12; aid++) { // is t
    NetManager* netManager = NetManager::Instance();

    u32 aidSlot = 1 << (u8)aid;

    if (netManager->isAidUsed(aid) && aid != netManager->myAid()) {
      RecordHolder* holder = netManager->recvRH1(aid);

      if (holder->recordSize() != 0) {
        RH1Record* rh1Packet = reinterpret_cast<RH1Record*>(holder->m_packet);
        if (rh1Packet->seed != 0) {
          m_aidsLoadedIntoRace.set(aid);
        }
        if (rh1Packet->unk17 != 0) {
          m_aidsShouldStop.set(aid);
        }
      }

      if (NetManager::Instance()->recvSelect(aid)->recordSize() != 0) {
        m_aidsLastSentRoomOrSelect.set(aid);
      }
    }
  }
}
} // namespace Net
