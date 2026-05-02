#pragma once

#include <rk_types.h>
#include <rk_common.h>

#include "net/records/Event.hpp"
#include "net/records/RaceData.hpp"
#include "net/records/RH1.hpp"
#include "net/records/RH2.hpp"
#include "net/records/Select.hpp"

#include <rvl/os/os.h>

namespace Net {

/*
 * MiscPacketHandler gets created when switching to RaceScene and destroyed upon
 * RaceScene exit. Has a number of misc jobs related to packets (and some not).
 * Imports, exports, and clears a number of records. Handles frame lag, timer
 * differences, and disconnect logic. Also has ties with Event and Item records
 */
class MiscPacketHandler {
public:
  // gets called on scene change
  static void createInstance();

  MiscPacketHandler();

  void clearAidInRace(u32 aid);

  inline u8 getAidFromPlayerId(s32 playerId) const;

  void setRH2Record(RH2Record* packet);

  void resetSendRaceDataRecords();

  void resetSendRH2Records();

  void createAndCopyRH1andUSERPackets();

  // gets inlined in update()
  void updateAsSpectator();

  // 0x80653728
  // called by RaceScene::calcSubsystems(). Though its ran every frame (while in
  // RaceScene) it's logic will only run when m_isPrepared is true. This runs in
  // online races but also rival ghost races to update friend status
  void update();

  // 0x80654150
  // Main online race update logic. Responsibilities include:
  // - starting and synchronizing the start of a race
  // - suspending/unsuspending match making at the start/end of a race
  // - calling Item and Event handler's update() and processLagFrames() during a
  // race
  // - Handling player disconnections
  void updateAsRacer();

  // 0x80654d08
  // Called by updateAsRacer(). Only RH1 record gets created since RaceData
  // is already created at this point. Both get exported to the send buffer.
  // Name isn't the cleanest and can be improved.
  void createExportRH1ExportRaceData();

  // 0x806554a0
  // Processes all received RH1 records into member fields
  void processRecvRH1Records();

  // 0x80654038
  // It checks that all aids sent over a RH1Seed (via m_aidsWithRH1Seed)
  // If all players have sent over a RH1Seed, they're all in the race,
  // meaning the countdown can start. Called by updateAsRacer() to check if the
  // countdown can start.
  bool isEveryoneInRace() const;

  // 0x80654b00
  // tbd better explanation, loops over recv RH1 records and compares other
  // players lag frames with ours. If another player's lag frames are greater
  // than ours, set GameScene + 0x2539 (maybe called laggedFrame?). This causes
  // GameScene::update() to call beginFrame() and endFrame(), which I'd assume
  // skips a frame? Also differentiates between 60 and 30 fps, to skip one frame
  // for 60fps and 2 for 30fps.
  void processLagFrames();

  // 0x806544a8
  // returns 0 if playerId isn't in the room
  // returns aidTimeSinceRaceStart if in a race (?) and difference between
  // m_elapsedTimeSinceRaceStart and aid's time is less than 600 ms (not a bunch
  // of) returns m_elapsedTimeSinceRaceStart otherwise
  u32 getPlayerElapsedTimeSinceRaceStart(u32 playerId);

  u16 getAidLagFrames(u8 aid);

  bool hasFreeEventEntries();

  u32 getEventFreeSpace();

  inline bool isPlayerIdInRoom(u32 playerId);

  inline bool isAidNotInRoom(u32 aid);

  inline bool isAidInRoom(u32 aid) const;

  inline bool isAidInRoomNoHelper(u32 aid) const;

  inline bool isAidNotInRoomNoHelper(u32 aid) const;

  inline bool isAidSlotInRoom(u32 aidSlot) const;

  inline bool isPlayerDisconnected(u32 playerId) const;

  bool isPlayerConnected(u32 playerId);

  bool isPlayerLocal(u32 playerId);

  u32 getLocalPlayerId(u32 playerId);

  void stopDisconnectedPlayers();

  // Inlined, has a few extra checks
  void stopPlayersAsSpectator();

  void updateBitfields();

  static MiscPacketHandler* Instance() { return spInstance; }

private:
  // Controls when to start/stop MiscPacketHandler from running
  bool m_isPrepared;

  // Triggers a disconnect when set
  bool scheduleDisconnect;
  u8 _002[0x004 - 0x002];

  // Bit field indexed by aid that indicates whether that aid has loaded into
  // the race. This is used to determine when to count the startdown and the
  // type of records to send to a given aid
  u32 m_aidsLoadedIntoRace;

  // Bit field indexed by aid. 0 indicates last sent a Room, 1 indicates last
  // sent Select
  u32 m_aidsLastSentRoomOrSelect;

  // Bit field indexed by aid. Partially used to determine stopping a player
  // (disconnect animation)
  u32 m_aidsShouldStop;

  // TODO: Explanation
  u16 m_myLagFrames; // 0x10

  // TODO: Explanation
  u16 m_countdownTime; // 0x12

  // RaceData records to send. One per local player.
  RaceDataRecord m_sendRaceDataRecords[2]; // 0x14 - 0x94

  // Event record to send.
  EventRecord m_sendEventRecords; // 0x94 - 0x18c
  u8 _18c[0x190 - 0x18c];

  // Time since last checked for friend status update.
  OSTime m_elapsedTicksSinceFriendUpdate;

  // TODO: Explanation
  u32 m_aidsTimeSinceRaceStart[MAX_PLAYER_COUNT];

  static MiscPacketHandler* spInstance;
};
static_assert(sizeof(MiscPacketHandler) == 0x1c8);

} // namespace Net
