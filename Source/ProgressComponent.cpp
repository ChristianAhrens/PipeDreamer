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


#include "ProgressComponent.h"
#include "LayoutConstants.h"
#include "GameRenderer.h"
#include "Board.h"
#include "Controller.h"


// ---- ProgressComponent class implementation ----

ProgressComponent::ProgressComponent()
{
}

void ProgressComponent::SetCountDown(int countDown, int maxCountDown)
{
    m_countDown    = countDown;
    m_maxCountDown = std::max(1, maxCountDown);
}

juce::Rectangle<int> ProgressComponent::GetFFButtonRect() const
{
    int W = getWidth();
    int H = getHeight();
    int btnW = juce::jlimit(50, 90, W / 6);
    int btnH = juce::jlimit(H / 4, H - 8, btnW * 2 / 3);
    return juce::Rectangle<int>(W - btnW - 8, (H - btnH) / 2, btnW, btnH);
}

void ProgressComponent::paint(juce::Graphics& g)
{
    const int W = getWidth();
    const int H = getHeight();
    const int margin = 4;
    const int vialH = H - 2 * margin;
    const int vialX = 16;
    const int vialY = margin;

    // ---- Ooze vial ----
    juce::Rectangle<int> vialRect(vialX, vialY, Layout::OOZE_VIAL_WIDTH, vialH);
    g.setColour(juce::Colours::black);
    g.fillRect(vialRect);

    int oozeMaxH = vialH - Layout::OOZE_VIAL_PADDING * 2;
    int oozeH    = 0;
    juce::Colour oozeColour = juce::LookAndFeel::getDefaultLookAndFeel()
                                   .findColour(GameRenderer::pipeOozeColourId);

    Board* board = Controller::GetInstance()->GetBoard();
    if (m_countDown > 0)
    {
        oozeH = oozeMaxH - (m_countDown * oozeMaxH) / m_maxCountDown;
    }
    else if (board->GetScoreValue() < Controller::MIN_SCORE_TO_ADVANCE)
    {
        oozeH = ((Controller::MIN_SCORE_TO_ADVANCE - board->GetScoreValue()) * oozeMaxH)
                / Controller::MIN_SCORE_TO_ADVANCE;
    }
    else
    {
        oozeColour = juce::Colours::yellow;
        oozeH      = oozeMaxH;
    }

    g.setColour(oozeColour);
    g.fillRect(vialX + Layout::OOZE_VIAL_PADDING,
               vialY + Layout::OOZE_VIAL_PADDING + oozeMaxH - oozeH,
               Layout::OOZE_FILL_WIDTH, oozeH);

    // Tick marks (shadow then highlight)
    const float tx = static_cast<float>(vialX);
    const float ty = static_cast<float>(vialY);
    g.setColour(juce::Colours::black);
    g.drawLine(tx + 15.0f, 1.0f + ty + vialH * 0.25f, tx + 22.0f, 1.0f + ty + vialH * 0.25f, 2.0f);
    g.drawLine(tx + 10.0f, 1.0f + ty + vialH * 0.5f,  tx + 22.0f, 1.0f + ty + vialH * 0.5f,  2.0f);
    g.drawLine(tx + 15.0f, 1.0f + ty + vialH * 0.75f, tx + 22.0f, 1.0f + ty + vialH * 0.75f, 2.0f);
    g.setColour(juce::Colours::white);
    g.drawLine(tx + 15.0f, ty + vialH * 0.25f, tx + 22.0f, ty + vialH * 0.25f, 1.0f);
    g.drawLine(tx + 10.0f, ty + vialH * 0.5f,  tx + 22.0f, ty + vialH * 0.5f,  1.0f);
    g.drawLine(tx + 15.0f, ty + vialH * 0.75f, tx + 22.0f, ty + vialH * 0.75f, 1.0f);
    g.drawRect(vialRect);

    // ---- Fast-forward button ----
    auto ffRect = GetFFButtonRect();
    float radius = ffRect.getHeight() * 0.33f;
    float cx1 = ffRect.getX() + ffRect.getWidth() * 0.33f;
    float cx2 = ffRect.getX() + ffRect.getWidth() * 0.62f;
    float cy  = static_cast<float>(ffRect.getCentreY());

    juce::Path ffPath;
    ffPath.addPolygon(juce::Point<float>(cx1, cy), 3, radius, Layout::FF_ICON_ROTATION);
    ffPath.addPolygon(juce::Point<float>(cx2, cy), 3, radius, Layout::FF_ICON_ROTATION);

    bool ff = Controller::GetInstance()->GetFastForward();
    float thickness         = ff ? 2.5f : 1.5f;
    juce::Colour iconColour  = ff ? juce::Colours::red   : juce::Colours::grey;
    juce::Colour frameColour = ff ? juce::Colours::black : juce::Colour(27, 27, 27);

    g.setColour(iconColour);
    g.fillPath(ffPath);
    g.setColour(juce::Colours::white);
    g.strokePath(ffPath, juce::PathStrokeType(thickness, juce::PathStrokeType::curved));
    g.setColour(frameColour);
    g.drawRect(ffRect.toFloat(), thickness);

    (void)W; // used indirectly via GetFFButtonRect()
}

void ProgressComponent::mouseDown(const juce::MouseEvent& e)
{
    if (Controller::GetInstance()->GetState() != Controller::STATE_RUNNING)
        return;

    if (GetFFButtonRect().contains(e.getMouseDownPosition()))
        Controller::GetInstance()->SetFastForward(!Controller::GetInstance()->GetFastForward());
}
