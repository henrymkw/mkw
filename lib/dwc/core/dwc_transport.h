#pragma once

#include <rk_types.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL DWC_SendUnreliable(u8 aid, void *packet, u32 size);

#ifdef __cplusplus
}
#endif
