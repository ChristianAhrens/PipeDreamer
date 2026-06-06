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


// ---- Forward declarations ----

class TilePiece;


// ---- Class Definition ----

/**
 * Stateless renderer responsible for all game drawing.
 * MainComponent owns one instance and calls SetLayout() on every resize,
 * then Render() on every paint.
 */
class GameRenderer
{
public:
    /**
     * ID of a label whose font size this renderer can compute.
     */
    enum LabelID
    {
        LABEL_SCORE = 0,    //< Plain score/level text.
        LABEL_BSCORE,       //< Bold score text, shown when the advance threshold is reached.
        LABEL_VERSION       //< Small version hyperlink label.
    };

    /**
     * Class constructor.
     */
    GameRenderer();

    /**
     * Update the stored window bounds and recompute derived layout values (tile size, button rect).
     * Must be called whenever the parent component is resized.
     *
     * @param windowBounds  The new bounds of the parent window, in local coordinates.
     */
    void SetLayout(juce::Rectangle<int> windowBounds);

    /**
     * Draw the entire game scene into the given graphics context.
     *
     * @param g         The graphics context to draw into.
     * @param countDown Remaining countdown ticks until ooze starts pumping (used by the ooze meter).
     */
    void Render(juce::Graphics& g, int countDown);

    /**
     * Get the computed tile side length, in pixels.
     *
     * @return Tile size in pixels, or 0 if SetLayout() has not been called yet.
     */
    int GetTileSize() const;

    /**
     * Get the screen rectangle occupied by the fast-forward button.
     *
     * @return Rectangle in the parent window's local coordinate space.
     */
    juce::Rectangle<int> GetFastForwardButtonRect() const;

    /**
     * Get the font to use for the given label, scaled to the current window size.
     *
     * @param labelID   Which label to compute a font for.
     * @return          A scaled juce::Font.
     */
    juce::Font GetFont(LabelID labelID) const;

    /**
     * Get the tile background colour for a given difficulty level.
     *
     * @param difficultyLevel   Level number starting at 1.
     * @return                  Colour used to fill pipe tile backgrounds at that level.
     */
    static juce::Colour GetTileColourForLevel(int difficultyLevel);

private:
    static constexpr float OOZE_THICKNESS = 15.0f;

    void DrawLevelAndScore(juce::Graphics& g);
    void DrawTile(TilePiece* tile, juce::Point<int> origin, juce::Graphics& g);
    void DrawOoze(TilePiece* tile, juce::Point<int> origin, juce::Graphics& g);
    void DrawCrossSecondWay(TilePiece* tile, juce::Point<int> origin, juce::Graphics& g);
    void DrawTileDecoration(TilePiece* tile, juce::Point<int> origin, juce::Graphics& g);
    void DrawSpill(juce::Point<int> origin, juce::Graphics& g);
    void DrawOozeMeter(juce::Point<int> origin, juce::Graphics& g, int countDown);
    void DrawBombs(juce::Point<int> origin, juce::Graphics& g);
    void DrawFastForwardButton(juce::Graphics& g);

    juce::Rectangle<int> m_windowBounds;
    int m_tileSize = 0;
    juce::Rectangle<int> m_fastForwardButtonRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GameRenderer)
};
