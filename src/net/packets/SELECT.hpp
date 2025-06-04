// Credits: Melg, used as reference for naming, see bottom of file for the licenses
// https://github.com/MelgMKW/Pulsar/blob/main/GameSource/MarioKartWii/RKNet/SELECT.hpp

#pragma once

#include <rk_types.h>

namespace Net {

struct SELECTPlayer {
  u16 prevRaceSpot;
  u16 totalPoints;
  u8 characterId;
  u8 vehicle;
  u8 votedCourse;
  u8 starRank;
};
static_assert(sizeof(SELECTPlayer) == 0x8);

struct SELECTPacket {
  u64 timeSent;
  u64 timeRecieved;
  SELECTPlayer m_players[2];
  u32 m_selectId;
  u8 _24[0x28 - 0x24];
  u8 m_playerIdToAidMap[12];
  u8 m_selectedCourseId;
  u8 m_votingPhase;
  u8 m_aidOfWinningVoter;
  u8 m_engineClass;
};
static_assert(sizeof(SELECTPacket) == 0x38);

// TODO: Investegate why this is similar to the enum in NetManager, but have completely different values??
enum TypeOfRoom {
  PUBLIC_VS = 0x1,
  PUBLIC_BATTLE = 0x2,
  PRIVATE_VS = 0x3,
  PRIVATE_BATTLE = 0x4,
};

class SELECTHandler {
private:
  TypeOfRoom m_typeOfRoom;
  u8 _004[0x008 - 0x004];
  SELECTPacket m_sendPacket;
  SELECTPacket m_recvPacket[12];
  u8 _2e0[0x3f8 - 0x2e0];
};
static_assert(sizeof(SELECTHandler) == 0x3f8);

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


