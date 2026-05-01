// Credits: Melg
// https://github.com/MelgMKW/Pulsar/blob/main/GameSource/core/rvl/DWC/DWCAccount.hpp

// MIT License

// Copyright (c) 2023 MelgMKW

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <rk_types.h>

#include <gamespy/GP/gp.h>

// again splits are wrong, just a very rough guess.
// documenting types and functions is more important than having 100% correct
// splits atp.

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  GPConnection* connection;
  u8 _004[0x890 - 0x004];
  u32 groupId;
  u8 _894[0x8c0 - 0x894];
} DWCMatchContext;

DWCMatchContext* s_MatchContext;

DWCMatchContext* DWCiGetMatchContextExt(); // this is what its named in Ghidra,
                                           // what does the Ext mean?

u32 DWC_GetConnectionUserData(u32 p1, u32* p2);

#ifdef __cplusplus
}
#endif
