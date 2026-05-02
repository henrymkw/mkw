#pragma once

#include <rk_types.h>

#define SLOT(aid) (1 << (aid))

namespace Net {

template <typename T> class AidBitmap {
public:
  AidBitmap() { m_bitField = 0; }
  AidBitmap(u32 val) { m_bitField = val; }

  inline bool off(T aid) const { return (SLOT(aid) & m_bitField) == 0; }

  inline bool on(T aid) const { return (SLOT(aid) & m_bitField) != 0; }

  inline void set(T aid) { m_bitField |= SLOT(aid); }

  inline void clear(T aid) { m_bitField &= ~SLOT(aid); }

  inline void reset() { m_bitField = 0; }

  inline bool none() const { return m_bitField == 0; }

  inline bool any() const { return m_bitField != 0; }

  inline AidBitmap& operator=(T val) {
    m_bitField = val;
    return *this;
  }
  inline AidBitmap& operator|=(T val) {
    m_bitField |= val;
    return *this;
  }
  inline AidBitmap operator|(const AidBitmap& rhs) const {
    return AidBitmap(m_bitField | rhs.m_bitField);
  }
  inline AidBitmap operator&(const AidBitmap& rhs) const {
    return AidBitmap(m_bitField & rhs.m_bitField);
  }
  inline bool operator==(const AidBitmap& rhs) const {
    return m_bitField == rhs.m_bitField;
  }

private:
  u32 m_bitField;
};

} // namespace Net
