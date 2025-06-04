// credits: Seeky
// https://github.com/SeekyCt/mkw-structures/blob/master/eventhandler.h

#pragma once

#include <rk_types.h>

namespace Net {

enum EVENTEntryState {
  ENTRY_STATE_FREE = 0x0,
  ENTRY_STATE_FULL = 0x1,
  ENTRY_STATE_EXPIRED = 0x2,
};

enum EVENTItemAction {
  ITEM_ACTION_NONE = 0x0,
  ITEM_ACTION_USE = 0x1,
  ITEM_ACTION_SHOOT = 0x2,
  ITEM_ACTION_BREAKDRAGGED = 0x3,
  ITEM_ACTION_BREAK = 0x4,
  ITEM_ACTION_TC_LOST = 0x5,
  ITEM_ACTION_UNK = 0x6,
  ITEM_ACTION_DROP = 0x7,
};

struct EVENTEntry {
  u32 timestamp;
  u8 buffer[21];
  u8 state;
  // TODO: Is there a way for these to be enum instead of u8?
  u8 itemObjId;
  u8 itemEvent;
  u8 length;
  u8 _1d[0x20 - 0x1d];
};
static_assert(sizeof(EVENTEntry) == 0x20);

struct EVENTPacket {
  u8 entryHeaders[24];
  u8 entryData[0xe0];
};
static_assert(sizeof(EVENTPacket) == 0xF8);

class EVENTHandler {
private:
  bool m_isPrepared;
  u8 _0001[0x0004 - 0x0001];
  EVENTEntry m_sendEntries[24];
  EVENTEntry m_recvEntries[12][24];
  u32 recvEntryPresences[12][24];
  u32 freeSpaceInSendBuffer;
};
static_assert(sizeof(EVENTHandler) == 0x2b88); 

}
