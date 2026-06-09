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
#include "AudioManager.h"


// ---- Forward declarations ----

class TilePiece;


// ---- Class Definition ----

/**
 * JUCE Component that renders the 10-column x 7-row pipe-tile grid and handles
 * tile-placement mouse input.
 *
 * Tile size is derived from the component's own bounds so MainComponent can
 * position and (in portrait) rotate this component freely via setBounds() and
 * setTransform(). JUCE automatically applies the inverse transform to mouse
 * events, so mouseDown() always receives coordinates in local (unrotated) space.
 *
 * The three static helper methods are also called by QueueComponent so that
 * tile rendering is not duplicated.
 */
class BoardComponent : public juce::Component
{
public:
    BoardComponent();

    void resized() override;
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;

    /**
     * Decrement the click-block counter. Call once per timer tick from
     * MainComponent::timerCallback() to re-enable tile placement after a click.
     */
    void Tick();

    /**
     * Reset the click-block counter to zero. Call when a new round begins.
     */
    void ResetInteraction();

    /**
     * Returns the tile side length in pixels computed from the current bounds.
     * Valid after the first resized() call.
     */
    int GetTileSize() const;

    // ---- Shared tile-drawing helpers (used by QueueComponent) ----

    /**
     * Draw the pipe shape (background fill + pipe lines) for one tile.
     * Does nothing for TYPE_NONE tiles.
     */
    static void DrawTile(TilePiece* tile, juce::Point<int> origin, int tileSize, juce::Graphics& g);

    /**
     * Draw the second channel of a cross-pipe (foreground pipe, separator lines,
     * and any ooze flowing through that channel).
     */
    static void DrawCrossSecondWay(TilePiece* tile, juce::Point<int> origin, int tileSize, juce::Graphics& g);

    /**
     * Draw the tile border and any active explosion animation.
     */
    static void DrawTileDecoration(TilePiece* tile, juce::Point<int> origin, int tileSize, juce::Graphics& g);

private:
    static constexpr float OOZE_THICKNESS = 15.0f;

    void DrawOoze(TilePiece* tile, juce::Point<int> origin, int tileSize, juce::Graphics& g);
    void DrawSpill(juce::Point<int> origin, int tileSize, juce::Graphics& g);

    int m_tileSize = 0;
    int m_blockInteraction = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoardComponent)
};
