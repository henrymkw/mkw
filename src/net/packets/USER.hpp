// Credits: Melg, used as reference for naming, see bottom of file for the licenses
// https://github.com/MelgMKW/Pulsar/blob/main/GameSource/MarioKartWii/RKNet/USER.hpp

#pragma once

#include <rk_types.h>

namespace Net {

struct USERPacket {
  u8 _00[0xa0 - 0x00]; // TODO: Define RFL::Packet
  u64 wiiFriendCode;
  u64 friendCode;
  u8 country;
  u8 state;
  u16 city;
  u16 longitude;
  u16 latitude;
  u16 vr;
  u16 br;
  char gameRegion;
  u8 regionId;
  u8 _be[0xc0 - 0xbe];
};
static_assert(sizeof(USERPacket) == 0xc0);

class USERHandler {
private:
  bool m_prepared;
  u8 _001[0x008 - 0x001];
  USERPacket m_sendPacket;
  USERPacket m_recvPackets[12];
  u8 _000[0x9f0 - 0x9c8];
};
static_assert(sizeof(USERHandler) == 0x9f0);

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
