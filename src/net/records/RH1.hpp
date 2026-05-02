#pragma once

#include "system/ResourceManager.hpp"

#include <rk_types.h>

namespace Net {

struct RH1Record {
  u32 elapsedTimeSinceRaceStart;
  u32 seed;
  u16 p1Team;
  u16 p2Team;
  u16 lagFrames;
  u8 p1Vehicle;
  u8 p2Vehicle;
  u8 p1Character;
  u8 p2Character;
  u16 countDownTime;
  u16 starRank;
  u8 coursePlayed;
  u8 unk17; // This i
  u8 aidMap[12];
  u8 engineClass;
  u8 _25[0x28 - 0x25];
};
static_assert(sizeof(RH1Record) == 0x28);

struct RH1Player {
  u32 timer;
  System::VehicleId p1Vehicle;
  System::VehicleId p2Vehicle;
  System::CharacterId p1Character;
  System::CharacterId p2Character;
  // This can take on values outside of the current courseId enum, maybe they're
  // different types?
  System::CourseId courseId;
  u8 _18[0x1c - 0x18];
  u32 RH1Seed;
  u8 aidMap[12];
  u8 _2c[0x30 - 0x2c];
};
static_assert(sizeof(RH1Player) == 0x30);

class RH1Handler {
public:
  void setPrepared();

  void reset();

  bool courseValid() const;

  inline s32 getCourseId() const;

  const u8* getPlayerIdToAidMapping() const;

  static RH1Handler* Instance() { return spInstance; }

private:
  bool m_prepared;
  u8 _001[0x004 - 0x001];

  // bit field for aids currently racing. This is set by
  // RaceConfig::Scenario::Settings::Seed1 which is 0 when in the globe scene or
  // spectating, and 1 when in racing. This is used to determine a few different
  // things, such as who needs a User record (sometimes if 0), who needs a
  // RaceData, etc
  u32 m_aidsInRace;

  // Bit field indexed by aid. Also set when importing a RH1 record from an aid.
  // When set for an aid, you won't export RH1Record._17 as 1 anymore.
  u32 m_receivedRH1FromAid;

  u32 m_receivedNewRH1FromAid;

  u8 _010[0x020 - 0x010];
  RH1Player m_RH1Players[12];

  static RH1Handler* spInstance;
};
static_assert(sizeof(RH1Handler) == 0x260);

} // namespace Net
