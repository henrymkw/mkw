#pragma once

#include <rk_types.h>

typedef enum {
    RFLErrcode_Busy = 0x6,
} RFLErrcode;

RFLErrcode RFLGetAsyncStatus();
