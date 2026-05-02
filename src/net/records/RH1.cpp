#include "RH1.hpp"

#include "net/NetManager.hpp"

#include <rk_common.h>

namespace Net {

void RH1Handler::setPrepared() {
  // setting this to true allows the main logic of this class's calc() to run()
  m_prepared = true;
}

s32 RH1Handler::getCourseId() const {
  s32 otherCourseId;
  u32 adjustedCourseId;
  s32 result;
  for (u8 i = 0; i < MAX_PLAYER_COUNT; i++) {
    const RH1Player* data = &m_RH1Players[i];
    adjustedCourseId = data->courseId;
    if (adjustedCourseId <= 0x42) {
      adjustedCourseId = data->courseId;
      otherCourseId = data->courseId;
    } else {
      otherCourseId = -1;
    }

    if (otherCourseId != -1 && data->timer != 0) {
      if (adjustedCourseId <= 0x42) {
        return data->courseId;
      } else {
        return -1;
      }
    }
  }
  return -1;
}

// https://decomp.me/scratch/zMnhg
bool RH1Handler::courseValid() const {
  bool result;
  s32 adjustedCourseId;
  System::CourseId courseId;

  if (!NetManager::Instance()->hasFoundMatch()) {
    result = false;
  } else {
    if (m_receivedRH1FromAid.any()) {
      NetManager* netManager = NetManager::Instance();
      NetManager::MatchMakingInfo* matchMakingInfo = netManager->mmInfo();
      AidBitmap<u8> myAidSlot = 1 << matchMakingInfo->myAid;
      AidBitmap<u8> availableAids = matchMakingInfo->availableAids;
      myAidSlot = availableAids & (m_receivedRH1FromAid | myAidSlot);
      result = (availableAids == myAidSlot);
    } else {
      result = false;
    }
  }
  if (result) {
    return getCourseId() != -1;
  }
  return result;
}

} // namespace Net
