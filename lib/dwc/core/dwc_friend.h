#pragma once

#include <rk_types.h>

#include <dwc/core/dwc_account.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  u8 localPlayerCount;
  u8 _1[0x4 - 0x1];
} DWCConnectionUserData;

// Is void * the right type? NetManager passes in StatusData.
u32 DWC_SetOwnStatusData(void* data, u32 unk);

// i think r5 is a pointer?
u8 DWC_GetFriendStatusData(DWCAccFriendData* friendData, void* r4, u32* r5);

#ifdef __cplusplus
}
#endif
