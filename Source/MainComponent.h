/*
===============================================================================

Copyright (C) 2021 Bernardo Escalona. All Rights Reserved.

  This file is part of Pipe Dreamer, found at:
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
#include "GameRenderer.h"
#include "BoardComponent.h"
#include "QueueComponent.h"
#include "ProgressComponent.h"


// ---- Forward declarations ----

class Controller;
class ScoreWindow;


// ---- Class Definition ----

/**
 * Root GUI component that occupies the entire game window.
 * Divides the window into four horizontal zones (header, content, progress, footer)
 * and positions child components within them for both landscape and portrait modes.
 */
class MainComponent : public juce::Component,
                      public juce::Timer,
                      public juce::ChangeListener
{
public:
    MainComponent();
    ~MainComponent() override;

    /** Returns the current tile side length from BoardComponent. Used by ScoreWindow. */
    int GetTileSize() const;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    std::unique_ptr<Controller>        m_controller;
    GameRenderer                       m_renderer;

    std::unique_ptr<BoardComponent>    m_boardComponent;
    std::unique_ptr<QueueComponent>    m_queueComponent;
    std::unique_ptr<ProgressComponent> m_progressComponent;

    std::unique_ptr<ScoreWindow>       m_scoreWindow;

    int m_countDown    = 0;
    int m_maxCountDown = 0;

    juce::CriticalSection m_lock;

    std::unique_ptr<juce::HyperlinkButton> m_hyperlink;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
