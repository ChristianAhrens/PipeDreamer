/*
===============================================================================

Copyright (C) 2026 Christian Ahrens. All Rights Reserved.

  This file is part of the PipeDreamer fork maintained by Christian Ahrens, found at:
  https://github.com/christianahrens/PipeDreamer

  Original project by Bernardo Escalona:
  https://github.com/escalonely/PipeDreamer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

===============================================================================
*/


#include "DifficultyConfig.h"


namespace Difficulty
{

// ---- Difficulty table -------------------------------------------------------
// To tune the game, edit the values below. Columns: { oozePerPump, countdown }
// oozePerPump: units per timer tick (multiplied ×10 in fast-forward mode).
// countdown:   ticks of grace period before ooze flows (1 tick ≈ 60 ms).

static const LevelConfig kLevels[] =
{
    { 1.0f, 320 },  // Level  1
    { 1.2f, 290 },  // Level  2
    { 1.4f, 260 },  // Level  3
    { 1.5f, 230 },  // Level  4
    { 1.6f, 200 },  // Level  5
    { 1.8f, 180 },  // Level  6
    { 2.0f, 160 },  // Level  7
    { 2.2f, 140 },  // Level  8
    { 2.5f, 120 },  // Level  9
    { 3.0f, 100 },  // Level 10
    { 3.5f,  80 },  // Level 11
    { 5.0f,  60 },  // Level 12
};

static const int kNumLevels = static_cast<int>(sizeof(kLevels) / sizeof(*kLevels));

// ----------------------------------------------------------------------------

const LevelConfig& GetLevelConfig(int level)
{
    int idx = level - 1; // convert 1-based to 0-based
    if (idx < 0)
        idx = 0;
    if (idx >= kNumLevels)
        idx = kNumLevels - 1;

    return kLevels[idx];
}

int GetNumLevels()
{
    return kNumLevels;
}

} // namespace Difficulty
