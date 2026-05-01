#include "User.hpp"

#include "net/NetManager.hpp"

#include <rfl/RFL_System.h>

namespace Net {

void UserRecord::reset() { memset(this, 0, sizeof(UserRecord)); }

void UserHandler::update() {
  bool hasMatch = NetManager::Instance()->hasFoundMatch();

  if (hasMatch) {
    if (!m_prepared) {
      prepareToSend();
    }
  } else {
    if (m_prepared) {
      m_prepared = false;
      _9e0 = 0;
      m_aidsThatSentUser = 0;
      _9e8 = 0;
      memset(&m_recordToSend, 0, sizeof(UserRecord));
      for (u32 i = 0; i < MAX_PLAYER_COUNT; i++) {
        m_recvRecords[i].reset();
      }
    }
  }

  if (m_prepared) {
    importRecords();
    if (_9dc == TRUE) {
      if (RFLGetAsyncStatus() != RFLErrcode_Busy) {
        _9e0 = _9e8;
        _9e8 = 0;
        _9dc = 0;
      }
    }
  }
}

} // namespace Net
