#pragma once

#include <rk_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  GPConnection* connection;
  u8 _004[0x890 - 0x004];
  u32 groupId;
  u8 _894[0x8c0 - 0x894];
} DWCstMatchControl;

// Unofficial name
typedef struct {
  u8 _0;
  u8 _1;
  u8 _2;
  u8 _3;
} DWCConnectionUserData;

u32 DWC_GetGroupId();

DWCstMatchControl* DWCi_GetMatchCntExt();

u32 DWC_GetConnectionUserData(u32 p1, u32* p2);

#ifdef __cplusplus
}
#endif
