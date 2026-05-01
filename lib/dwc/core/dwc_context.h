#pragma once

#include <rk_types.h>

// unsure on the .text splits, currently just using the static pointer to make
// .text splits

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  u8 _0000[0x1438 - 0x0000];
} DWCContext;

typedef void (*DWCUpdateServersAsyncCallback)(u32, u32, void*);
typedef void (*DWCUpdateServersBLR)();

void DWC_ShutdownFriendsMatch();
void DWC_ProcessFriendsMatch();
void DWC_UpdateServersAsync(u32 r3, DWCUpdateServersAsyncCallback updateCB,
                            void* callersObj, DWCUpdateServersBLR cb2,
                            void* cb2Obj, u32 unk2, u32 unk3);

#ifdef __cplusplus
}
#endif
