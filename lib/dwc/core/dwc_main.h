#pragma once

#include <rk_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  u8 _0000[0x1438 - 0x0000];
} DWCstControl;

void DWC_UpdateServersAsync(u32 r3, void* r4, void* r5, void* r6, void* r7,
                            u32 r8, u32 r9);

#ifdef __cplusplus
}
#endif
