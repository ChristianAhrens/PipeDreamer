/*
===============================================================================

Copyright (C) 2026 Christian Ahrens. All Rights Reserved.

  This file is part of the PipeDreamer fork maintained by Christian Ahrens.

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
 * Wraps any Component as a resizable custom item inside a juce::PopupMenu.
 * getIdealSize() scales to 75% of the main display's smaller dimension
 * (matching the Umsci reference implementation), with a configurable minimum.
 */
class CustomAboutItem : public juce::PopupMenu::CustomComponent
{
public:
    CustomAboutItem(juce::Component* componentToHold, juce::Rectangle<int> minIdealSize)
        : m_component(componentToHold)
        , m_minIdealSize(minIdealSize)
    {
        addAndMakeVisible(m_component);
    }

    ~CustomAboutItem() override
    {
        if (m_component)
            m_component->setVisible(false);
    }

    void getIdealSize(int& idealWidth, int& idealHeight) override
    {
        auto resultingSize = juce::Rectangle<int>(idealWidth, idealHeight);

        auto* mc = juce::Desktop::getInstance().getComponent(0);
        if (mc)
        {
            auto fb = mc->getBounds().toFloat();
            float w = fb.getWidth(), h = fb.getHeight();
            if (w > 0.0f && h > 0.0f)
            {
                if (h > w) { w = 0.75f * w; h = w; }
                else       { h = 0.75f * h; w = h; }
                resultingSize = juce::Rectangle<float>(w, h).toNearestInt();
            }
        }

        if (resultingSize.getWidth()  < m_minIdealSize.getWidth() &&
            resultingSize.getHeight() < m_minIdealSize.getHeight())
        {
            idealWidth  = m_minIdealSize.getWidth();
            idealHeight = m_minIdealSize.getHeight();
        }
        else
        {
            idealWidth  = resultingSize.getWidth();
            idealHeight = resultingSize.getHeight();
        }
    }

    void resized() override
    {
        if (m_component)
            m_component->setBounds(getLocalBounds());
    }

private:
    juce::Component*     m_component    = nullptr;
    juce::Rectangle<int> m_minIdealSize;
};
