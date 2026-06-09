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
 * Horizontal strip combining the ooze-countdown vial and the fast-forward toggle button.
 * Reads game state directly from Controller::GetInstance(). Call SetCountDown() each
 * timer tick so the vial fill stays in sync with the main countdown.
 */
class ProgressComponent : public juce::Component
{
public:
    ProgressComponent();

    /** Update the countdown values used to compute the ooze fill level. */
    void SetCountDown(int countDown, int maxCountDown);

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;

private:
    /** Returns the FF button rect in local component coordinates. */
    juce::Rectangle<int> GetFFButtonRect() const;

    int m_countDown    = 0;
    int m_maxCountDown = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgressComponent)
};
