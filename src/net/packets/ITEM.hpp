// Credits: Melg, used as reference for naming, see bottom of file for the licenses
// https://github.com/MelgMKW/Pulsar/blob/main/GameSource/MarioKartWii/RKNet/ITEM.hpp

#pragma once

#include <rk_types.h>

namespace Net {

struct ITEMPacket {
  u8 timer;
  u8 itemBox;
  u8 itemTail;
  u8 mode;
  u8 tailMode;
  u8 ack;
  u8 actTimer;
  u8 _7; // padding/unused
};
static_assert(sizeof(ITEMPacket) == 0x8);

enum DecidedItemStatus {
  DECIDED_ITEM_STATUS_NO_ITEM = 0x0,
  DECIDED_ITEM_STATUS_OBTAINABLE = 0x1,
  DECIDED_ITEM_STATUS_NOT_OBTAINABLE = 0x2, // 8065f254 
};

class ITEMHandler {
private:
  ITEMPacket m_sendPackets[2];
  ITEMPacket m_recvPackets[12];
  DecidedItemStatus playerStatuses[12];
  u32 m_someTimer[12]; // seems to be some kind of difference?
  u32 m_activeItemsTable[15][3]; // One per ItemId, [][0] is held, [][1] is for trailed items, not sure about [][2] (maybe something to do with limit)
};
static_assert(sizeof(ITEMHandler) == 0x184);

}

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
