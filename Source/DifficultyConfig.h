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


#pragma once


/**
 * @namespace Difficulty
 * @brief Per-level tuning parameters for game difficulty progression.
 *
 * All values were originally calibrated for 12 levels. To add or change a
 * level, edit the table in DifficultyConfig.cpp. No other file needs touching.
 */
namespace Difficulty
{
    /**
     * Per-level configuration parameters.
     * All timing is expressed in timer ticks (one tick = Layout::GUI_REFRESH_RATE ms).
     */
    struct LevelConfig
    {
        /** Ooze units added per timer tick once the countdown reaches zero. */
        float oozePerPump;

        /** Timer ticks before ooze starts flowing at the beginning of a round. */
        int   countdown;
    };

    /**
     * Return the configuration for a given 1-based difficulty level.
     * If @p level exceeds the number of defined levels, the last entry is returned.
     *
     * @param level  Difficulty level, starting at 1.
     * @return       Reference to the corresponding LevelConfig.
     */
    const LevelConfig& GetLevelConfig(int level);

    /**
     * Total number of distinct difficulty levels defined in the config table.
     */
    int GetNumLevels();
}
