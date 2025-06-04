// Credits: Melg, used as reference for naming, see bottom of file for the licenses
// https://github.com/MelgMKW/Pulsar/blob/main/GameSource/MarioKartWii/RKNet/RH1.hpp

#pragma once

#include <rk_types.h>

namespace Net {

struct RACEHEADER1Packet {
  u32 timer;
  u32 selectId;
  u16 team[2];
  u16 lagFrames;
  u8 player1Vehicle;
  u8 player1Character;
  u8 player2Vehicle;
  u8 player2Character;
  u16 countdownTime;
  u8 player1StarRank;
  u8 player2StarRank;
  u8 playedCourse;
  u8 typeOfPlayer; // wiki says 0 for regular player, 2 for spectator
  u8 playerIdToAidMap[12];
  u8 engineClass;
  u8 _25[0x28 - 0x25];
};
static_assert(sizeof(RACEHEADER1Packet) == 0x28);

// Similar to the structure to the packet but less condenced
struct RACEHEADER1Data {
  u32 timer;
  u32 vehicleIds[2];
  u32 characterIds[2];
  u32 courseId;
  u32 team;
  u32 selectId;
  u8 playerIdToAidMap[12];
  u8 engineClass;
  u8 starRank[2];
  u8 _2f;
};
static_assert(sizeof(RACEHEADER1Data) == 0x30);

class RACEHEADER1Handler {
public:
  static RACEHEADER1Handler *getInstance() {
    return spInstance;
  }

  void reset();

  void setPrepared();

private:
  bool m_prepared;
  u8 _000[0x020 - 0x004];
  RACEHEADER1Data m_datas[12];
  static RACEHEADER1Handler *spInstance;
};
static_assert(sizeof(RACEHEADER1Handler) == 0x260);

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
