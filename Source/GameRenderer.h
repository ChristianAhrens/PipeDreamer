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

#include <JuceHeader.h>


// ---- Class Definition ----

/**
 * Stateless header renderer. Draws Level, Score, and Bomb indicators into the
 * header strip at the top of the window.
 *
 * MainComponent calls SetLayout() on every resize, then Render() on every paint.
 * The ooze meter and FF button live in ProgressComponent.
 */
class GameRenderer
{
public:
    /** ID of a label whose font size this renderer can compute. */
    enum LabelID
    {
        LABEL_SCORE = 0,    //< Plain score/level text.
        LABEL_BSCORE,       //< Bold score text, shown when the advance threshold is reached.
        LABEL_VERSION       //< Small version / footer label.
    };

    GameRenderer();

    /**
     * Update layout parameters. Must be called on every parent resize.
     *
     * @param headerBounds  Area in the window occupied by the header strip.
     * @param fontRefBounds Full virtual-landscape window bounds used only for
     *                      consistent font scaling across portrait/landscape modes.
     *                      Pass the full window bounds in landscape; in portrait
     *                      pass a rectangle with width=realHeight, height=realWidth.
     */
    void SetLayout(juce::Rectangle<int> headerBounds, juce::Rectangle<int> fontRefBounds);

    /** Draw the header (level, score, bombs) into @p g. */
    void Render(juce::Graphics& g);

    /** Get the font for the given label, scaled to the current window size. */
    juce::Font GetFont(LabelID labelID) const;

    /**
     * Get the tile background colour for a given difficulty level.
     * Static so that BoardComponent and QueueComponent can call it without a renderer instance.
     */
    static juce::Colour GetTileColourForLevel(int difficultyLevel);

private:
    void DrawHeader(juce::Graphics& g);

    /** Area in the parent window where header elements are positioned. */
    juce::Rectangle<int> m_headerBounds;

    /** Virtual landscape bounds — used only for font scaling (see SetLayout). */
    juce::Rectangle<int> m_fontRefBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GameRenderer)
};
