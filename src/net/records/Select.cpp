#include "Select.hpp"

namespace Net {

const u8* SelectHandler::getPlayerIdToAidMapping() const {
  return m_sendPacket.m_playerIdToAidMapping;
}

} // namespace Net
