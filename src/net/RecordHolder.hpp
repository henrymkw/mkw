#pragma once

#include <rk_types.h>

namespace Net {

class RecordHolder {
public:
  RecordHolder(u32 bufferSize);
  ~RecordHolder();

  void reset();

  void copy(void* src, u32 len);

  void append(void* src, u32 len);

  template <typename T> T* getPacket() {
    return reinterpret_cast<T*>(m_packet);
  }
  u32 getBufferSize() { return m_bufferSize; }
  u32 getPacketSize() { return m_packetSize; }

  void* m_packet;
  u32 m_bufferSize;
  u32 m_packetSize;
};

} // namespace Net
