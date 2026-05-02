#pragma once

#include <rk_types.h>

#include <gamespy/GP/gp.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  u8 _00[0x40 - 0x00];
} DWCstAccUserData;

typedef struct {
  u32 _0;
  u32 _4;
  u32 _8;
} DWCstAccFriendData;

GPProfile DWC_GetGsProfileId(DWCstAccUserData* userData,
                             DWCstAccFriendData* friendData);

BOOL DWC_IsBuddyFriendData(DWCstAccFriendData* friendData);

#ifdef __cplusplus
}
#endif
