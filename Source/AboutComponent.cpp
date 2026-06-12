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


#include "AboutComponent.h"


// ---- Class Implementation ----

AboutComponent::AboutComponent(const char* imageData, int imageDataSize)
{
    m_appIcon = std::make_unique<juce::DrawableButton>("App Icon",
                                                       juce::DrawableButton::ButtonStyle::ImageFitted);
    m_appIcon->setColour(juce::DrawableButton::ColourIds::backgroundColourId,
                         juce::Colours::transparentBlack);
    m_appIcon->setColour(juce::DrawableButton::ColourIds::backgroundOnColourId,
                         juce::Colours::transparentBlack);
    m_appIcon->setImages(
        juce::Drawable::createFromImageData(imageData, imageDataSize).get());
    addAndMakeVisible(m_appIcon.get());

    m_appInfoLabel = std::make_unique<juce::Label>("Version",
        juce::JUCEApplication::getInstance()->getApplicationName() + " " +
        juce::JUCEApplication::getInstance()->getApplicationVersion());
    m_appInfoLabel->setJustificationType(juce::Justification::centredBottom);
    m_appInfoLabel->setFont(juce::Font(juce::FontOptions(16.0f, juce::Font::plain)));
    addAndMakeVisible(m_appInfoLabel.get());

    m_appRepoLink = std::make_unique<juce::HyperlinkButton>(
        juce::JUCEApplication::getInstance()->getApplicationName() + " on GitHub",
        juce::URL("https://github.com/christianahrens/PipeDreamer"));
    m_appRepoLink->setFont(juce::Font(juce::FontOptions(16.0f, juce::Font::plain)), false);
    m_appRepoLink->setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(m_appRepoLink.get());
}

void AboutComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    juce::Component::paint(g);
}

void AboutComponent::resized()
{
    auto bounds = getLocalBounds();
    const int margin = bounds.getHeight() / 8;
    bounds.reduce(margin, margin);

    auto iconBounds = bounds.removeFromTop(bounds.getHeight() / 2);
    auto infoBounds = bounds.removeFromTop(bounds.getHeight() / 2);

    m_appIcon->setBounds(iconBounds);
    m_appInfoLabel->setBounds(infoBounds);
    m_appRepoLink->setBounds(bounds);
}
