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


#include "GameRenderer.h"
#include "LayoutConstants.h"
#include "Board.h"
#include "Controller.h"


// ---- Class Implementation ----

GameRenderer::GameRenderer()
{
}

void GameRenderer::SetLayout(juce::Rectangle<int> headerBounds, juce::Rectangle<int> fontRefBounds)
{
    m_headerBounds  = headerBounds;
    m_fontRefBounds = fontRefBounds;
}

void GameRenderer::Render(juce::Graphics& g)
{
    juce::Graphics::ScopedSaveState save(g);
    g.setOrigin(m_headerBounds.getX(), m_headerBounds.getY());
    DrawHeader(g);
}

juce::Font GameRenderer::GetFont(LabelID labelID) const
{
    // Scale fonts from the virtual landscape reference bounds so that text sizes
    // remain consistent regardless of orientation or zone size.
    int minDimension = std::min<int>(
        static_cast<int>(m_fontRefBounds.getWidth() / Layout::ASPECT_RATIO),
        m_fontRefBounds.getHeight());

    switch (labelID)
    {
    case LABEL_VERSION:
        return { juce::FontOptions("consolas", minDimension * Layout::FONT_VERSION_PT / Layout::FONT_REF_HEIGHT, juce::Font::plain) };
    case LABEL_SCORE:
        return { juce::FontOptions("consolas", minDimension * Layout::FONT_SCORE_PT / Layout::FONT_REF_HEIGHT, juce::Font::plain) };
    case LABEL_BSCORE:
        return { juce::FontOptions("consolas", minDimension * Layout::FONT_SCORE_PT / Layout::FONT_REF_HEIGHT, juce::Font::bold) };
    }

    return { juce::FontOptions() };
}

juce::Colour GameRenderer::GetTileColourForLevel(int difficultyLevel)
{
    static const juce::Colour colorsPerLevel[] = {
        juce::Colour(125, 125, 125),    // Level 1
        juce::Colours::cadetblue,       // Level 2
        juce::Colours::darkkhaki,       // Level 3
        juce::Colour(140, 180, 90),     // Level 4
        juce::Colours::darkslategrey,   // Level 5
        juce::Colours::hotpink,         // Level 6
        juce::Colour(27, 122, 165),     // Level 7
        juce::Colours::coral,           // Level 8
        juce::Colours::blueviolet,      // Level 9
        juce::Colours::darkorange,      // Level 10
        juce::Colours::mediumseagreen,  // Level 11
        juce::Colours::orangered,       // Level 12
    };

    int arraySize = static_cast<int>(sizeof(colorsPerLevel) / sizeof(*colorsPerLevel));
    difficultyLevel--;
    if (difficultyLevel < 0)           difficultyLevel = 0;
    if (difficultyLevel >= arraySize)  difficultyLevel = arraySize - 1;

    return colorsPerLevel[difficultyLevel];
}

void GameRenderer::DrawHeader(juce::Graphics& g)
{
    Controller* controller = Controller::GetInstance();
    int playerScore = controller->GetBoard()->GetScoreValue();
    bool thresholdReached = (playerScore >= Controller::MIN_SCORE_TO_ADVANCE);

    const int W = m_headerBounds.getWidth();
    const int H = m_headerBounds.getHeight();

    // ---- Bombs: right-aligned, sized to ~40% of header height ----
    Board* board         = controller->GetBoard();
    int bombDiameter     = juce::jlimit(10, H - 8, (H * 4) / 10);
    int bombStep         = bombDiameter + 4;
    int totalBombsW      = Board::MAX_NUM_BOMBS * bombStep - 4; // no trailing gap
    int bombStartX       = W - totalBombsW - 12;

    for (int i = 0; i < Board::MAX_NUM_BOMBS; i++)
    {
        juce::Rectangle<int> bombRect(bombStartX + i * bombStep,
                                      (H - bombDiameter) / 2,
                                      bombDiameter, bombDiameter);
        if (i < board->GetNumBombs())
        {
            g.setColour(juce::Colours::red);
            g.fillEllipse(bombRect.toFloat());
        }
        else if (i == board->GetNumBombs())
        {
            g.setColour(juce::Colour(static_cast<juce::uint8>(67 + board->GetPercentUntilFreeBomb()), 67, 67));
            g.fillEllipse(bombRect.toFloat());
        }
        g.setColour(juce::Colours::white);
        g.drawEllipse(bombRect.toFloat(), 1.0f);
    }

    // ---- Labels: left-to-right flow, clipped before the bomb area ----
    auto plainFont = GetFont(LABEL_SCORE);
    auto boldFont  = GetFont(LABEL_BSCORE);
    const int maxLabelRight = bombStartX - 8; // don't overlap bombs
    const int smallGap = 6;
    const int medGap   = 16;
    int x = 12; // left margin

    auto drawItem = [&](const juce::String& text, juce::Font font, juce::Colour colour)
    {
        int textW = juce::GlyphArrangement::getStringWidthInt(font, text) + 4;
        if (x + textW > maxLabelRight)
            return; // no room — skip rather than clip
        g.setFont(font);
        g.setColour(colour);
        g.drawText(text, x, 0, textW, H, juce::Justification::centredLeft, false);
        x += textW;
    };

    drawItem("Level:",
             plainFont,
             juce::Colours::grey);
    x += smallGap;

    drawItem(juce::String(controller->GetDifficultyLevel()),
             plainFont,
             GetTileColourForLevel(controller->GetDifficultyLevel()));
    x += medGap;

    drawItem("Score:",
             plainFont,
             juce::Colours::grey);
    x += smallGap;

    drawItem(juce::String(playerScore),
             thresholdReached ? boldFont : plainFont,
             thresholdReached ? juce::Colours::yellow : juce::Colours::grey);
}
