#pragma once

#include <rk_types.h>

#include "net/RecordHolder.hpp"

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

} // namespace Net
