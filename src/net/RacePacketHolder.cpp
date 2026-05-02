#include "RacePacketHolder.hpp"

namespace Net {

const u32 recordSizes[9] = {0x10, 0x28, 0x28, 0x38, 0x80,
                            0xc0, 0x10, 0xf8, 0x2e0};

RacePacketHolder::RacePacketHolder() {
  for (u32 i = 0; i < ARRAY_SIZE(m_records); i++) {
    // issue, instructions are swapped:
    // current loads the size then sets m_packet = nullptr (which happens in
    // RecordHolder's inline ctor) target is the other way around
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
