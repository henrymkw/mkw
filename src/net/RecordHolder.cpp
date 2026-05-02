#include "RecordHolder.hpp"

#include <string.h>

namespace Net {

RecordHolder::RecordHolder(u32 bufferSize) {
  m_packet = nullptr;
  m_bufferSize = bufferSize;
  m_packetSize = 0;
  m_packet = operator new[](bufferSize);
  reset();
}

RecordHolder::~RecordHolder() {
  delete m_packet;
  m_packet = 0;
}

void RecordHolder::reset() {
  memset(m_packet, 0, m_bufferSize);
  m_packetSize = 0;
}

void RecordHolder::copy(void* src, u32 len) {
  memcpy(m_packet, src, len);
  m_packetSize = len;
}

void RecordHolder::append(void* src, u32 len) {
  memcpy((void*)((u32)m_packet + m_packetSize), src, len);
  m_packetSize += len;
}

} // namespace Net
